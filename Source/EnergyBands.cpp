/*
  ==============================================================================

    EnergyBands.cpp
    Created: 28 Oct 2022 11:17:58am
    Author:  andreas

  ==============================================================================
*/

#include "EnergyBands.h"

EnergyBands::EnergyBands() : energyTable(numberOfBands), audioData(*new juce::AudioBuffer<float>(0, 0))
{

  addAndMakeVisible(&blockSize);
  blockSize.addItem("128", 7);
  blockSize.addItem("256", 8);
  blockSize.addItem("512", 9);
  blockSize.addItem("1024", 10);
  blockSize.addItem("2048", 11);
  blockSize.addItem("4096", 12);
  blockSize.addItem("8192", 13);
  blockSize.addItem("16384", 14);
  blockSize.setSelectedId(12, juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&calcButton);
  calcButton.setButtonText("Calculate");
  calcButton.onClick = [this]
  { calcButtonClicked(); };
  calcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&energyTable);
  energyTable.setFramesPerSecond(30);

  addAndMakeVisible(&phon);
  phon.setText("80");
  phon.setInputRestrictions(2, "0123456789");
  phon.setMultiLine(false);

  addAndMakeVisible(&corrected);
  corrected.setState(juce::Button::buttonNormal);
  corrected.setButtonText("Loudness Correction");
  corrected.triggerClick();
}
EnergyBands::~EnergyBands()
{
}

inline size_t EnergyBands::getBlockSize()
{
  return blockSize.getText().getIntValue();
}

void EnergyBands::calcButtonClicked()
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
    auto complexBins = new kfr::complex<double>[numberOfBands];
    auto freqStep = (fileInput->getSampleRate() / 2.0) / (blockSize / 2);
    int k = 0;
    bool loudnessCorrection = getLoudnessCorrection();
    double phon = getPhon();
    juce::dsp::WindowingFunction<float> window(blockSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
    for (auto i = 0; i < fftBins; i++)
    {
      if (k < numberOfBands - 1 && (i + 1) * freqStep > criticalBandCuts[k])
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
    auto outputData = new float[numBlocks * numberOfBands];
    juce::FloatVectorOperations::fill(outputData, 0.0f, (size_t)numBlocks * numberOfBands);
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
    energyTable.replaceData(outputData, numBlocks, false, true);
    delete bandIndex;
    delete complexBins;
    delete &dftOutData;
    delete &temp;
    delete inDataBlock;
    delete outputData;
    calcButton.setEnabled(true);
    calculating.set(false);
  }
}

void EnergyBands::changeListenerCallback(juce::ChangeBroadcaster *source)
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

void EnergyBands::paintOverChildren(juce::Graphics &g)
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
    g.drawText(std::to_string((int)criticalBandCuts[i]) + " Hz", 22, h - 10, 100, 10, juce::Justification::bottom, true);
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

void EnergyBands::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void EnergyBands::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

double EnergyBands::getPhon()
{
  return phon.getText().getDoubleValue();
}

bool EnergyBands::getLoudnessCorrection()
{
  return corrected.getToggleState();
}

void EnergyBands::resized()
{
  int width = getWidth() - 4;
  int height = getHeight();
  blockSize.setBounds(0, 2, width / 3, 20);
  phon.setBounds(width / 3, 2, width / 3, 20);
  corrected.setBounds(2 * width / 3, 2, width / 3, 20);
  calcButton.setBounds(2, 24, width, 20);
  energyTable.setBounds(2, 46, width, height - 46);
}