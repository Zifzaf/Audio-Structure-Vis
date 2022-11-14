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
      if (k < numberOfBands - 1 && (i + 1) * freqStep > criticalBandCuts[k + 1])
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
    juce::dsp::WindowingFunction<float> window(blockSize, juce::dsp::WindowingFunction<float>::blackmanHarris);

    // Calculate blocking for input segment
    auto segmentLength = fileInput->getSegmentLength();
    int numBlocks = std::ceil((double)segmentLength / (double)(blockSize / 2)) + 1;

    // Get Pointer to data
    const float *inData = audioData.getReadPointer(0);

    // Prepare calculation arrays
    float *inDataBlock = new float[blockSize];
    auto complexBins = new kfr::complex<double>[numberOfBands];

    // Prepare time borders array
    float* timeBorders = new float[numBlocks + 1];

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
    energyTable.replaceData(outputData, numBlocks, false, false, timeBorders, criticalBandCuts.data());
    delete bandIndex;
    delete &dftOutData;
    delete complexBins;
    delete &temp;
    delete inDataBlock;
    delete outputData;
    delete timeBorders;
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