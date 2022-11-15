/*
  ==============================================================================

    WaveformDisplayWraper.cpp
    Created: 15 Nov 2022 8:35:39am
    Author:  andreas

  ==============================================================================
*/

#include "WaveformDisplayWraper.h"

WaveformDisplayWraper::WaveformDisplayWraper() : waveForm()
{

  addAndMakeVisible(&calcButton);
  calcButton.setButtonText("Calculate");
  calcButton.onClick = [this]
  { calcButtonClicked(); };
  calcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&waveForm);
}

WaveformDisplayWraper::~WaveformDisplayWraper()
{
}

void WaveformDisplayWraper::calcButtonClicked()
{
  if (fileInput != nullptr && fileInput->getTotalLength() != 0)
  {
    auto len = fileInput->getSegmentLength();
    juce::AudioBuffer<float> audioData(1, len);
    fileInput->getAudioBlock(&audioData);
    std::cout << audioData.getReadPointer(0) << std::endl;
    waveForm.replaceData(audioData.getReadPointer(0), len, true, fileInput->getSampleRate());
    repaint();
  }
}

void WaveformDisplayWraper::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void WaveformDisplayWraper::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

void WaveformDisplayWraper::resized()
{
  int width = getWidth() - 4;
  int height = getHeight() - 6;
  calcButton.setBounds(2, 2, width, 20);
  waveForm.setBounds(2, 24, width, height - 24);
}