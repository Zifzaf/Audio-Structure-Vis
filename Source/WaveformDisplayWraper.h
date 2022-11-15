/*
  ==============================================================================

    WaveformDisplayWraper.h
    Created: 15 Nov 2022 8:35:39am
    Author:  andreas

  ==============================================================================
*/

#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "FileHandler.h"
#include "WaveformDisplay.h"

#pragma once
class WaveformDisplayWraper : public juce::Component
{
public:
  //==============================================================================
  WaveformDisplayWraper();
  ~WaveformDisplayWraper() override;
  void changeListenerCallback(juce::ChangeBroadcaster *source);
  void calcButtonClicked();
  void addFileHandler(FileHandler *in);

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  WaveformDisplay waveForm;
  FileHandler *fileInput = nullptr;
  juce::TextButton calcButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplayWraper)
};
