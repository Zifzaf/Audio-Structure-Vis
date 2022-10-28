/*
  ==============================================================================

    EnergyBands.h
    Created: 28 Oct 2022 11:17:58am
    Author:  andreas

  ==============================================================================
*/
#include <JuceHeader.h>
#include <kfr/all.hpp>
#include "Histogram.h"
#include "FileHandler.h"

#pragma once
class EnergyBands : public juce::Component, public juce::ChangeListener
{
public:
  //==============================================================================
  EnergyBands();
  ~EnergyBands() override;
  void changeListenerCallback(juce::ChangeBroadcaster *source);

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;
  std::array<float, 25> criticalBandCuts = {20, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500};

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnergyBands)
};
