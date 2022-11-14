/*
  ==============================================================================

    NoteShower.h
    Created: 26 Oct 2022 9:23:35am
    Author:  andreas

  ==============================================================================
*/

#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Histogram.h"
#include "FileHandler.h"

#pragma once
class NoteShower : public juce::Component, public juce::ChangeListener
{
public:
  //==============================================================================
  NoteShower();
  ~NoteShower() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;
  void calcNoteFrequencies();
  void calcButtonClicked();
  void setAudio(const juce::AudioBuffer<float> in, float sampleRate, int channelNum = 0);
  void changeListenerCallback(juce::ChangeBroadcaster *source);
  void addFileHandler(FileHandler *in);
  inline size_t getBlockSize();
  int getExtractedFreqs();
  int getOverTones();
  float getSmoothFactor();
  inline std::string floatToString(float a);

  static const size_t numberOfNotes = 60;
  std::array<float, numberOfNotes> noteFreqencies;
  float baseTuneing = 440.0;
  juce::TextButton calcButton;
  float sampleRate = 0.0;

private:
  Histogram noteTable;
  FileHandler *fileInput = nullptr;
  juce::Label overTonesName;
  juce::Label extractedFreqsName;
  juce::Label smoothFactorName;
  juce::Label blockSizeName;
  juce::TextEditor extractedFreqs;
  juce::TextEditor overTones;
  juce::TextEditor smoothFactor;
  juce::ComboBox blockSize;
  juce::AudioBuffer<float> &audioData;
  juce::Atomic<bool> audioAvailable = false;
  juce::Atomic<bool> calculating = false;
  std::array<juce::Label, numberOfNotes> noteLables;
  std::array<juce::Label, numberOfNotes> freqLables;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteShower)
};