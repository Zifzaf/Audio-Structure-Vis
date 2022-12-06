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

  addAndMakeVisible(&setSpectrogram);
  setSpectrogram.setButtonText("Distribution Focus");
  setSpectrogram.onClick = [this]
  { setSpectrogramCall(); };
  setSpectrogram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setSpectrogram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setSpectrogram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

  addAndMakeVisible(&setHistgram);
  setHistgram.setButtonText("Time Focus");
  setHistgram.onClick = [this]
  { setHistgramCall(); };
  setHistgram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setHistgram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setHistgram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

  addAndMakeVisible(&setFrequencygram);
  setFrequencygram.setButtonText("Frequency Focus");
  setFrequencygram.onClick = [this]
  { setFrequencygramCall(); };
  setFrequencygram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setFrequencygram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setFrequencygram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

  addAndMakeVisible(&setWavegram);
  setWavegram.setButtonText("Wave Focus");
  setWavegram.onClick = [this]
  { setWavegramCall(); };
  setWavegram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setWavegram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setWavegram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

  addAndMakeVisible(&setWaveform);
  setWaveform.setButtonText("Wave Form");
  setWaveform.onClick = [this]
  { setWaveformCall(); };
  setWaveform.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setWaveform.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setWaveform.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
}

WaveogramUI::~WaveogramUI()
{
  delete &audioData;
}

void WaveogramUI::threshholdClicked()
{
  waveData.setThreshhold(threshhold.getText().getFloatValue());
  waveData.redrawImageCall();
}

void WaveogramUI::clipClicked()
{
  waveData.setClip(clip.getText().getFloatValue());
  waveData.redrawImageCall();
}

void WaveogramUI::levelBinNumClicked()
{
  waveData.setLevelBinNum(levelBinNum.getText().getIntValue());
  waveData.redrawImageCall();
}

