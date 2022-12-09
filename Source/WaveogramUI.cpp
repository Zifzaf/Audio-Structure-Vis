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
  frequencyBinInput.setInputRestrictions(3, "0123456789");
  frequencyBinInput.setMultiLine(false);

  addAndMakeVisible(&timeBinInput);
  timeBinInput.addItem("1/100", 1);
  timeBinInput.addItem("1/50", 2);
  timeBinInput.addItem("1/20", 3);
  timeBinInput.addItem("1/10", 4);
  timeBinInput.addItem("1/5", 5);
  timeBinInput.addItem("1/2", 6);
  timeBinInput.addItem("1", 7);
  timeBinInput.addItem("2", 8);
  timeBinInput.addItem("5", 9);
  timeBinInput.addItem("10", 10);
  timeBinInput.setSelectedId(4, juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&threshhold);
  threshhold.setText("-inf");
  threshhold.setInputRestrictions(5, "0123456789.-inf");
  threshhold.setMultiLine(false);
  threshhold.onReturnKey = [this]
  { threshholdClicked(); };

  addAndMakeVisible(&clip);
  clip.setText("0.0");
  clip.setInputRestrictions(5, "0123456789.-inf");
  clip.setMultiLine(false);
  clip.onReturnKey = [this]
  { clipClicked(); };

  addAndMakeVisible(&horizontalLines);
  horizontalLines.setState(juce::Button::buttonNormal);
  horizontalLines.setButtonText("Show Frequency Bin Borders");
  horizontalLines.onClick = [this]
  { horizontalLinesClicked(); };

  addAndMakeVisible(&verticalLables);
  verticalLables.setState(juce::Button::buttonNormal);
  verticalLables.setButtonText("Show Time Labels");
  verticalLables.onClick = [this]
  { verticalLablesClicked(); };

  addAndMakeVisible(&horizontalLables);
  horizontalLables.setState(juce::Button::buttonNormal);
  horizontalLables.setButtonText("Show Frequency Border Values");
  horizontalLables.onClick = [this]
  { horizontalLablesClicked(); };
  horizontalLables.triggerClick();

  addAndMakeVisible(&drawEllipse);
  drawEllipse.setState(juce::Button::buttonNormal);
  drawEllipse.setButtonText("Use Ellipse/Rectangle");
  drawEllipse.onClick = [this]
  { drawEllipseClicked(); };
  drawEllipse.triggerClick();

  addAndMakeVisible(&normalizeFrequencyDim);
  normalizeFrequencyDim.setState(juce::Button::buttonNormal);
  normalizeFrequencyDim.setButtonText("Normalize Color over Frequnency");
  normalizeFrequencyDim.onClick = [this]
  { normalizeFrequencyDimClicked(); };

  addAndMakeVisible(&normalizeTimeDim);
  normalizeTimeDim.setState(juce::Button::buttonNormal);
  normalizeTimeDim.setButtonText("Normalize Size over Time");
  normalizeTimeDim.onClick = [this]
  { normalizeTimeDimClicked(); };

  addAndMakeVisible(&scaleVertical);
  scaleVertical.setState(juce::Button::buttonNormal);
  scaleVertical.setButtonText("Show Level on Verical Scaling");
  scaleVertical.onClick = [this]
  { scaleVerticalClicked(); };
  scaleVertical.triggerClick();

  addAndMakeVisible(&scaleHorizontal);
  scaleHorizontal.setState(juce::Button::buttonNormal);
  scaleHorizontal.setButtonText("Show Level on Horizontal Scaling");
  scaleHorizontal.onClick = [this]
  { scaleHorizontalClicked(); };

  addAndMakeVisible(&loudnessCorrection);
  loudnessCorrection.setState(juce::Button::buttonNormal);
  loudnessCorrection.setButtonText("Enable Loudness Crorrection");
  loudnessCorrection.onClick = [this]
  { loudnessCorrectionClicked(); };

  addAndMakeVisible(&centered);
  centered.setState(juce::Button::buttonNormal);
  centered.setButtonText("Center Glyth");
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

  addAndMakeVisible(&frequencyLabels);
  frequencyLabels.setState(juce::Button::buttonNormal);
  frequencyLabels.setButtonText("Show Frequency Labels");
  frequencyLabels.onClick = [this]
  { frequencyLabelsClicked(); };
}

WaveogramUI::~WaveogramUI()
{
  delete &audioData;
}

float WaveogramUI::getThreshhold()
{
  auto val = threshhold.getText().getFloatValue();
  if (val == -std::numeric_limits<float>::infinity())
  {
    return 0.0;
  }
  return std::pow(10.0, val / 20.0);
}

float WaveogramUI::getClip()
{
  auto val = clip.getText().getFloatValue();
  if (val == -std::numeric_limits<float>::infinity())
  {
    return 0.0;
  }
  return std::pow(10.0, val / 20.0);
}

int WaveogramUI::getTimeBinSize()
{
  int out = 48000;
  if (audioAvailable.get())
  {
    int out = fileInput->getSampleRate();
  }
  switch (timeBinInput.getSelectedId())
  {
  case 1:
    out = out * 0.01;
    break;

  case 2:
    out = out * 0.02;
    break;

  case 3:
    out = out * 0.05;
    break;

  case 4:
    out = out * 0.1;
    break;

  case 5:
    out = out * 0.2;
    break;

  case 6:
    out = out * 0.5;
    break;

  case 7:
    out = out * 1.0;
    break;

  case 8:
    out = out * 2.0;
    break;

  case 9:
    out = out * 5.0;
    break;

  case 10:
    out = out * 10.0;
    break;

  default:
    break;
  }
  return out;
}

