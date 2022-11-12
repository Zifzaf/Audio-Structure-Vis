/*
  ==============================================================================

    Histogram.h
    Created: 20 Oct 2022 9:08:59am
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Histogram : public juce::Component
{
public:
  //==============================================================================
  Histogram(int dataLevels);
  Histogram(float *data, int dataLevels, int dataWidth);
  ~Histogram() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;
  void addDataLine(const float dataLine[], bool normalized);
  void replaceData(const float *inData, size_t inDataLength, bool normalized, bool logScale, float *widthBorderValues = NULL, float *heightBorderValues = NULL);
  juce::Colour levelToColour(float level, bool selection = false);
  void processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop);
  void mouseUp(const juce::MouseEvent &event);
  inline bool overlap(int startA, int endA, int startB, int endB);
  void recalculateImage();
  void updateImage();
  void redrawImage();
  void getSelection(float *selectionOut, bool borderValuesSet);

private:
  juce::Image histogramImage;
  juce::Viewport viewer;
  juce::Component container;
  int dataLevels = 1;
  int *heightDataMap;
  int widthAvailable = 0;
  int heightAvailable = 0;
  int levelWidth = 50;
  int dataLength = 1;
  float *data;
  int *heightBinBorders = NULL;
  int *widthBinBorders = NULL;
  float *widthBorderValues = NULL;
  float *heightBorderValues = NULL;
  int widthBins = 1;
  juce::Atomic<bool> dataReady = false;
  std::array<int, 4> selction = {0, 0, 0, 0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Histogram)
};
