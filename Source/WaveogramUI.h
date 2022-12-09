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
  float getThreshhold();
  float getClip();
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
  void frequencyLabelsClicked();

  void setSpectrogramCall();
  void setWavegramCall();
  void setFrequencygramCall();
  void setHistgramCall();
  void setWaveformCall();

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  Waveogram waveData;
  FileHandler *fileInput = nullptr;
  juce::TextButton calcButton;
  juce::TextEditor frequencyBinInput;
  juce::ComboBox timeBinInput;
  juce::TextEditor threshhold;
  juce::TextEditor clip;
  juce::ToggleButton horizontalLines;
  juce::ToggleButton verticalLables;
  juce::ToggleButton horizontalLables;
  juce::ToggleButton drawEllipse;
  juce::ToggleButton normalizeFrequencyDim;
  juce::ToggleButton normalizeTimeDim;
  juce::ToggleButton scaleVertical;
  juce::ToggleButton scaleHorizontal;
  juce::ToggleButton loudnessCorrection;
  juce::ToggleButton centered;
  juce::ToggleButton frequencyLabels;
  juce::TextButton setSpectrogram;
  juce::TextButton setWavegram;
  juce::TextButton setFrequencygram;
  juce::TextButton setHistgram;
  juce::TextButton setWaveform;

  juce::AudioBuffer<float> &audioData;
  juce::Atomic<bool> audioAvailable = false;
  juce::Atomic<bool> calculating = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveogramUI)
};
