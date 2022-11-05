/*
  ==============================================================================

    FileHandler.h
    Created: 21 Oct 2022 10:04:54am
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FileHandler : public juce::PositionableAudioSource, public juce::Component, public juce::ChangeBroadcaster, public juce::TextEditor::Listener
{
public:
  FileHandler();
  ~FileHandler();
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
  void releaseResources();
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
  void setNextReadPosition(juce::int64 newPosition);
  juce::int64 getNextReadPosition() const;
  juce::int64 getTotalLength() const;
  bool isLooping() const;
  void setLooping(bool shouldLoop);
  void paint(juce::Graphics &g) override;
  void resized() override;
  void openButtonClicked();
  void stopButtonClicked();
  void playButtonClicked();
  void getAudioBlock(juce::AudioBuffer<float> *bufferToFill);
  double getSampleRate();
  juce::AudioBuffer<float> *getAudio(double from, double to);
  bool isAudioPlaying();
  inline float getStartTime();
  inline float getEndTime();
  void textEditorTextChanged(juce::TextEditor &source);
  juce::int64 getSegmentLength();
  double getCurrentTime();

private:
  juce::Label time;
  juce::Label seconds1;
  juce::Label seconds2;
  juce::Label to;
  juce::TextEditor startTime;
  juce::TextEditor endTime;
  juce::AudioFormatManager formatManager;
  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transportSource;
  std::unique_ptr<juce::FileChooser> chooser;
  juce::TextButton openButton;
  juce::TextButton playButton;
  juce::TextButton stopButton;
  juce::Label fileName;
  juce::Atomic<bool> fileLoaded = false;
  float startTimeVal = 0.0;
  float endTimeVal = 0.0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileHandler)
};
