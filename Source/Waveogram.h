/*
  ==============================================================================

    Waveogram.h
    Created: 24 Nov 2022 8:30:59am
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <kfr/all.hpp>
#include <chrono>
#include "Loudness.h"

class Waveogram : public juce::Component
{
public:
  //==============================================================================
  Waveogram();
  ~Waveogram() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;
  void setRawAudio(const float *audioData, size_t audioDataLength, double audioDataSampleRate);

  void setNotesPerBin(int newNotesPerBin);
  void setTimeBinSize(int newTimeBinSize);
  void setHorizontalLines(bool in);
  void setVerticalLables(bool in);
  void setHorizontalLables(bool in);
  void setDrawEllipse(bool in);
  void setNormalizeFrequencyDim(bool in);
  void setNormalizeTimeDim(bool in);
  void setScaleVertical(bool in);
  void setScaleHorizontal(bool in);
  void setThreshhold(float in);
  void setClip(float in);
  void setLevelBinNum(int in);
  void setLevelBinLogScale(bool in);
  void setLoudnessCorrection(bool in);
  void setCentered(bool in);
  void setFrequencyLabels(bool in);
  void setViewerPosition(float time);
  void resetSelection();

  void redrawImageCall();
  void recalculateImageCall();
  void calculateValueArrayCall();
  void calculateFTTCall();

  //==============================================================================
  void replaceData(const float *inData, size_t inDataLength, bool normalized, bool logScale, float *widthBorderValues = NULL, float *heightBorderValues = NULL);
  juce::Colour levelToColour(float level, bool selection = false);
  void processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop);
  void mouseUp(const juce::MouseEvent &event);
  inline bool overlap(int startA, int endA, int startB, int endB);

  void getSelection(float *selectionOut, int *selectionOutPixel);
  void setZoom(double newZoom);
  double getZoom();
  void zoomInClicked();
  void zoomOutClicked();
  void paintOverChildren(juce::Graphics &g);
  inline std::string floatToString(float a);

private:
  void recalculateImage();
  void updateImage();
  void redrawImage();
  void calculateFFT();
  void calculateFFTBlockSize();
  void calculateValueArray();
  void calculateFrequencyBorders();
  void calculateVerticalPixelMap();

  juce::Image WaveogramImage;
  juce::Viewport viewer;
  juce::Component container;
  juce::TextButton zoomIn;
  juce::TextButton zoomOut;

  double sampleRate;

  float *rawAudioData;
  size_t rawAudioDataLength;
  size_t rawAudioDataBufferSize;

  int fftBlockSize;
  int timeBinSize;
  int frequencyBinNum;
  int notesPerBin;
  kfr::complex<float> *fftOutput;
  size_t fftBlockNum;
  size_t fftBufferSize;

  double lowFreqCut = 20.0152;
  double highFreqCut = 20495.6;

  float *valueArray;
  size_t valueArrayBufferSize;

  float *frequencyBorderValues;

  double zoom;

  int widthAvailable;
  int heightAvailable;
  int heightData;
  int xAxisSize;

  double *verticalPixelMap;

  int *heightBinBorders;
  int *widthBinBorders;

  int samplesPerPixel;
  int samplesPerPixelDefault;

  int selectionCoordinates[4];

  bool horizontalLinesIn;
  bool verticalLables;
  bool horizontalLablesIn;

  juce::Atomic<bool> dataAvailable;
  juce::Atomic<bool> fftAvailable;
  juce::Atomic<bool> valueArrayAvailable;
  juce::Atomic<bool> imageCalculated;

  bool drawEllipse;

  bool normalizeFrequencyDim;
  bool normalizeTimeDim;

  bool scaleVertical;
  bool scaleHorizontal;

  float threshhold;
  float clip;

  bool selectionInfo;

  bool loudnessCorrection;

  bool centered;

  bool frequencyLabels;

  int yAxisSize;

  bool levelHistogram;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Waveogram)
};
