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

  addAndMakeVisible(&startTime);
  startTime.setText("0.0");
  startTime.setInputRestrictions(6, "0123456789.");
  startTime.setMultiLine(false);

  addAndMakeVisible(&endTime);
  endTime.setText("0.0");
  endTime.setInputRestrictions(6, "0123456789.");
  endTime.setMultiLine(false);

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
  noteTable.setFramesPerSecond(30);
}

inline float NoteShower::getStartTime()
{
  return startTime.getText().getFloatValue();
}

inline float NoteShower::getEndTime()
{
  return endTime.getText().getFloatValue();
}

void NoteShower::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == fileInput)
  {
    audioAvailable.set(false);
    auto len = fileInput->getTotalLength();
    delete &audioData;
    audioData = *new juce::AudioBuffer<float>(1, len);
    fileInput->getAudioBlock(&audioData, 0, len);
    sampleRate = fileInput->getSampleRate();
    audioAvailable.set(true);
    endTime.setText(juce::String(((float)len) / sampleRate));
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
  calcButton.setEnabled(false);
  if (audioAvailable.get())
  {
    int extractedFreqs = 30;
    int overTones = 3;
    const size_t blockSize = getBlockSize();
    int startSample = getStartTime() * sampleRate;
    int endSample = getEndTime() * sampleRate;
    auto fftBins = blockSize / 2 - 2;
    auto freqList = new float[fftBins];
    auto ampSpectrum = new float[fftBins];
    auto ampSpectrumTwo = new float[fftBins];
    auto freqStep = (sampleRate / 2.0) / (blockSize / 2);
    for (auto i = 0; i < fftBins; i++)
    {
      freqList[i] = (i + 1) * freqStep;
    }
    kfr::univector<kfr::complex<float>> &dftOutData = *new kfr::univector<kfr::complex<float>>(blockSize);
    kfr::dft_plan_real<float> dft(blockSize);
    kfr::univector<kfr::u8> &temp = *new kfr::univector<kfr::u8>(dft.temp_size);
    const float *inData = audioData.getReadPointer(0);
    float *inDataChunck = new float[blockSize];
    float *noteLevel = new float[numberOfNotes];
    float *prevNoteLevel = new float[numberOfNotes];
    juce::FloatVectorOperations::fill(prevNoteLevel, 0, numberOfNotes);
    for (auto i = startSample; i < endSample - blockSize; i = i + blockSize / 2)
    {
      juce::FloatVectorOperations::copy(inDataChunck, &inData[i], blockSize);
      dft.execute(dftOutData, kfr::make_univector(inDataChunck, blockSize), temp);
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
        for (auto k = 0; k < overTones; k++)
        {
          int index = j * (k + 2);
          if (index < fftBins)
          {
            ampSpectrum[j] += ampSpectrum[index];
          }
        }
      }
      int index = 0;
      float diff = 0.5*(noteFreqencies[1] - noteFreqencies[0]);
      while(abs(freqList[index] - noteFreqencies[0]) > diff){
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
      diff = abs(freqList[index] - noteFreqencies[numberOfNotes - 1]);
      while (abs(freqList[index] - noteFreqencies[numberOfNotes - 1]) < diff)
      {
        noteLevel[numberOfNotes - 1] += ampSpectrum[index];
        index++;
      }
      noteTable.addDataLine(noteLevel, false);
    }
    delete noteLevel;
    delete inDataChunck;
    delete freqList;
    delete ampSpectrum;
    delete ampSpectrumTwo;
    delete &dftOutData;
    delete &temp;
  }
  calcButton.setEnabled(true);
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
  endTime.setText(juce::String(((float)len) / sampleRate));
}

void NoteShower::calcNoteFrequencies()
{
  float factor = std::pow(2.0, 1.0 / 12.0);
  noteFreqencies[0] = 0.125 * baseTuneing;
  noteFreqencies[12] = 0.25 * baseTuneing;
  noteFreqencies[24] = 0.5 * baseTuneing;
  noteFreqencies[36] = baseTuneing;
  noteFreqencies[48] = 2 * baseTuneing;
  noteFreqencies[60] = 4 * baseTuneing;
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
  blockSize.setBounds(0, 0, width, 20);
  startTime.setBounds(0, 20, width, 20);
  endTime.setBounds(0, 40, width, 20);
  calcButton.setBounds(0, 60, width, 20);
  noteTable.setBounds(0, 80, width, height - 80);
}
