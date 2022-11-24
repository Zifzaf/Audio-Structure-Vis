/*
  ==============================================================================

    EventSelector.h
    Created: 7 Nov 2022 8:17:50am
    Author:  andreas

  ==============================================================================
*/
#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Histogram.h"
#include "FileHandler.h"
#include "Loudness.h"
#include "AnalyseWindow.h"

#pragma once
class EventSelector : public juce::Component, public juce::ChangeListener
{
public:
  //==============================================================================
  EventSelector();
  EventSelector(FileHandler *in);
  ~EventSelector() override;
  void changeListenerCallback(juce::ChangeBroadcaster *source);
  void calcButtonClicked();
  void analyseButtonClicked();
  void addFileHandler(FileHandler *in);
  inline size_t getBlockSize();
  double getPhon();
  bool getLoudnessCorrection();
  void zoomInClicked();
  void zoomOutClicked();

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void paintOverChildren(juce::Graphics &g);
  void resized() override;
  static const size_t numberOfBands = 32;
  std::array<float, numberOfBands + 1> bandCuts;
  float *timeBorders = NULL;

private:
  Histogram thirdOctaveSpectrogarm;
  FileHandler *fileInput = nullptr;
  juce::TextButton calcButton;
  juce::TextButton analyseButton;
  juce::ComboBox blockSize;
  juce::TextEditor phon;
  juce::ToggleButton corrected;
  juce::AudioBuffer<float> &audioData;
  juce::Atomic<bool> audioAvailable = false;
  juce::Atomic<bool> calculating = false;
  AnalyseWindow *popUp;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventSelector)
};
