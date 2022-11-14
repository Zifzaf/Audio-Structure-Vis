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
  void setZoom(double newZoom);
  double getZoom();
  void zoomInClicked();
  void zoomOutClicked();
  void setVerticalLines(bool in);
  void setHorizontalLines(bool in);
  void setVerticalLables(bool in);
  void setHorizontalLables(bool in);
  void paintOverChildren(juce::Graphics &g);

private:
  juce::Image histogramImage;
  juce::Viewport viewer;
  juce::Component container;
  juce::TextButton zoomIn;
  juce::TextButton zoomOut;
  int dataLevels = 1;
  int *heightDataMap;
  int widthAvailable = 0;
  int heightAvailable = 0;
  int levelWidth = 50;
  int dataLength = 0;
  float *data;
  int *heightBinBorders = NULL;
  int *widthBinBorders = NULL;
  float *widthBorderValues = NULL;
  bool widthBorderValuesSet = false;
  float *heightBorderValues = NULL;
  bool heightBorderValuesSet = false;
  int widthBins = 1;
  juce::Atomic<bool> dataReady = false;
  std::array<int, 4> selction = {0, 0, 0, 0};
  double zoom = 1.0;
  bool verticalLines = true;
  bool horizontalLines = true;
  bool verticalLables = true;
  bool horizontalLables = true;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Histogram)
};
