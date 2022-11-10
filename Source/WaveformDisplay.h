/*
  ==============================================================================

    WaveformDisplay.h
    Created: 8 Nov 2022 2:50:20pm
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::AnimatedAppComponent
{
public:
  //==============================================================================
  WaveformDisplay();
  ~WaveformDisplay() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;
  void update() override;
  void replaceData(const float *inData, size_t inDataLength, bool normalized);
  void processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop);
  void showRMS(bool val);

private:
  juce::Image WaveformDisplayImage;
  float *data;
  size_t dataLength = 0;
  bool showRMSVal = false;
  juce::Atomic<bool> dataReady = false;
  int widthAvailable = 0;
  int heightAvailable = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
