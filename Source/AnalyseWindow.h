/*
  ==============================================================================

    AnalyseWindow.h
    Created: 8 Nov 2022 11:51:50am
    Author:  andreas

  ==============================================================================
*/
#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Histogram.h"
#include "FileHandler.h"
#include "Loudness.h"
#include "WaveformDisplay.h"

#pragma once
class AnalyseWindow : public juce::DocumentWindow
{
public:
  //==============================================================================
  AnalyseWindow(const float *inData, size_t numSamples, double sampleRate);
  ~AnalyseWindow() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void paintOverChildren(juce::Graphics &g);
  void resized() override;
  void closeButtonPressed();

private:
  WaveformDisplay waveFrom;
  float *data;
  size_t dataLength = 0;
  double sampleRate = 1.0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyseWindow)
};
