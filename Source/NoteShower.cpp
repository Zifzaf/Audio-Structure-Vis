/*
  ==============================================================================

    NoteShower.cpp
    Created: 26 Oct 2022 9:23:35am
    Author:  andreas

  ==============================================================================
*/

#include "NoteShower.h"

NoteShower::NoteShower() : noteTable(numberOfNotes), audioData(*new juce::AudioBuffer<float>(0, 0))
{
  calcNoteFrequencies();
  std::string noteNames[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

  for (auto i = 0; i < numberOfNotes; i++)
  {
    addAndMakeVisible(&noteLables[i]);
    noteLables[i].setText(noteNames[i % 12] + std::to_string(i / 12 + 1), juce::NotificationType::dontSendNotification);

    addAndMakeVisible(&freqLables[i]);
    freqLables[i].setText(floatToString(noteFreqencies[i]), juce::NotificationType::dontSendNotification);
  }

  addAndMakeVisible(&overTonesName);
  overTonesName.setText("Number of Overtones to remove:", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&extractedFreqsName);
  extractedFreqsName.setText("Frequencies extracted form Specturm:", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&blockSizeName);
  blockSizeName.setText("Blocksize:", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&smoothFactorName);
  smoothFactorName.setText("Smoothing Factor:", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&extractedFreqs);
  extractedFreqs.setText("30");
  extractedFreqs.setInputRestrictions(3, "0123456789");
  extractedFreqs.setMultiLine(false);

  addAndMakeVisible(&overTones);
  overTones.setText("3");
  overTones.setInputRestrictions(1, "0123456789");
  overTones.setMultiLine(false);

  addAndMakeVisible(&smoothFactor);
  smoothFactor.setText("0.0");
  smoothFactor.setInputRestrictions(6, "0123456789.");
  smoothFactor.setMultiLine(false);

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

  addAndMakeVisible(&noteTable);
}

inline std::string NoteShower::floatToString(float a)
{
  std::string num_text = std::to_string(a);
  return num_text.substr(0, num_text.find(".") + 3);
}

inline int NoteShower::getExtractedFreqs()
{
  return extractedFreqs.getText().getIntValue();
}

inline int NoteShower::getOverTones()
{
  return overTones.getText().getIntValue();
}

inline float NoteShower::getSmoothFactor()
{
  return smoothFactor.getText().getFloatValue();
}

void NoteShower::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == fileInput)
  {
    audioAvailable.set(false);
    sampleRate = fileInput->getSampleRate();
    auto len = fileInput->getSegmentLength();
    if (len > audioData.getNumSamples())
    {
      delete &audioData;
      audioData = *new juce::AudioBuffer<float>(1, len);
    }
    audioAvailable.set(true);
  }
}

void NoteShower::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

inline size_t NoteShower::getBlockSize()
{
  return blockSize.getText().getIntValue();
}