void WaveogramUI::horizontalLinesClicked()
{
  waveData.setHorizontalLines(horizontalLines.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::verticalLablesClicked()
{
  waveData.setVerticalLables(verticalLables.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::horizontalLablesClicked()
{
  waveData.setHorizontalLables(horizontalLables.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::drawEllipseClicked()
{
  waveData.setDrawEllipse(drawEllipse.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::normalizeFrequencyDimClicked()
{
  waveData.setNormalizeFrequencyDim(normalizeFrequencyDim.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::normalizeTimeDimClicked()
{
  waveData.setNormalizeTimeDim(normalizeTimeDim.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::scaleVerticalClicked()
{
  waveData.setScaleVertical(scaleVertical.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::scaleHorizontalClicked()
{
  waveData.setScaleHorizontal(scaleHorizontal.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::levelBinLogScaleClicked()
{
  waveData.setLevelBinLogScale(levelBinLogScale.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::loudnessCorrectionClicked()
{
  waveData.setLoudnessCorrection(loudnessCorrection.getToggleState());
  waveData.calculateValueArrayCall();
}

void WaveogramUI::centeredClicked()
{
  waveData.setCentered(centered.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::setSpectrogramCall()
{
  waveData.setFrequencyBins(114);
  frequencyBinInput.setText("114", false);

  if (audioAvailable.get())
  {
    waveData.setTimeBinSize((int)(0.01 * fileInput->getSampleRate()));
    timeBinInput.setText(std::to_string((int)(0.01 * fileInput->getSampleRate())), false);
  }
  else
  {
    waveData.setTimeBinSize(512);
    timeBinInput.setText("512", false);
  }

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setThreshhold(0.01);
  threshhold.setText("0.01", false);

  waveData.setClip(0.75);
  clip.setText("0.75", false);

  waveData.setLevelBinNum(0);
  levelBinNum.setText("0", false);

  waveData.setLevelBinLogScale(false);
  levelBinLogScale.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLines(true);
  horizontalLines.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(false);
  drawEllipse.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(false);
  normalizeFrequencyDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(false);
  normalizeTimeDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(true);
  scaleVertical.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(true);
  scaleHorizontal.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setCentered(true);
  centered.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setZoom(2.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setHistgramCall()
{
  waveData.setFrequencyBins(19);
  frequencyBinInput.setText("19", false);

  if (audioAvailable.get())
  {
    waveData.setTimeBinSize((int)(0.01 * fileInput->getSampleRate()));
    timeBinInput.setText(std::to_string((int)(0.01 * fileInput->getSampleRate())), false);
  }
  else
  {
    waveData.setTimeBinSize(512);
    timeBinInput.setText("512", false);
  }

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setThreshhold(0.01);
  threshhold.setText("0.01", false);

  waveData.setClip(1.0);
  clip.setText("1.0", false);

  waveData.setLevelBinNum(0);
  levelBinNum.setText("0", false);

  waveData.setLevelBinLogScale(false);
  levelBinLogScale.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLines(true);
  horizontalLines.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(true);
  horizontalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(false);
  drawEllipse.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(true);
  normalizeFrequencyDim.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(true);
  normalizeTimeDim.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(true);
  scaleVertical.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(false);
  scaleHorizontal.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setCentered(false);
  centered.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setZoom(2.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWavegramCall()
{
  waveData.setFrequencyBins(38);
  frequencyBinInput.setText("38", false);

  if (audioAvailable.get())
  {
    waveData.setTimeBinSize((int)(0.01 * fileInput->getSampleRate()));
    timeBinInput.setText(std::to_string((int)(0.01 * fileInput->getSampleRate())), false);
  }
  else
  {
    waveData.setTimeBinSize(512);
    timeBinInput.setText("512", false);
  }

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setThreshhold(0.01);
  threshhold.setText("0.01", false);

  waveData.setClip(1.0);
  clip.setText("1.0", false);

  waveData.setLevelBinNum(0);
  levelBinNum.setText("0", false);

  waveData.setLevelBinLogScale(false);
  levelBinLogScale.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLines(false);
  horizontalLines.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(true);
  horizontalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(true);
  drawEllipse.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(true);
  normalizeFrequencyDim.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(false);
  normalizeTimeDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(true);
  scaleVertical.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(false);
  scaleHorizontal.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setCentered(true);
  centered.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setZoom(2.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWaveformCall()
{
  waveData.setFrequencyBins(1);
  frequencyBinInput.setText("1", false);

  if (audioAvailable.get())
  {
    waveData.setTimeBinSize((int)(0.001 * fileInput->getSampleRate()));
    timeBinInput.setText(std::to_string((int)(0.001 * fileInput->getSampleRate())), false);
  }
  else
  {
    waveData.setTimeBinSize(64);
    timeBinInput.setText("64", false);
  }

  waveData.setLoudnessCorrection(false);
  loudnessCorrection.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setThreshhold(0.001);
  threshhold.setText("0.001", false);

  waveData.setClip(1.0);
  clip.setText("1.0", false);

  waveData.setLevelBinNum(0);
  levelBinNum.setText("0", false);

  waveData.setLevelBinLogScale(false);
  levelBinLogScale.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLines(false);
  horizontalLines.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(true);
  drawEllipse.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(false);
  normalizeFrequencyDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(false);
  normalizeTimeDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(true);
  scaleVertical.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(false);
  scaleHorizontal.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setCentered(true);
  centered.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setZoom(0.5);

  waveData.calculateFTTCall();
}

void WaveogramUI::setFrequencygramCall()
{
  waveData.setFrequencyBins(57);
  frequencyBinInput.setText("57", false);

  if (audioAvailable.get())
  {
    waveData.setTimeBinSize((int)(0.1 * fileInput->getSampleRate()));
    timeBinInput.setText(std::to_string((int)(0.1 * fileInput->getSampleRate())), false);
  }
  else
  {
    waveData.setTimeBinSize(4096);
    timeBinInput.setText("4096", false);
  }

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setThreshhold(0.0);
  threshhold.setText("0.0", false);

  waveData.setClip(0.5);
  clip.setText("0.5", false);

  waveData.setLevelBinNum(0);
  levelBinNum.setText("0", false);

  waveData.setLevelBinLogScale(false);
  levelBinLogScale.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLines(true);
  horizontalLines.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(true);
  horizontalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(false);
  drawEllipse.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(true);
  normalizeFrequencyDim.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(false);
  normalizeTimeDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(false);
  scaleVertical.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(true);
  scaleHorizontal.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setCentered(false);
  centered.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setZoom(0.125);

  waveData.calculateFTTCall();
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

  frequencyBinInput.setBounds(2, 2, width / 10, 20);
  timeBinInput.setBounds(2 + width / 10, 2, width / 10, 20);
  calcButton.setBounds(2 + 2 * width / 10, 2, width / 10, 20);
  loudnessCorrection.setBounds(2 + 3 * width / 10, 2, width / 10, 20);
  threshhold.setBounds(2 + 4 * width / 10, 2, width / 10, 20);
  clip.setBounds(2 + 5 * width / 10, 2, width / 10, 20);
  levelBinNum.setBounds(2 + 6 * width / 10, 2, width / 10, 20);
  levelBinLogScale.setBounds(2 + 7 * width / 10, 2, width / 10, 20);

  horizontalLines.setBounds(2 + 0 * width / 10, 24, width / 10, 20);
  verticalLables.setBounds(2 + 1 * width / 10, 24, width / 10, 20);
  horizontalLables.setBounds(2 + 2 * width / 10, 24, width / 10, 20);
  drawEllipse.setBounds(2 + 3 * width / 10, 24, width / 10, 20);
  normalizeFrequencyDim.setBounds(2 + 4 * width / 10, 24, width / 10, 20);
  normalizeTimeDim.setBounds(2 + 5 * width / 10, 24, width / 10, 20);
  scaleVertical.setBounds(2 + 6 * width / 10, 24, width / 10, 20);
  scaleHorizontal.setBounds(2 + 7 * width / 10, 24, width / 10, 20);
  centered.setBounds(2 + 8 * width / 10, 24, width / 10, 20);

  setSpectrogram.setBounds(2 + 9 * width / 10, 2, width / 10, 8);
  setFrequencygram.setBounds(2 + 9 * width / 10, 11, width / 10, 8);
  setHistgram.setBounds(2 + 9 * width / 10, 20, width / 10, 8);
  setWavegram.setBounds(2 + 9 * width / 10, 29, width / 10, 8);
  setWaveform.setBounds(2 + 9 * width / 10, 39, width / 10, 8);

  waveData.setBounds(2, 46, width, height - 46);
}