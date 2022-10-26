/*
  ==============================================================================

    FileHandler.h
    Created: 21 Oct 2022 10:04:54am
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FileHandler : public juce::PositionableAudioSource, public juce::Component, public juce::ChangeBroadcaster
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
  void getAudioBlock(juce::AudioBuffer<float> *bufferToFill, int startSample, int numSamples, int channelNum = 0);
  double getSampleRate();
  juce::AudioBuffer<float> *getAudio(double from, double to);
  bool isAudioPlaying();

private:
  juce::AudioFormatManager formatManager;
  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transportSource;
  std::unique_ptr<juce::FileChooser> chooser;
  juce::TextButton openButton;
  juce::TextButton playButton;
  juce::TextButton stopButton;
  juce::Label fileName;
  juce::Atomic<bool> fileLoaded = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileHandler)
};
