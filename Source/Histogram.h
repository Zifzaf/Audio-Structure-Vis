/*
  ==============================================================================

    Histogram.h
    Created: 20 Oct 2022 9:08:59am
    Author:  andreas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class Histogram  : public juce::AnimatedAppComponent
{
public:
    //==============================================================================
    Histogram(int dataLevels);
    Histogram(float* data, int dataLevels, int dataWidth);
    ~Histogram() override;
    

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void update() override;
    void addDataLine(float dataLine[], bool normalized);

private:                  
    juce::Image histogramImage;
    int dataLevels = 1;
    int* heightDataMap;
    int widthAvailable = 0;
    int heightAvailable = 0;
    int levelWidth = 50;
    int dataLength = 1;
    float* data;
    bool fixedLengthData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Histogram)
};
