#pragma once

#include <JuceHeader.h>
#include "Histogram.h"
#include "FileHandler.h"
#include "NoteShower.h"
#include <kfr/all.hpp>
#include <chrono>



enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void visualize(const float * buffer, int startSample, int numSamples);
    static const size_t fftSize = 8192;
    const size_t numLevles = (size_t) std::log2(fftSize/2);
    kfr::univector<kfr::fbase, fftSize> inBuffer;
    kfr::univector<kfr::complex<kfr::fbase>, fftSize> outBuffer;
    kfr::dft_plan_real<kfr::fbase> dft;
    kfr::univector<kfr::u8> *temp;
    int inBufferIndex = 0; 
    NoteShower notes;
    

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    FileHandler openFile;
    Histogram spectro;


    //==============================================================================
    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
