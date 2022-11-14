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
  waveFrom.replaceData(data, numSamples, false);
  waveFrom.showRMS(false);
  addAndMakeVisible(&waveFrom);
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
  int width = getWidth();
  int height = getHeight();
  int bottomWave = height - 2 - 100;
  double time = 1.0 / sampleRate * dataLength;
  int marks = width / 200;
  double interval = time / marks;
  double orderFull = std::floor(std::log10(interval));
  double order = (orderFull + 300.0) - ((int)(orderFull + 300.0) % 3) - 300.0;
  std::string unit = "";
  if (order >= 0.0)
  {
    order = 0.0;
    unit = "s";
  }
  if (order == -3.0)
  {
    unit = "ms";
  }
  if (order == -6.0)
  {
    unit = "us";
  }
  if (order == -9.0)
  {
    unit = "ns";
  }
  interval = std::floor(interval / std::pow(10.0, orderFull));
  interval *= std::pow(10.0, orderFull - order);
  double step = sampleRate * (interval * std::pow(10.0, order)) / (dataLength / (width - 4) + 1);
  for (double i = 2; i < width - 2; i = i + step)
  {
    g.fillRect(i, bottomWave, 1, 4);
    g.drawText(std::to_string((int)(((i - 2) / step) * interval)) + unit, i - 50, bottomWave + 4, 100, 10, juce::Justification::centredTop, true);
  }
}

void AnalyseWindow::paintOverChildren(juce::Graphics &g)
{
}
void AnalyseWindow::resized()
{
  int width = getWidth() - 4;
  int height = getHeight() - 4;
  waveFrom.setBounds(2, 2, width, height - 100);
}