void NoteShower::calcButtonClicked()
{
  if (audioAvailable.get() && calculating.compareAndSetBool(true, false))
  {
    // Start Calculation
    calcButton.setEnabled(false);

    // Get Audio Data
    fileInput->getAudioBlock(&audioData);
    const float *inData = audioData.getReadPointer(0);
    int startSample = 0;
    int endSample = fileInput->getSegmentLength();

    // Get Settings
    int extractedFreqs = getExtractedFreqs();
    int overTones = getOverTones();
    float smoothFactor = getSmoothFactor();
    const size_t blockSize = getBlockSize();

    // Calculate frequency mapping
    auto fftBins = blockSize / 2 - 2;
    auto freqList = new float[fftBins];
    auto freqStep = (sampleRate / 2.0) / (blockSize / 2);
    for (auto i = 0; i < fftBins; i++)
    {
      freqList[i] = (i + 1) * freqStep;
    }

    // Prepare FFT
    kfr::univector<kfr::complex<float>> &dftOutData = *new kfr::univector<kfr::complex<float>>(blockSize);
    kfr::dft_plan_real<float> dft(blockSize);
    kfr::univector<kfr::u8> &temp = *new kfr::univector<kfr::u8>(dft.temp_size);
    juce::dsp::WindowingFunction<float> window(blockSize, juce::dsp::WindowingFunction<float>::hamming);

    // Prepare Calculation arrays
    auto ampSpectrum = new float[fftBins];
    auto ampSpectrumTwo = new float[fftBins];
    float *inDataBlock = new float[blockSize];
    float *noteLevel = new float[numberOfNotes];
    float *prevNoteLevel = new float[numberOfNotes];
    juce::FloatVectorOperations::fill(prevNoteLevel, 0, numberOfNotes);

    // Calculate number of blocks
    int numBlocks = std::ceil((double)endSample / (double)(blockSize / 2)) + 1;

    // Prepare time borders array
    float *timeBorders = new float[numBlocks + 1];

    // Prepare Output Array
    auto outputData = new float[numBlocks * numberOfNotes];

    for (int i = startSample - blockSize / 2; i < endSample; i = i + blockSize / 2)
    {
      if (i < 0)
      {
        juce::FloatVectorOperations::fill(inDataBlock, 0.0, blockSize);
        juce::FloatVectorOperations::copy(&inDataBlock[-i], &inData[0], blockSize + i);
      }
      else if (i + blockSize > endSample)
      {
        juce::FloatVectorOperations::fill(inDataBlock, 0.0, blockSize);
        juce::FloatVectorOperations::copy(inDataBlock, &inData[i], (size_t)endSample - i);
      }
      else
      {
        juce::FloatVectorOperations::copy(inDataBlock, &inData[i], blockSize);
      }
      window.multiplyWithWindowingTable(inDataBlock, blockSize);
      dft.execute(dftOutData, kfr::make_univector(inDataBlock, blockSize), temp);
      for (auto j = 0; j < fftBins; j++)
      {
        ampSpectrum[j] = kfr::cabs(dftOutData[j + 1]);
        ampSpectrumTwo[j] = ampSpectrum[j];
      }
      // kfr::plot_show("Spec", kfr::make_univector(ampSpectrum,fftBins), "title='Spec', div_by_N=True");
      std::sort(ampSpectrumTwo, &ampSpectrumTwo[fftBins]);
      float threshhold = ampSpectrumTwo[fftBins - extractedFreqs];
      for (auto j = 0; j < fftBins; j++)
      {
        if (ampSpectrum[j] < threshhold)
        {
          ampSpectrum[j] = 0.0;
        }
      }
      for (auto j = 0; j < fftBins; j++)
      {
        float max = ampSpectrum[j];
        for (auto k = 0; k < overTones; k++)
        {
          int index = j * (k + 2);
          if (index < fftBins && ampSpectrum[index] > max)
          {
            max = ampSpectrum[index];
            break;
          }
        }
        if (ampSpectrum[j] >= max)
        {

          for (auto k = 0; k < overTones; k++)
          {
            int index = j * (k + 2);
            if (index < fftBins)
            {
              ampSpectrum[j] += ampSpectrum[index];
              ampSpectrum[index] = 0.0;
            }
          }
        }
      }
      int index = 0;
      float diff = 0.5 * (noteFreqencies[1] - noteFreqencies[0]);
      while (abs(freqList[index] - noteFreqencies[0]) > diff)
      {
        index++;
      }
      for (auto j = 0; j < numberOfNotes - 1; j++)
      {
        noteLevel[j] = 0.0;
        float currFreq = noteFreqencies[j];
        float nextFreq = noteFreqencies[j + 1];
        while (abs(freqList[index] - currFreq) < abs(freqList[index] - nextFreq))
        {
          noteLevel[j] += ampSpectrum[index];
          index++;
        }
      }
      noteLevel[numberOfNotes - 1] = 0.0;
      diff = abs(freqList[index] - noteFreqencies[numberOfNotes - 1]);
      while (abs(freqList[index] - noteFreqencies[numberOfNotes - 1]) < diff)
      {
        noteLevel[numberOfNotes - 1] += ampSpectrum[index];
        index++;
      }
      juce::FloatVectorOperations::addWithMultiply(noteLevel, prevNoteLevel, smoothFactor, numberOfNotes);
      int blockIndex = (i + (blockSize / 2) - startSample) / (blockSize / 2);
      juce::FloatVectorOperations::copy(&outputData[blockIndex * numberOfNotes], noteLevel, numberOfNotes);
      timeBorders[blockIndex] = (double)i / fileInput->getSampleRate();
      float *temp = prevNoteLevel;
      prevNoteLevel = noteLevel;
      noteLevel = temp;
    }
    timeBorders[numBlocks] = (double)(numBlocks * blockSize / 2) / fileInput->getSampleRate();
    noteTable.replaceData(outputData, numBlocks, false, false, timeBorders, NULL);
    delete outputData;
    delete noteLevel;
    delete inDataBlock;
    delete freqList;
    delete ampSpectrum;
    delete ampSpectrumTwo;
    delete &dftOutData;
    delete &temp;
    calcButton.setEnabled(true);
    calculating.set(false);
  }
}