void WaveogramUI::threshholdClicked()
{
  waveData.setThreshhold(getThreshhold());
  waveData.redrawImageCall();
}

void WaveogramUI::clipClicked()
{
  waveData.setClip(getClip());
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

void WaveogramUI::frequencyLabelsClicked()
{
  waveData.setFrequencyLabels(frequencyLabels.getToggleState());
  waveData.redrawImageCall();
}

void WaveogramUI::setSpectrogramCall()
{
  waveData.setFrequencyBins(114);
  frequencyBinInput.setText("114", false);

  timeBinInput.setSelectedId(2);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  threshhold.setText("-40.0", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("-2.5", false);
  waveData.setClip(getClip());

  waveData.setHorizontalLines(false);
  horizontalLines.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setVerticalLables(true);
  verticalLables.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setDrawEllipse(false);
  drawEllipse.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeFrequencyDim(true);
  normalizeFrequencyDim.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setNormalizeTimeDim(false);
  normalizeTimeDim.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setScaleVertical(true);
  scaleVertical.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setScaleHorizontal(true);
  scaleHorizontal.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setCentered(true);
  centered.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setFrequencyLabels(true);
  frequencyLabels.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setZoom(4.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setHistgramCall()
{
  waveData.setFrequencyBins(19);
  frequencyBinInput.setText("19", false);

  timeBinInput.setSelectedId(3);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  threshhold.setText("-40.0", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("0.0", false);
  waveData.setClip(getClip());

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

  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setZoom(4.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWavegramCall()
{
  waveData.setFrequencyBins(38);
  frequencyBinInput.setText("38", false);

  timeBinInput.setSelectedId(2);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  threshhold.setText("-40.0", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("0.0", false);
  waveData.setClip(getClip());

  waveData.setHorizontalLines(false);
  horizontalLines.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);

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

  waveData.setFrequencyLabels(true);
  frequencyLabels.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setZoom(4.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWaveformCall()
{
  waveData.setFrequencyBins(1);
  frequencyBinInput.setText("1", false);

  timeBinInput.setSelectedId(4);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(false);
  loudnessCorrection.setToggleState(false, juce::NotificationType::dontSendNotification);

  threshhold.setText("-60.0", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("0.0", false);
  waveData.setClip(getClip());

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

  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setZoom(4.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setFrequencygramCall()
{
  waveData.setFrequencyBins(57);
  frequencyBinInput.setText("57", false);

  timeBinInput.setSelectedId(7);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(true);
  loudnessCorrection.setToggleState(true, juce::NotificationType::dontSendNotification);

  threshhold.setText("-inf", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("-6.0", false);
  waveData.setClip(getClip());

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

  waveData.setCentered(true);
  centered.setToggleState(true, juce::NotificationType::dontSendNotification);

  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setZoom(4.0);

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

void WaveogramUI::paint(juce::Graphics &g)
{
  int width = getWidth() - 4;
  int height = getHeight();
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.setColour(juce::Colours::white);
  g.drawText("Number of Frquency Bins: ", 2 + 0 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Time Bin length [s]: ", 2 + 2 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Cut Threshhold [dB]: ", 2 + 4 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Clip Limit [dB]: ", 2 + 6 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
}

void WaveogramUI::resized()
{
  int width = getWidth() - 4;
  int height = getHeight();

  frequencyBinInput.setBounds(2 + 1 * width / 10, 2, width / 10, 20);
  timeBinInput.setBounds(2 + 3 * width / 10, 2, width / 10, 20);
  threshhold.setBounds(2 + 5 * width / 10, 2, width / 10, 20);
  clip.setBounds(2 + 7 * width / 10, 2, width / 10, 20);
  loudnessCorrection.setBounds(2 + 9 * width / 10, 2, width / 10, 20);

  scaleVertical.setBounds(2 + 0 * width / 10, 24, width / 10, 20);
  scaleHorizontal.setBounds(2 + 1 * width / 10, 24, width / 10, 20);
  centered.setBounds(2 + 2 * width / 10, 24, width / 10, 20);
  drawEllipse.setBounds(2 + 3 * width / 10, 24, width / 10, 20);
  normalizeFrequencyDim.setBounds(2 + 4 * width / 10, 24, width / 10, 20);
  normalizeTimeDim.setBounds(2 + 5 * width / 10, 24, width / 10, 20);
  verticalLables.setBounds(2 + 6 * width / 10, 24, width / 10, 20);
  frequencyLabels.setBounds(2 + 7 * width / 10, 24, width / 10, 20);
  horizontalLines.setBounds(2 + 8 * width / 10, 24, width / 10, 20);
  horizontalLables.setBounds(2 + 9 * width / 10, 24, width / 10, 20);

  setSpectrogram.setBounds(2 + 0 * width / 10, 46, width / 10, 20);
  setFrequencygram.setBounds(2 + 1 * width / 10, 46, width / 10, 20);
  setHistgram.setBounds(2 + 2 * width / 10, 46, width / 10, 20);
  setWavegram.setBounds(2 + 3 * width / 10, 46, width / 10, 20);
  setWaveform.setBounds(2 + 4 * width / 10, 46, width / 10, 20);
  calcButton.setBounds(2 + 9 * width / 10, 46, width / 10, 20);

  waveData.setBounds(2, 68, width, height - 68);
}