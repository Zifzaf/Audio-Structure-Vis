#pragma once

#include <JuceHeader.h>
#include "WaveogramUI.h"
#include <kfr/all.hpp>
#include <chrono>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    void visualize(const float *buffer, int startSample, int numSamples);
   
    WaveogramUI waveData;

    //==============================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    FileHandler openFile;
    juce::TooltipWindow toolTips;


    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
