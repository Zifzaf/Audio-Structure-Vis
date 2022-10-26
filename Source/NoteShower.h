/*
  ==============================================================================

    NoteShower.h
    Created: 26 Oct 2022 9:23:35am
    Author:  andreas

  ==============================================================================
*/

#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Histogram.h"
#include "FileHandler.h"

#pragma once
class NoteShower  : public juce::Component, public juce::ChangeListener
{
public:
    //==============================================================================
    NoteShower();
    ~NoteShower() override;
    

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void calcNoteFrequencies();
    void calcButtonClicked();
    void setAudio(const juce::AudioBuffer<float> in, float sampleRate, int channelNum = 0);
    void changeListenerCallback (juce::ChangeBroadcaster *source);
    inline float getStartTime();
    inline float getEndTime();
    void addFileHandler(FileHandler* in);
    inline size_t getBlockSize();


    static const size_t numberOfNotes = 72;
    std::array<float, numberOfNotes> noteFreqencies;
    float baseTuneing = 440.0;
    juce::TextButton calcButton;
    float sampleRate = 0.0;
    


private:
    Histogram noteTable;
    FileHandler *fileInput = nullptr;
    juce::TextEditor startTime;
    juce::TextEditor endTime;
    juce::ComboBox blockSize;
    juce::AudioBuffer<float> &audioData;
    juce::Atomic<bool> audioAvailable = false;


    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteShower)
};