void NoteShower::setAudio(const juce::AudioBuffer<float> in, float sampleRate, int channelNum)
{
  audioAvailable.set(false);
  this->sampleRate = sampleRate;
  size_t len = in.getNumSamples();
  delete &audioData;
  audioData = *new juce::AudioBuffer<float>(1, len);
  audioData.copyFrom(0, 0, in.getReadPointer(channelNum), len);
  audioAvailable.set(true);
}

void NoteShower::calcNoteFrequencies()
{
  float factor = std::pow(2.0, 1.0 / 12.0);
  noteFreqencies[0] = 0.25 * baseTuneing;
  noteFreqencies[12] = 0.5 * baseTuneing;
  noteFreqencies[24] = baseTuneing;
  noteFreqencies[36] = 2 * baseTuneing;
  noteFreqencies[48] = 4 * baseTuneing;
  for (auto i = 0; i < numberOfNotes; i = i + 12)
  {
    float baseValue = noteFreqencies[i];
    for (auto j = 1; j < 12; j++)
    {
      baseValue *= factor;
      noteFreqencies[i + j] = baseValue;
    }
  }
}

NoteShower::~NoteShower()
{
  delete &audioData;
}

void NoteShower::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void NoteShower::resized()
{
  int width = getWidth();
  int height = getHeight();
  int size = width / 8;
  blockSizeName.setBounds(2, 2, size, 20);
  blockSize.setBounds(2 + 1 * size, 2, size, 20);
  extractedFreqsName.setBounds(2 + 2 * size, 2, size, 20);
  extractedFreqs.setBounds(2 + 3 * size, 2, size, 20);
  overTonesName.setBounds(2 + 4 * size, 2, size, 20);
  overTones.setBounds(2 + 5 * size, 2, size, 20);
  smoothFactorName.setBounds(2 + 6 * size, 2, size, 20);
  smoothFactor.setBounds(2 + 7 * size, 2, size, 20);
  calcButton.setBounds(0, 24, width, 20);
  noteTable.setBounds(40, 46, width - 120, height - 46);
  int noteTableHeight = height - 46 - noteTable.xAxisSize;
  int noteLabelHeight = noteTableHeight / numberOfNotes;
  int rest = noteTableHeight % numberOfNotes;
  auto h = height - noteLabelHeight - noteTable.xAxisSize;
  if (rest > 0)
  {
    rest--;
    h--;
  }
  for (auto i = 0; i < numberOfNotes; ++i)
  {
    noteLables[i].setBounds(0, h, 40, noteLabelHeight);
    freqLables[i].setBounds(width - 80, h, 80, noteLabelHeight);
    h = h - noteLabelHeight;
    if (rest > 0)
    {
      rest--;
      h--;
    }
  }
}
