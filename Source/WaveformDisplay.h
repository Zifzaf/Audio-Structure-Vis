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
  void replaceData(const float *inData, size_t inDataLength, bool normalized,  double newSampleRate);
  void processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop);
  void showRMS(bool val);
  void recalculateImage();
  void updateImage();
  void redrawImage();
  void setZoom(double newZoom);
  double getZoom();
  void zoomInClicked();
  void zoomOutClicked();
  int xAxisSize = 28;

private:
  juce::Viewport viewer;
  juce::Component container;
  juce::TextButton zoomIn;
  juce::TextButton zoomOut;
  juce::Image WaveformDisplayImage;
  float *data;
  size_t dataLength = 0;
  bool showRMSVal = false;
  juce::Atomic<bool> dataReady = false;
  int widthAvailable = 0;
  int heightAvailable = 0;
  int widthImage = 0;
  double zoom = 1.0;
  int numBins = 0;
  double sampleRate = 0.0;
  float *meanArray = NULL;
  float *rmsArray = NULL;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
