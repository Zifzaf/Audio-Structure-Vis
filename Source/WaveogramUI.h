/*
  ==============================================================================

    WaveogramUI.h
    Created: 26 Nov 2022 7:03:58am
    Author:  andreas

  ==============================================================================
*/

#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Waveogram.h"
#include "FileHandler.h"
#include "Loudness.h"

#pragma once
class WaveogramUI : public juce::Component, public juce::ChangeListener
{
public:
  //==============================================================================
  WaveogramUI(FileHandler *in);
  ~WaveogramUI() override;
  void changeListenerCallback(juce::ChangeBroadcaster *source);
  void calcButtonClicked();
  void addFileHandler(FileHandler *in);
  int getFrequencyBins();
  int getTimeBinSize();
  void threshholdClicked();
  void clipClicked();
  void levelBinNumClicked();
  void horizontalLinesClicked();
  void verticalLablesClicked();
  void horizontalLablesClicked();
  void drawEllipseClicked();
  void normalizeFrequencyDimClicked();
  void normalizeTimeDimClicked();
  void scaleVerticalClicked();
  void scaleHorizontalClicked();
  void levelBinLogScaleClicked();
  void loudnessCorrectionClicked();
  void centeredClicked();

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  Waveogram waveData;
  FileHandler *fileInput = nullptr;
  juce::TextButton calcButton;
  juce::TextEditor frequencyBinInput;
  juce::TextEditor timeBinInput;
  juce::TextEditor threshhold;
  juce::TextEditor clip;
  juce::TextEditor levelBinNum;
  juce::ToggleButton horizontalLines;
  juce::ToggleButton verticalLables;
  juce::ToggleButton horizontalLables;
  juce::ToggleButton drawEllipse;
  juce::ToggleButton normalizeFrequencyDim;
  juce::ToggleButton normalizeTimeDim;
  juce::ToggleButton scaleVertical;
  juce::ToggleButton scaleHorizontal;
  juce::ToggleButton levelBinLogScale;
  juce::ToggleButton loudnessCorrection;
  juce::ToggleButton centered;

  juce::AudioBuffer<float> &audioData;
  juce::Atomic<bool> audioAvailable = false;
  juce::Atomic<bool> calculating = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveogramUI)
};
