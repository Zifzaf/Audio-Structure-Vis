/*
  ==============================================================================

    WaveogramUI.cpp
    Created: 26 Nov 2022 7:03:58am
    Author:  andreas

  ==============================================================================
*/

#include "WaveogramUI.h"

WaveogramUI::WaveogramUI(FileHandler *in) : fileInput(in)
{
  addAndMakeVisible(&calcButton);
  calcButton.setButtonText("Calculate");
  calcButton.onClick = [this]
  { calcButtonClicked(); };
  calcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&waveData);

  addAndMakeVisible(&notesPerBinInput);
  notesPerBinInput.addItem("1", 1);
  notesPerBinInput.addItem("2", 2);
  notesPerBinInput.addItem("3", 3);
  notesPerBinInput.addItem("4", 4);
  notesPerBinInput.addItem("5", 5);
  notesPerBinInput.addItem("6", 6);
  notesPerBinInput.addItem("8", 7);
  notesPerBinInput.addItem("10", 8);
  notesPerBinInput.addItem("12", 9);
  notesPerBinInput.addItem("15", 10);
  notesPerBinInput.addItem("20", 11);
  notesPerBinInput.addItem("24", 12);
  notesPerBinInput.addItem("30", 13);
  notesPerBinInput.addItem("40", 14);
  notesPerBinInput.addItem("60", 15);
  notesPerBinInput.addItem("120", 16);
  notesPerBinInput.setSelectedId(1, juce::NotificationType::dontSendNotification);
  notesPerBinInput.setTooltip("Choose the number of halfsteps which are displayed as one frequency bin on the y axis!");

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
  timeBinInput.setTooltip("Choose how time is discretized. If the bins are short low frequencies can't be displayed accurately.");

  addAndMakeVisible(&threshhold);
  threshhold.setText("-40.0");
  threshhold.setInputRestrictions(5, "0123456789.-inf");
  threshhold.setMultiLine(false);
  threshhold.onReturnKey = [this]
  { threshholdClicked(); };
  threshhold.setTooltip("Choose a lower threshhold below which the values are displayed as zero. You can use the histogram displayed to help select a good value. The value is choosen in dB releative to the max level 1.0. Therefore the value needs to be negative to have an effect.");

  addAndMakeVisible(&clip);
  clip.setText("-2.5");
  clip.setInputRestrictions(5, "0123456789.-inf");
  clip.setMultiLine(false);
  clip.onReturnKey = [this]
  { clipClicked(); };
  clip.setTooltip("Choose an upper limit for values displayed. Values above this level will be displayed in a darker orange an will all have the same size. The value is choosen in dB releative to the max level 1.0. Therefore the value needs to be negative to have an effect.");

  addAndMakeVisible(&horizontalLines);
  horizontalLines.setState(juce::Button::buttonNormal);
  horizontalLines.setButtonText("Show Frequency Bin Borders");
  horizontalLines.onClick = [this]
  { horizontalLinesClicked(); };
  horizontalLines.setTooltip("Select if lines to seperate the frequency bins are displayed.");

  addAndMakeVisible(&verticalLables);
  verticalLables.setState(juce::Button::buttonNormal);
  verticalLables.setButtonText("Show Time Labels");
  verticalLables.onClick = [this]
  { verticalLablesClicked(); };
  verticalLables.setTooltip("Select if time marker are displayed.");

  addAndMakeVisible(&horizontalLables);
  horizontalLables.setState(juce::Button::buttonNormal);
  horizontalLables.setButtonText("Show Frequency Border Values");
  horizontalLables.onClick = [this]
  { horizontalLablesClicked(); };
  horizontalLables.setTooltip("Select if frequency labels for the frequency bin borders are displayed. Enable 'Show Frequency Bin Borders' to see the corresponding lines.");

  addAndMakeVisible(&drawEllipse);
  drawEllipse.setState(juce::Button::buttonNormal);
  drawEllipse.setButtonText("Use Ellipse/Rectangle");
  drawEllipse.onClick = [this]
  { drawEllipseClicked(); };
  drawEllipse.setTooltip("Select wether Ellipses or Rectangles are used to visulaize the levels.");

  addAndMakeVisible(&normalizeFrequencyDim);
  normalizeFrequencyDim.setState(juce::Button::buttonNormal);
  normalizeFrequencyDim.setButtonText("Normalize Color over Frequnency");
  normalizeFrequencyDim.onClick = [this]
  { normalizeFrequencyDimClicked(); };
  normalizeFrequencyDim.setTooltip("If selected color is normalized over the corresponding time bin. So you can't use color anymore to compare in time, but it indicates the level at a set time. If not selected color displays the global level.");

  addAndMakeVisible(&normalizeTimeDim);
  normalizeTimeDim.setState(juce::Button::buttonNormal);
  normalizeTimeDim.setButtonText("Normalize Size over Time");
  normalizeTimeDim.onClick = [this]
  { normalizeTimeDimClicked(); };
  normalizeTimeDim.setTooltip("If selected size is normalized over the corresponding frequency bin. So you can't use size anymore to compare different frequencies, but it now indicates the level relative to other levels in the same frequency range. If not selected size displays the global level.");

  addAndMakeVisible(&scaleVertical);
  scaleVertical.setState(juce::Button::buttonNormal);
  scaleVertical.setButtonText("Show Level on Height Scaling");
  scaleVertical.onClick = [this]
  { scaleVerticalClicked(); };
  scaleVertical.setTooltip("If selected the height of the glyth is scaled according to the level. If not selected height is keept constant.");

  addAndMakeVisible(&scaleHorizontal);
  scaleHorizontal.setState(juce::Button::buttonNormal);
  scaleHorizontal.setButtonText("Show Level on Width Scaling");
  scaleHorizontal.onClick = [this]
  { scaleHorizontalClicked(); };
  scaleHorizontal.setTooltip("If selected the width of the glyth is scaled according to the level. If not selected width is keept constant.");

  addAndMakeVisible(&loudnessCorrection);
  loudnessCorrection.setState(juce::Button::buttonNormal);
  loudnessCorrection.setButtonText("Enable Loudness Crorrection");
  loudnessCorrection.onClick = [this]
  { loudnessCorrectionClicked(); };
  loudnessCorrection.setTooltip("If selected level of frequencies is scaled such that they correspond to the level perceived at 80 dB SPL. If not selected the unscaled values are displayed.");

  addAndMakeVisible(&centered);
  centered.setState(juce::Button::buttonNormal);
  centered.setButtonText("Center Glyth");
  centered.onClick = [this]
  { centeredClicked(); };
  centered.setTooltip("Select wether the glyth is centered at it's position or placed at the bottom lefthand corner.");

  addAndMakeVisible(&setSpectrogram);
  setSpectrogram.setButtonText("Distribution Focus");
  setSpectrogram.onClick = [this]
  { setSpectrogramCall(); };
  setSpectrogram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setSpectrogram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setSpectrogram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setSpectrogram.setTooltip("If pressed a spectogram like visualization of the file is displayed. Each frequency bin corresponds to a single note so it is useful to get an overview.");

  addAndMakeVisible(&setHistgram);
  setHistgram.setButtonText("Time Focus");
  setHistgram.onClick = [this]
  { setHistgramCall(); };
  setHistgram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setHistgram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setHistgram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setHistgram.setTooltip("If pressed a visualization is displayed which shows the frequency bins as histograms. It uses time and frequency normalization, so color corresponds the level over frequencies and size to the level over time. It is useful to analize the decay of harmonic instruments.");

  addAndMakeVisible(&setFrequencygram);
  setFrequencygram.setButtonText("Frequency Focus");
  setFrequencygram.onClick = [this]
  { setFrequencygramCall(); };
  setFrequencygram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setFrequencygram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setFrequencygram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setFrequencygram.setTooltip("If pressed a visualization is displayed which show the frequency variation in big time steps. It is useful to get an overview over the broad frequency contents of a recording.");

  addAndMakeVisible(&setWavegram);
  setWavegram.setButtonText("Wave Focus");
  setWavegram.onClick = [this]
  { setWavegramCall(); };
  setWavegram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setWavegram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setWavegram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setWavegram.setTooltip("If pressed a visualization is displayed which shows waveforms for each frequency bin. Normalization of color over frequencies is enabled so color indicates the highest frequency level at a spesific time. It is useful to judge decay times and frequency distributions of rythm instruments.");

  addAndMakeVisible(&setWaveform);
  setWaveform.setButtonText("Wave Form");
  setWaveform.onClick = [this]
  { setWaveformCall(); };
  setWaveform.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setWaveform.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setWaveform.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setWaveform.setTooltip("If pressed a normal waveform vislualization is displayed.");

  addAndMakeVisible(&frequencyLabels);
  frequencyLabels.setState(juce::Button::buttonNormal);
  frequencyLabels.setButtonText("Show Frequency Labels");
  frequencyLabels.onClick = [this]
  { frequencyLabelsClicked(); };
  frequencyLabels.setTooltip("If selected labeled lines for common freqencies are drawn over the visualiztion to indicate where the level for these are displayed.");

  addAndMakeVisible(&selectionToFocus);
  selectionToFocus.setButtonText("Focus on Selection");
  selectionToFocus.onClick = [this]
  { selectionToFocusClicked(); };
  selectionToFocus.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgreen);
  selectionToFocus.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  selectionToFocus.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  selectionToFocus.setTooltip("If pressed the view is switched to only display the selected part of the visulaization.");

  audioData = NULL;
  setSpectrogramCall();
}

