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
  bandCuts[0] = 0;
  for (auto i = 16; i >= 0; i--)
  {
    bandCuts[i + 1] = std::pow(factorInv, 17 - i) * 1000.0;
  }
  bandCuts[18] = 1000.0;
  for (auto i = 18; i < numberOfBands - 1; i++)
  {
    bandCuts[i + 1] = std::pow(factor, i - 17) * 1000.0;
  }
  bandCuts[numberOfBands] = 24000.0;

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

  addAndMakeVisible(&analyseButton);
  analyseButton.setButtonText("Analyse");
  analyseButton.onClick = [this]
  { analyseButtonClicked(); };
  analyseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);

  addAndMakeVisible(&thirdOctaveSpectrogarm);

  addAndMakeVisible(&phon);
  phon.setText("80");
  phon.setInputRestrictions(2, "0123456789");
  phon.setMultiLine(false);

  addAndMakeVisible(&corrected);
  corrected.setState(juce::Button::buttonNormal);
  corrected.setButtonText("Loudness Correction");
  corrected.triggerClick();
}

EventSelector::EventSelector(FileHandler *in) : thirdOctaveSpectrogarm(numberOfBands), audioData(*new juce::AudioBuffer<float>(0, 0)), fileInput(in)
{
  double factor = std::pow(2.0, 1.0 / 3.0);
  double factorInv = 1.0 / factor;
  bandCuts[0] = 0;
  for (auto i = 16; i >= 0; i--)
  {
    bandCuts[i + 1] = std::pow(factorInv, 17 - i) * 1000.0;
  }
  bandCuts[18] = 1000.0;
  for (auto i = 18; i < numberOfBands - 1; i++)
  {
    bandCuts[i + 1] = std::pow(factor, i - 17) * 1000.0;
  }
  bandCuts[numberOfBands] = fileInput->getSampleRate() / 2.0;

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

  addAndMakeVisible(&analyseButton);
  analyseButton.setButtonText("Analyse");
  analyseButton.onClick = [this]
  { analyseButtonClicked(); };
  analyseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);

  addAndMakeVisible(&thirdOctaveSpectrogarm);

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

void EventSelector::zoomInClicked()
{
  double z = thirdOctaveSpectrogarm.getZoom();
  thirdOctaveSpectrogarm.setZoom(z + 0.2);
}

void EventSelector::zoomOutClicked()
{
  double z = thirdOctaveSpectrogarm.getZoom();
  thirdOctaveSpectrogarm.setZoom(std::max(z - 0.2, 0.2));
}

inline size_t EventSelector::getBlockSize()
{
  return blockSize.getText().getIntValue();
}

void EventSelector::analyseButtonClicked()
{
  const float *inData = audioData.getReadPointer(0);
  float sampleRate = fileInput->getSampleRate();
  float selection[4];
  thirdOctaveSpectrogarm.getSelection(&selection[0], true);
  int segmentLength = (selection[1] - selection[0]) * sampleRate;
  int startIndex = selection[0] * sampleRate;
  float *analysisSection = new float[segmentLength];
  juce::FloatVectorOperations::copy(analysisSection, &inData[startIndex], segmentLength);
  auto highPassFilter = kfr::to_filter(kfr::biquad<kfr::f32>(kfr::biquad_highpass<kfr::f32>(selection[3] / sampleRate, 1.0), kfr::placeholder<kfr::f32>()));
  auto lowPassFilter = kfr::to_filter(kfr::biquad<kfr::f32>(kfr::biquad_lowpass<kfr::f32>(selection[2] / sampleRate, 1.0), kfr::placeholder<kfr::f32>()));
  highPassFilter.apply(analysisSection, segmentLength);
  lowPassFilter.apply(analysisSection, segmentLength);
  popUp = new AnalyseWindow(analysisSection, segmentLength, sampleRate);
  popUp->setSize(1200, 800);
  popUp->setUsingNativeTitleBar(true);
  popUp->addToDesktop();
  popUp->setVisible(true);
  delete analysisSection;
}

