/*
  ==============================================================================

    WaveogramUI.cpp
    Created: 26 Nov 2022 7:03:58am
    Author:  andreas

  ==============================================================================
*/

#include "WaveogramUI.h"

WaveogramUI::WaveogramUI(FileHandler *in) : fileInput(in), audioData(*new juce::AudioBuffer<float>(0, 0))
{
  addAndMakeVisible(&calcButton);
  calcButton.setButtonText("Calculate");
  calcButton.onClick = [this]
  { calcButtonClicked(); };
  calcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&waveData);

  addAndMakeVisible(&frequencyBinInput);
  frequencyBinInput.setText("19");
  frequencyBinInput.setInputRestrictions(2, "0123456789");
  frequencyBinInput.setMultiLine(false);

  addAndMakeVisible(&timeBinInput);
  timeBinInput.setText("2048");
  timeBinInput.setInputRestrictions(6, "0123456789");
  timeBinInput.setMultiLine(false);

  addAndMakeVisible(&threshhold);
  threshhold.setText("0.0");
  threshhold.setInputRestrictions(5, "0123456789.");
  threshhold.setMultiLine(false);
  threshhold.onReturnKey = [this]
  { threshholdClicked(); };

  addAndMakeVisible(&clip);
  clip.setText("1.0");
  clip.setInputRestrictions(5, "0123456789.");
  clip.setMultiLine(false);
  clip.onReturnKey = [this]
  { clipClicked(); };

  addAndMakeVisible(&levelBinNum);
  levelBinNum.setText("-1");
  levelBinNum.setInputRestrictions(5, "0123456789-");
  levelBinNum.setMultiLine(false);
  levelBinNum.setTitle("levelBinNum");
  levelBinNum.onReturnKey = [this]
  { levelBinNumClicked(); };

  addAndMakeVisible(&horizontalLines);
  horizontalLines.setState(juce::Button::buttonNormal);
  horizontalLines.setButtonText("horizontalLines");
  horizontalLines.onClick = [this]
  { horizontalLinesClicked(); };

  addAndMakeVisible(&verticalLables);
  verticalLables.setState(juce::Button::buttonNormal);
  verticalLables.setButtonText("verticalLables");
  verticalLables.onClick = [this]
  { verticalLablesClicked(); };

  addAndMakeVisible(&horizontalLables);
  horizontalLables.setState(juce::Button::buttonNormal);
  horizontalLables.setButtonText("horizontalLables");
  horizontalLables.onClick = [this]
  { horizontalLablesClicked(); };
  horizontalLables.triggerClick();

  addAndMakeVisible(&drawEllipse);
  drawEllipse.setState(juce::Button::buttonNormal);
  drawEllipse.setButtonText("drawEllipse");
  drawEllipse.onClick = [this]
  { drawEllipseClicked(); };
  drawEllipse.triggerClick();

  addAndMakeVisible(&normalizeFrequencyDim);
  normalizeFrequencyDim.setState(juce::Button::buttonNormal);
  normalizeFrequencyDim.setButtonText("normalizeFrequencyDim");
  normalizeFrequencyDim.onClick = [this]
  { normalizeFrequencyDimClicked(); };

  addAndMakeVisible(&normalizeTimeDim);
  normalizeTimeDim.setState(juce::Button::buttonNormal);
  normalizeTimeDim.setButtonText("normalizeTimeDim");
  normalizeTimeDim.onClick = [this]
  { normalizeTimeDimClicked(); };

  addAndMakeVisible(&scaleVertical);
  scaleVertical.setState(juce::Button::buttonNormal);
  scaleVertical.setButtonText("scaleVertical");
  scaleVertical.onClick = [this]
  { scaleVerticalClicked(); };
  scaleVertical.triggerClick();

  addAndMakeVisible(&scaleHorizontal);
  scaleHorizontal.setState(juce::Button::buttonNormal);
  scaleHorizontal.setButtonText("scaleHorizontal");
  scaleHorizontal.onClick = [this]
  { scaleHorizontalClicked(); };

  addAndMakeVisible(&levelBinLogScale);
  levelBinLogScale.setState(juce::Button::buttonNormal);
  levelBinLogScale.setButtonText("levelBinLogScale");
  levelBinLogScale.onClick = [this]
  { levelBinLogScaleClicked(); };

  addAndMakeVisible(&loudnessCorrection);
  loudnessCorrection.setState(juce::Button::buttonNormal);
  loudnessCorrection.setButtonText("loudnessCorrection");
  loudnessCorrection.onClick = [this]
  { loudnessCorrectionClicked(); };

  addAndMakeVisible(&centered);
  centered.setState(juce::Button::buttonNormal);
  centered.setButtonText("centered");
  centered.onClick = [this]
  { centeredClicked(); };
  centered.triggerClick();
}

WaveogramUI::~WaveogramUI()
{
  delete &audioData;
}