WaveogramUI::~WaveogramUI()
{
  delete audioData;
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
    out = fileInput->getSampleRate();
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
  std::cout << out << std::endl;
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

void WaveogramUI::selectionToFocusClicked()
{
  float currentSelection[4];
  int currentSelectionPixel[4];
  float timeZero = fileInput->getStartTime();
  waveData.getSelection(&currentSelection[0], &currentSelectionPixel[0]);
  fileInput->setStartTime(currentSelection[0] + timeZero);
  fileInput->setEndTime(currentSelection[1] + timeZero);
  waveData.resetSelection();
  waveData.setZoom(waveData.getZoom() * 0.5);
  calcButtonClicked();
}

void WaveogramUI::setSpectrogramCall()
{
  notesPerBinInput.setSelectedId(1);
  waveData.setNotesPerBin(getNotesPerBin());

  timeBinInput.setSelectedId(4);
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

  waveData.setZoom(8.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setHistgramCall()
{
  notesPerBinInput.setSelectedId(6);
  waveData.setNotesPerBin(getNotesPerBin());

  timeBinInput.setSelectedId(4);
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

  waveData.setZoom(8.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWavegramCall()
{
  notesPerBinInput.setSelectedId(4);
  waveData.setNotesPerBin(getNotesPerBin());

  timeBinInput.setSelectedId(4);
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

  waveData.setZoom(8.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setWaveformCall()
{
  notesPerBinInput.setSelectedId(16);
  waveData.setNotesPerBin(getNotesPerBin());

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
  notesPerBinInput.setSelectedId(4);
  waveData.setNotesPerBin(getNotesPerBin());

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

  waveData.setZoom(8.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == fileInput)
  {
    audioAvailable.set(false);
    auto len = fileInput->getSegmentLength();
    if (audioData == NULL || len > audioData->getNumSamples())
    {
      auto newAudioData = new juce::AudioBuffer<float>(1, len);
      auto tempAudioData = audioData;
      audioData = newAudioData;
      delete tempAudioData;
    }
    audioAvailable.set(true);
    waveData.setViewerPosition(fileInput->getCurrentTime() - fileInput->getStartTime());
  }
}

void WaveogramUI::calcButtonClicked()
{
  if (audioAvailable.get() && calculating.compareAndSetBool(true, false))
  {
    // Start Calcualtion
    calcButton.setEnabled(false);

    // Get audio data to process
    fileInput->getAudioBlock(audioData);
    const float *inData = audioData->getReadPointer(0);
    auto segmentLength = fileInput->getSegmentLength();
    auto sampleRate = fileInput->getSampleRate();

    waveData.setTimeBinSize(getTimeBinSize());
    waveData.setNotesPerBin(getNotesPerBin());

    waveData.setRawAudio(inData, segmentLength, sampleRate);

    calcButton.setEnabled(true);
    calculating.set(false);
  }
}

void WaveogramUI::addFileHandler(FileHandler *in)
{
  fileInput = in;
}

int WaveogramUI::getNotesPerBin()
{
  return notesPerBinInput.getText().getIntValue();
}

void WaveogramUI::paint(juce::Graphics &g)
{
  int width = getWidth() - 4;
  int height = getHeight();
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.setColour(juce::Colours::white);
  g.drawText("Number of Notes per Bin: ", 2 + 0 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Time Bin length [s]: ", 2 + 2 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Cut Threshhold [dB]: ", 2 + 4 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
  g.drawText("Clip Limit [dB]: ", 2 + 6 * width / 10, 2, width / 10, 20, juce::Justification::centredRight, false);
}

void WaveogramUI::resized()
{
  int width = getWidth() - 4;
  int height = getHeight();

  notesPerBinInput.setBounds(2 + 1 * width / 10, 2, width / 10, 20);
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
  selectionToFocus.setBounds(2 + 5 * width / 10, 46, width / 10, 20);
  calcButton.setBounds(2 + 8 * width / 10, 46, width / 5, 20);

  waveData.setBounds(2, 68, width, height - 68);
}