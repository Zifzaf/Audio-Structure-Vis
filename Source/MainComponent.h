#pragma once

#include <JuceHeader.h>
#include "Histogram.h"
#include "FileHandler.h"
#include "NoteShower.h"
#include "EnergyBands.h"
#include <kfr/all.hpp>
#include <chrono>

enum VisType
{
    None,
    VisNoteShower,
    VisEnergyBand,
};

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
    NoteShower notes;
    EnergyBands energy;
    VisType activeVis = None;
    static const size_t numVis = 2;
    std::array<juce::TextButton, numVis> visSelect;
    std::array<std::string, numVis> visNames = {"Note Shower", "Energy Bands"};
    void loadVis(std::string visName);

    //==============================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    FileHandler openFile;


    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