void WaveogramUI::threshholdClicked()
{
  waveData.setThreshhold(threshhold.getText().getFloatValue());
}

void WaveogramUI::clipClicked()
{
  waveData.setClip(clip.getText().getFloatValue());
}

void WaveogramUI::levelBinNumClicked()
{
  waveData.setLevelBinNum(levelBinNum.getText().getIntValue());
}

void WaveogramUI::horizontalLinesClicked()
{
  waveData.setHorizontalLines(horizontalLines.getToggleState());
}

void WaveogramUI::verticalLablesClicked()
{
  waveData.setVerticalLables(verticalLables.getToggleState());
}

void WaveogramUI::horizontalLablesClicked()
{
  waveData.setHorizontalLables(horizontalLables.getToggleState());
}

void WaveogramUI::drawEllipseClicked()
{
  waveData.setDrawEllipse(drawEllipse.getToggleState());
}

void WaveogramUI::normalizeFrequencyDimClicked()
{
  waveData.setNormalizeFrequencyDim(normalizeFrequencyDim.getToggleState());
}

void WaveogramUI::normalizeTimeDimClicked()
{
  waveData.setNormalizeTimeDim(normalizeTimeDim.getToggleState());
}

void WaveogramUI::scaleVerticalClicked()
{
  waveData.setScaleVertical(scaleVertical.getToggleState());
}

void WaveogramUI::scaleHorizontalClicked()
{
  waveData.setScaleHorizontal(scaleHorizontal.getToggleState());
}

void WaveogramUI::levelBinLogScaleClicked()
{
  waveData.setLevelBinLogScale(levelBinLogScale.getToggleState());
}

void WaveogramUI::loudnessCorrectionClicked()
{
  waveData.setLoudnessCorrection(loudnessCorrection.getToggleState());
}

void WaveogramUI::centeredClicked()
{
  waveData.setCentered(centered.getToggleState());
}

void WaveogramUI::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == fileInput)
  {
    audioAvailable.set(false);
    auto len = fileInput->getSegmentLength();
    if (len > audioData.getNumSamples())
    {
      delete &audioData;
      audioData = *new juce::AudioBuffer<float>(1, len);
    }
    audioAvailable.set(true);
  }
}

void WaveogramUI::calcButtonClicked()
{
  if (audioAvailable.get() && calculating.compareAndSetBool(true, false))
  {
    // Start Calcualtion
    calcButton.setEnabled(false);

    // Get audio data to process
    fileInput->getAudioBlock(&audioData);
    const float *inData = audioData.getReadPointer(0);
    auto segmentLength = fileInput->getSegmentLength();
    auto sampleRate = fileInput->getSampleRate();

    waveData.setTimeBinSize(getTimeBinSize());
    waveData.setFrequencyBins(getFrequencyBins());

    waveData.setRawAudio(inData, segmentLength, sampleRate);

    calcButton.setEnabled(true);
    calculating.set(false);
  }
}

void WaveogramUI::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

int WaveogramUI::getFrequencyBins()
{
  return frequencyBinInput.getText().getIntValue();
}

int WaveogramUI::getTimeBinSize()
{
  return timeBinInput.getText().getIntValue();
}

void WaveogramUI::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void WaveogramUI::resized()
{
  int width = getWidth() - 4;
  int height = getHeight();

  frequencyBinInput.setBounds(2, 2, width / 8, 20);
  timeBinInput.setBounds(2 + width / 8, 2, width / 8, 20);
  calcButton.setBounds(2 + 2 * width / 8, 2, width / 8, 20);
  loudnessCorrection.setBounds(2 + 3 * width / 8, 2, width / 8, 20);
  threshhold.setBounds(2 + 4 * width / 8, 2, width / 8, 20);
  clip.setBounds(2 + 5 * width / 8, 2, width / 8, 20);
  levelBinNum.setBounds(2 + 6 * width / 8, 2, width / 8, 20);
  levelBinLogScale.setBounds(2 + 7 * width / 8, 2, width / 8, 20);

  horizontalLines.setBounds(2 + 0 * width / 10, 24, width / 10, 20);
  verticalLables.setBounds(2 + 1 * width / 10, 24, width / 10, 20);
  horizontalLables.setBounds(2 + 2 * width / 10, 24, width / 10, 20);
  drawEllipse.setBounds(2 + 3 * width / 10, 24, width / 10, 20);
  normalizeFrequencyDim.setBounds(2 + 4 * width / 10, 24, width / 10, 20);
  normalizeTimeDim.setBounds(2 + 5 * width / 10, 24, width / 10, 20);
  scaleVertical.setBounds(2 + 6 * width / 10, 24, width / 10, 20);
  scaleHorizontal.setBounds(2 + 7 * width / 10, 24, width / 10, 20);
  centered.setBounds(2 + 8 * width / 10, 24, width / 10, 20);

  waveData.setBounds(2, 46, width, height - 46);
}