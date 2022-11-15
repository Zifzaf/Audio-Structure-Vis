/*
  ==============================================================================

    AnalyseWindow.cpp
    Created: 8 Nov 2022 11:51:50am
    Author:  andreas

  ==============================================================================
*/

#include "AnalyseWindow.h"

AnalyseWindow::AnalyseWindow(const float *inData, size_t numSamples, double sampleRate) : juce::DocumentWindow("AnalyserWindow",
                                                                                                               juce::Colours::darkgrey,
                                                                                                               juce::DocumentWindow::TitleBarButtons::closeButton | juce::DocumentWindow::TitleBarButtons::minimiseButton,
                                                                                                               false),
                                                                                          waveFrom(),
                                                                                          sampleRate(sampleRate)
{
  data = new float[numSamples];
  juce::FloatVectorOperations::copy(data, inData, numSamples);
  dataLength = numSamples;
  waveFrom.replaceData(data, numSamples, true, sampleRate);
  waveFrom.showRMS(false);
  setContentOwned(&waveFrom, false);
}

AnalyseWindow::~AnalyseWindow()
{
  delete data;
}

void AnalyseWindow::closeButtonPressed()
{
  delete this;
}

void AnalyseWindow::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AnalyseWindow::paintOverChildren(juce::Graphics &g)
{
}
void AnalyseWindow::resized()
{
  int width = getWidth() - 4;
  int height = getHeight() - 4;
  waveFrom.setBounds(2, 2, width, height);
}