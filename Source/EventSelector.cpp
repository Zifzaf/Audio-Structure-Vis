/*
  ==============================================================================

    EventSelector.cpp
    Created: 7 Nov 2022 8:17:50am
    Author:  andreas

  ==============================================================================
*/

#include "EventSelector.h"

EventSelector::EventSelector() : thirdOctaveSpectrogarm(numberOfBands), audioData(*new juce::AudioBuffer<float>(0, 0))
{
  double factor = std::pow(2.0, 1.0 / 3.0);
  double factorInv = 1.0 / factor;
  for (auto i = 16; i >= 0; i--)
  {
    bandCuts[i] = std::pow(factorInv, 17 - i) * 1000.0;
  }
  bandCuts[17] = 1000.0;
  for (auto i = 18; i < numberOfBands - 1; i++)
  {
    bandCuts[i] = std::pow(factor, i - 17) * 1000.0;
  }

  addAndMakeVisible(&blockSize);
  blockSize.addItem("4096", 12);
  blockSize.addItem("8192", 13);
  blockSize.addItem("16384", 14);
  blockSize.addItem("32768", 15);
  blockSize.setSelectedId(13, juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&calcButton);
  calcButton.setButtonText("Calculate");
  calcButton.onClick = [this]
  { calcButtonClicked(); };
  calcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&thirdOctaveSpectrogarm);
  thirdOctaveSpectrogarm.setFramesPerSecond(30);

  addAndMakeVisible(&phon);
  phon.setText("80");
  phon.setInputRestrictions(2, "0123456789");
  phon.setMultiLine(false);

  addAndMakeVisible(&corrected);
  corrected.setState(juce::Button::buttonNormal);
  corrected.setButtonText("Loudness Correction");
  corrected.triggerClick();
}
EventSelector::~EventSelector()
{
}

inline size_t EventSelector::getBlockSize()
{
  return blockSize.getText().getIntValue();
}

void EventSelector::calcButtonClicked()
{
  if (audioAvailable.get() && calculating.compareAndSetBool(true, false))
  {

    calcButton.setEnabled(false);
    fileInput->getAudioBlock(&audioData);
    size_t blockSize = getBlockSize();
    auto segmentLength = fileInput->getSegmentLength();
    auto fftBins = blockSize / 2 - 2;
    auto bandIndex = new int[fftBins];
    auto freqScale = new double[fftBins];
    auto freqStep = (fileInput->getSampleRate() / 2.0) / (blockSize / 2);
    int k = 0;
    bool loudnessCorrection = getLoudnessCorrection();
    double phon = getPhon();
    // std::cout << loudnessCorrection << std::endl;
    for (auto i = 0; i < fftBins; i++)
    {
      if (k < numberOfBands - 1 && (i + 1) * freqStep > bandCuts[k])
      {
        k++;
      }
      bandIndex[i] = k;
      if (loudnessCorrection)
      {
        freqScale[i] = Loudness::getScaleFactor(phon, (i + 1) * freqStep);
      }
      else
      {
        freqScale[i] = 1.0;
      }
    }
    kfr::univector<kfr::complex<float>> &dftOutData = *new kfr::univector<kfr::complex<float>>(blockSize);
    kfr::dft_plan_real<float> dft(blockSize);
    kfr::univector<kfr::u8> &temp = *new kfr::univector<kfr::u8>(dft.temp_size);
    const float *inData = audioData.getReadPointer(0);
    auto numBlocks = segmentLength / (blockSize / 2) + 1;
    float *inDataBlock = new float[blockSize];
    auto complexBins = new kfr::complex<double>[numberOfBands];
    auto outputData = new float[numBlocks * numberOfBands];
    juce::FloatVectorOperations::fill(outputData, 0.0f, (size_t)numBlocks * numberOfBands);
    juce::dsp::WindowingFunction<float> window(blockSize, juce::dsp::WindowingFunction<float>::kaiser, true, 3.0);
    for (auto i = 0; i < segmentLength - blockSize; i = i + blockSize / 2)
    {
      juce::FloatVectorOperations::copy(inDataBlock, &inData[i], blockSize);
      window.multiplyWithWindowingTable(inDataBlock, blockSize);
      for (auto j = 0; j < numberOfBands; j++)
      {
        complexBins[j] = 0.0;
      }
      dft.execute(dftOutData, kfr::make_univector(inDataBlock, blockSize), temp);
      int blockIndex = i / (blockSize / 2);
      for (auto j = 0; j < fftBins; j++)
      {
        complexBins[bandIndex[j]] = complexBins[bandIndex[j]] + freqScale[j] * dftOutData[j + 1] / blockSize;
      }
      for (auto j = 0; j < numberOfBands; j++)
      {
        outputData[blockIndex * numberOfBands + j] = kfr::cabs(complexBins[j]);
      }
    }

    thirdOctaveSpectrogarm.replaceData(outputData, numBlocks, false, false);
    delete bandIndex;
    delete &dftOutData;
    delete complexBins;
    delete &temp;
    delete inDataBlock;
    delete outputData;
    calcButton.setEnabled(true);
    calculating.set(false);
  }
}