void EventSelector::calcButtonClicked()
{
  if (audioAvailable.get() && calculating.compareAndSetBool(true, false))
  {
    // Start Calcualtion
    calcButton.setEnabled(false);

    // Get audio data to process
    fileInput->getAudioBlock(&audioData);

    // Calculate FFT frequencies
    size_t blockSize = getBlockSize();
    auto fftBins = blockSize / 2 - 2;

    // Calcuate Binning Index for Frequencies
    auto bandIndex = new int[fftBins];

    // Calculate loudness correction for frequencies
    bool loudnessCorrection = getLoudnessCorrection();
    double phon = getPhon();
    auto freqScale = new double[fftBins];
    auto freqStep = (fileInput->getSampleRate() / 2.0) / (blockSize / 2);

    // Calculation Loop for loudness and binning
    int k = 0;
    for (auto i = 0; i < fftBins; i++)
    {
      if (k < numberOfBands - 1 && (i + 1) * freqStep > bandCuts[k + 1])
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
    // Prepare FFT
    kfr::univector<kfr::complex<float>> &dftOutData = *new kfr::univector<kfr::complex<float>>(blockSize);
    kfr::dft_plan_real<float> dft(blockSize);
    kfr::univector<kfr::u8> &temp = *new kfr::univector<kfr::u8>(dft.temp_size);
    juce::dsp::WindowingFunction<float> window(blockSize, juce::dsp::WindowingFunction<float>::kaiser, true, 3.0);

    // Calculate blocking for input segment
    auto segmentLength = fileInput->getSegmentLength();
    int numBlocks = std::ceil((double)segmentLength / (double)(blockSize / 2)) + 1;

    // Get Pointer to data
    const float *inData = audioData.getReadPointer(0);

    // Prepare calculation arrays
    float *inDataBlock = new float[blockSize];
    auto complexBins = new kfr::complex<double>[numberOfBands];

    // Prepare time borders array
    delete timeBorders;
    timeBorders = new float[numBlocks + 1];

    // Prepare output array
    auto outputData = new float[numBlocks * numberOfBands];
    juce::FloatVectorOperations::fill(outputData, 0.0f, (size_t)numBlocks * numberOfBands);

    for (int i = -blockSize / 2; i < segmentLength; i = i + blockSize / 2)
    {
      if (i < 0)
      {
        juce::FloatVectorOperations::fill(inDataBlock, 0.0, blockSize);
        juce::FloatVectorOperations::copy(&inDataBlock[-i], &inData[0], blockSize + i);
      }
      else if (i + blockSize > segmentLength)
      {
        juce::FloatVectorOperations::fill(inDataBlock, 0.0, blockSize);
        juce::FloatVectorOperations::copy(inDataBlock, &inData[i], (size_t)segmentLength - i);
      }
      else
      {
        juce::FloatVectorOperations::copy(inDataBlock, &inData[i], blockSize);
      }
      window.multiplyWithWindowingTable(inDataBlock, blockSize);
      for (auto j = 0; j < numberOfBands; j++)
      {
        complexBins[j] = 0.0;
      }
      dft.execute(dftOutData, kfr::make_univector(inDataBlock, blockSize), temp);
      int blockIndex = (i + (blockSize / 2)) / (blockSize / 2);
      for (auto j = 0; j < fftBins; j++)
      {
        complexBins[bandIndex[j]] = complexBins[bandIndex[j]] + freqScale[j] * dftOutData[j + 1] / blockSize;
      }
      for (auto j = 0; j < numberOfBands; j++)
      {
        outputData[blockIndex * numberOfBands + j] = kfr::cabs(complexBins[j]);
      }
      timeBorders[blockIndex] = (double)i / fileInput->getSampleRate();
    }
    timeBorders[numBlocks] = (double)(numBlocks * blockSize / 2) / fileInput->getSampleRate();
    thirdOctaveSpectrogarm.replaceData(outputData, numBlocks, false, false, timeBorders, bandCuts.data());
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
  /*
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
    g.drawText(std::to_string((int)bandCuts[i + 1]) + " Hz", 22, h - 10, 100, 10, juce::Justification::bottom, true);
    g.fillRect(2, h, width, 1);
    h = h - bandLabelHeight;
    if (rest > 0)
    {
      rest--;
      h--;
    }
  }
  float fraction = (fileInput->getCurrentTime() - fileInput->getStartTime()) / (fileInput->getEndTime() - fileInput->getStartTime());
  fraction = std::max(0.0f, fraction);
  g.setColour(juce::Colours::white);
  int pos = fraction * (width);
  g.fillRect(pos - 1, 46, 2, height - 46);*/
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
  blockSize.setBounds(0, 2, width / 4, 20);
  phon.setBounds(width / 4, 2, width / 4, 20);
  corrected.setBounds(2 * width / 4, 2, width / 4, 20);
  analyseButton.setBounds(3 * width / 4, 2, width / 4, 20);
  calcButton.setBounds(2, 24, width, 20);
  thirdOctaveSpectrogarm.setBounds(2, 46, width, height - 46);
}