void EventSelector::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == fileInput)
  {
    audioAvailable.set(false);
    auto len = fileInput->getSegmentLength();
    if (len > audioData.getNumSamples())
    {
      delete &audioData;
      audioData = *new juce::AudioBuffer<float>(1, len);
    }
    audioAvailable.set(true);
  }
}

void EventSelector::paintOverChildren(juce::Graphics &g)
{
  int width = getWidth() - 4;
  int height = getHeight();
  g.setColour(juce::Colours::grey);
  int bandTableHeight = height - 46;
  int bandLabelHeight = bandTableHeight / numberOfBands;
  int rest = bandTableHeight % numberOfBands;
  auto h = height - bandLabelHeight;
  if (rest > 0)
  {
    rest--;
    h--;
  }
  for (auto i = 0; i < numberOfBands; ++i)
  {
    g.drawText(std::to_string((int)bandCuts[i]) + " Hz", 22, h - 10, 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, h, width, 1);
    h = h - bandLabelHeight;
    if (rest > 0)
    {
      rest--;
      h--;
    }
  }
  /*  // 100 Hz line
  if (rest >= 2)
  {
    g.drawText("100 Hz", 22, height - (2 * bandLabelHeight + 2 + 2 + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (2 * bandLabelHeight + 2 + 2), width, 1);
  }
  else
  {
    g.drawText("100 Hz", 22, height - (2 * bandLabelHeight + rest + 2 + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (2 * bandLabelHeight + rest + 2), width, 1);
  }
  // 1000 Hz line
  int shift = (1000.0 - 920.0) / (1080.0 - 920.0) * bandLabelHeight;
  if (rest >= 9)
  {
    g.drawText("1000 Hz", 22, height - (9 * bandLabelHeight + 9 + 2 + shift + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (9 * bandLabelHeight + 9 + 2 + shift), width, 1);
  }
  else
  {
    g.drawText("1000 Hz", 22, height - (9 * bandLabelHeight + rest + 2 + shift + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (9 * bandLabelHeight + rest + 2 + shift), width, 1);
  }
  // 10000 Hz line
  shift = (10000.0 - 9500.0) / (12000.0 - 9500.0) * bandLabelHeight;
  if (rest >= 23)
  {
    g.drawText("10000 Hz", 22, height - (23 * bandLabelHeight + 23 + 2 + shift + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (23 * bandLabelHeight + 23 + 2 + shift), width, 1);
  }
  else
  {
    g.drawText("10000 Hz", 22, height - (23 * bandLabelHeight + rest + 2 + shift + 10), 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, height - (23 * bandLabelHeight + rest + 2 + shift), width, 1);
  }*/
  float fraction = (fileInput->getCurrentTime() - fileInput->getStartTime()) / (fileInput->getEndTime() - fileInput->getStartTime());
  fraction = std::max(0.0f, fraction);
  g.setColour(juce::Colours::white);
  int pos = fraction * (width);
  g.fillRect(pos - 1, 46, 2, height - 46);
}

void EventSelector::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void EventSelector::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

double EventSelector::getPhon()
{
  return phon.getText().getDoubleValue();
}

bool EventSelector::getLoudnessCorrection()
{
  return corrected.getToggleState();
}

void EventSelector::resized()
{
  int width = getWidth() - 4;
  int height = getHeight();
  blockSize.setBounds(0, 2, width / 3, 20);
  phon.setBounds(width / 3, 2, width / 3, 20);
  corrected.setBounds(2 * width / 3, 2, width / 3, 20);
  calcButton.setBounds(2, 24, width, 20);
  thirdOctaveSpectrogarm.setBounds(2, 46, width, height - 46);
}