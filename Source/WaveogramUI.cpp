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

  addAndMakeVisible(&setNotegram);
  setNotegram.setButtonText("Note Focus");
  setNotegram.onClick = [this]
  { setNotegramCall(); };
  setNotegram.setColour(juce::TextButton::buttonColourId, juce::Colours::azure);
  setNotegram.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setNotegram.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setNotegram.setTooltip("If pressed a visualization is displayed which levels for each note bin. Normalization of color over frequencies is enabled so color indicates the highest frequency level at a spesific time. It is useful to find melodies from a recording.");

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

  addAndMakeVisible(&lowNote);
  lowNote.setTooltip("Choose the lowest note/frequency displayed.");
  lowNote.onChange = [this]
  { lowNoteChanged(); };

  addAndMakeVisible(&highNote);
  highNote.setTooltip("Choose the highest note/frequency displayed.");
  highNote.onChange = [this]
  { highNoteChanged(); };

  addItemsToNotes();

  addAndMakeVisible(&noteLabels);
  noteLabels.setState(juce::Button::buttonNormal);
  noteLabels.setButtonText("Show Note Labels");
  noteLabels.onClick = [this]
  { noteLabelsClicked(); };
  noteLabels.setTooltip("Select if pressed frequency bins are labeled with corresponding not name. Enable 'Show Frequency Bin Borders' to see the corresponding lines.");

  audioData = NULL;
  setSpectrogramCall();
}

WaveogramUI::~WaveogramUI()
{
  delete audioData;
}

void WaveogramUI::addItemsToNotes()
{
  std::string noteNames[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

  const int numberOfNotesTotal = 132;
  double noteFreqencies[numberOfNotesTotal];
  double baseTuneing = 440.0;
  double factor = std::pow(2.0, 1.0 / 12.0);
  noteFreqencies[0] = 0.03125 * baseTuneing;
  noteFreqencies[12] = 0.0625 * baseTuneing;
  noteFreqencies[24] = 0.125 * baseTuneing;
  noteFreqencies[36] = 0.25 * baseTuneing;
  noteFreqencies[48] = 0.5 * baseTuneing;
  noteFreqencies[60] = baseTuneing;
  noteFreqencies[72] = 2 * baseTuneing;
  noteFreqencies[84] = 4 * baseTuneing;
  noteFreqencies[96] = 8 * baseTuneing;
  noteFreqencies[108] = 16 * baseTuneing;
  noteFreqencies[120] = 32 * baseTuneing;
  for (auto i = 0; i < numberOfNotesTotal; i = i + 12)
  {
    float baseValue = noteFreqencies[i];
    for (auto j = 1; j < 12; j++)
    {
      noteFreqencies[i + j] = baseValue * std::pow(factor, (double)j);
    }
  }
  double noteFreqencyBorders[numberOfNotesTotal - 1];
  for (auto i = 0; i < numberOfNotesTotal - 1; i++)
  {
    noteFreqencyBorders[i] = std::sqrt(noteFreqencies[i] * noteFreqencies[i + 1]);
  }
  for (auto i = 2; i < numberOfNotesTotal - 2; i++)
  {
    lowNote.addItem(noteNames[(i + 1) % 12] + std::to_string((i + -2) / 12) + " / " + std::to_string((int)noteFreqencyBorders[i]) + " Hz", i);
    highNote.addItem(noteNames[(i + 1) % 12] + std::to_string((i + -2) / 12) + " / " + std::to_string((int)noteFreqencyBorders[i + 1]) + " Hz", i + 1);
  }
  lowNote.setSelectedId(6, juce::NotificationType::dontSendNotification);
  for (auto i = 2; i <= 6; i++)
  {
    highNote.setItemEnabled(i, false);
  }
  highNote.setSelectedId(126, juce::NotificationType::dontSendNotification);
  for (auto i = 126; i < numberOfNotesTotal - 2; i++)
  {
    lowNote.setItemEnabled(i, false);
  }
}

void WaveogramUI::lowNoteChanged()
{
  int id = lowNote.getSelectedId();
  for (auto i = 2; i <= id; i++)
  {
    highNote.setItemEnabled(i, false);
  }
  for (auto i = id + 1; i < 130; i++)
  {
    highNote.setItemEnabled(i, true);
  }
}

void WaveogramUI::highNoteChanged()
{
  int id = highNote.getSelectedId();
  for (auto i = 2; i < id; i++)
  {
    lowNote.setItemEnabled(i, true);
  }
  for (auto i = id; i < 130; i++)
  {
    lowNote.setItemEnabled(i, false);
  }
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
  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);
  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);
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
  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);
  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);
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
  const int numberOfNotesTotal = 132;
  double noteFreqencies[numberOfNotesTotal];
  double baseTuneing = 440.0;
  double factor = std::pow(2.0, 1.0 / 12.0);
  noteFreqencies[0] = 0.03125 * baseTuneing;
  noteFreqencies[12] = 0.0625 * baseTuneing;
  noteFreqencies[24] = 0.125 * baseTuneing;
  noteFreqencies[36] = 0.25 * baseTuneing;
  noteFreqencies[48] = 0.5 * baseTuneing;
  noteFreqencies[60] = baseTuneing;
  noteFreqencies[72] = 2 * baseTuneing;
  noteFreqencies[84] = 4 * baseTuneing;
  noteFreqencies[96] = 8 * baseTuneing;
  noteFreqencies[108] = 16 * baseTuneing;
  noteFreqencies[120] = 32 * baseTuneing;
  for (auto i = 0; i < numberOfNotesTotal; i = i + 12)
  {
    float baseValue = noteFreqencies[i];
    for (auto j = 1; j < 12; j++)
    {
      noteFreqencies[i + j] = baseValue * std::pow(factor, (double)j);
    }
  }
  double noteFreqencyBorders[numberOfNotesTotal - 1];
  for (auto i = 0; i < numberOfNotesTotal - 1; i++)
  {
    noteFreqencyBorders[i] = std::sqrt(noteFreqencies[i] * noteFreqencies[i + 1]);
  }
  for (auto i = 0; i < numberOfNotesTotal - 1; i++)
  {
    if (std::abs(noteFreqencyBorders[i] - currentSelection[2]) < 0.1)
    {
      highNote.setSelectedId(i, juce::NotificationType::sendNotification);
    }
    if (std::abs(noteFreqencyBorders[i] - currentSelection[3]) < 0.1)
    {
      lowNote.setSelectedId(i, juce::NotificationType::sendNotification);
    }
  }
  waveData.resetSelection();
  waveData.setZoom(waveData.getZoom() * 0.5);
  calcButtonClicked();
}

void WaveogramUI::noteLabelsClicked()
{
  waveData.setNoteLabels(noteLabels.getToggleState());
  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);
  waveData.setHorizontalLables(false);
  horizontalLables.setToggleState(false, juce::NotificationType::dontSendNotification);
  waveData.redrawImageCall();
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

  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(6, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(126, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(4.0);

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

  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(6, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(126, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(4.0);

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

  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(6, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(126, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(4.0);

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

  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(6, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(126, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(2.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setFrequencygramCall()
{
  notesPerBinInput.setSelectedId(4);
  waveData.setNotesPerBin(getNotesPerBin());

  timeBinInput.setSelectedId(9);
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

  waveData.setNoteLabels(false);
  noteLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(6, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(126, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(4.0);

  waveData.calculateFTTCall();
}

void WaveogramUI::setNotegramCall()
{
  notesPerBinInput.setSelectedId(1);
  waveData.setNotesPerBin(getNotesPerBin());

  timeBinInput.setSelectedId(4);
  waveData.setTimeBinSize(getTimeBinSize());

  waveData.setLoudnessCorrection(false);
  loudnessCorrection.setToggleState(false, juce::NotificationType::dontSendNotification);

  threshhold.setText("-20.0", false);
  waveData.setThreshhold(getThreshhold());

  clip.setText("-6.0", false);
  waveData.setClip(getClip());

  waveData.setHorizontalLines(true);
  horizontalLines.setToggleState(true, juce::NotificationType::dontSendNotification);

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

  waveData.setFrequencyLabels(false);
  frequencyLabels.setToggleState(false, juce::NotificationType::dontSendNotification);

  waveData.setNoteLabels(true);
  noteLabels.setToggleState(true, juce::NotificationType::dontSendNotification);

  lowNote.setSelectedId(26, juce::NotificationType::sendNotification);
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  highNote.setSelectedId(74, juce::NotificationType::sendNotification);
  waveData.setHighNoteIndex(highNote.getSelectedId());
  waveData.setLowNoteIndex(lowNote.getSelectedId());

  waveData.setZoom(4.0);

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
    waveData.setViewerPosition(fileInput->getCurrentTime() - fileInput->getStartTime() - 500.0 * waveData.getZoom() * 0.5 * (getWidth() - 40.0) / fileInput->getSampleRate());
    waveData.setCursorPosition(fileInput->getCurrentTime() - fileInput->getStartTime());
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
    waveData.setStartTime(fileInput->getStartTime());
    waveData.setLowNoteIndex(lowNote.getSelectedId());
    waveData.setHighNoteIndex(highNote.getSelectedId());
    waveData.setLowNoteIndex(lowNote.getSelectedId());

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
  int width = getWidth();
  int height = getHeight();
  g.setColour(juce::Colours::white);
  int ankerPresets = 110;
  g.drawRect(0, ankerPresets - 1, 147, 7 * 22 + 2);
  g.drawText("Presets: ", 2, ankerPresets + 1, 144, 19, juce::Justification::centredLeft, false);
  int ankerLabels = 300;
  g.drawRect(0, ankerLabels - 1, 147, 6 * 22 + 2);
  g.drawText("Label Control: ", 2, ankerLabels + 1, 144, 19, juce::Justification::centredLeft, false);
  int ankerBin = height / 2 - 22;
  g.drawRect(0, ankerBin - 1, 147, 2 * 22 + 2);
  g.drawText("Notes per Bin: ", 2, ankerBin + 1, 144, 19, juce::Justification::centredLeft, false);
  int ankerYAxis = height / 2 + 45;
  g.drawRect(0, ankerYAxis - 1, 147, 2 * 22 + 2);
  g.drawText("y-Axis: ", 2, ankerYAxis + 1, 144, 19, juce::Justification::centredLeft, false);
  int ankerNorm = height / 2 + 120;
  g.drawRect(0, ankerNorm - 1, 147, 3 * 22 + 2);
  g.drawText("Normalization: ", 2, ankerNorm + 1, 144, 19, juce::Justification::centredLeft, false);
  int ankerGlyth = height / 2 + 220;
  g.drawRect(0, ankerGlyth - 1, 147, 3 * 22 + 2);
  g.drawText("Glyth Settings: ", 2, ankerGlyth + 1, 144, 19, juce::Justification::centredLeft, false);
  int loudnessAnker = height - 60 - 38 - 20 - 40 - 22;
  g.drawRect(0, loudnessAnker - 1, 147, 2 * 22 + 2);
  g.drawText("Loudness: ", 2, loudnessAnker + 1, 144, 19, juce::Justification::centredLeft, false);

  g.drawRect(width - 140 - (width - 150) / 10 - 4, 4, (width - 150) / 10 + 8, 46);
  g.drawText("Threshhold: ", width - 140 - (width - 150) / 10 - 2, 6, (width - 150) / 20, 20, juce::Justification::centredLeft, false);
  g.drawText("Clip: ", width - 140 - (width - 150) / 20 + 2, 6, (width - 150) / 20, 20, juce::Justification::centredLeft, false);
  g.drawText(" dB", width - 140 - (width - 150) / 10 - 2 + (width - 150) * 2 / 60, 28, (width - 150) / 60, 20, juce::Justification::centredLeft, false);
  g.drawText(" dB", width - 140 - (width - 150) / 20 + 2 + (width - 150) * 2 / 60, 28, (width - 150) / 60, 20, juce::Justification::centredLeft, false);

  g.drawRect(0.61 * width + 2, 4, 1* (width) / 10 + 4, 46);
  g.drawText("Time Bin Size[s]: ", 0.61 * width + 4, 6, width / 10, 20, juce::Justification::centredLeft, false);

  g.drawRect(0.72 * width + 2, 4, 1* (width) / 10 + 4, 46);
  g.drawText("x-Axis: ", 0.72 * width + 4, 6, width / 10, 20, juce::Justification::centredLeft, false);
}

void WaveogramUI::resized()
{
  int width = getWidth();
  int height = getHeight();

  calcButton.setBounds(width / 2 + 2, 2, 0.1 * width, 45);

  timeBinInput.setBounds(0.61 * width + 4, 28, width / 10, 20);

  scaleHorizontal.setBounds(0.72 * width + 4, 28, width / 10, 20);

  threshhold.setBounds(width - 140 - (width - 150) / 10 - 2, 28, (width - 150) * 2 / 60, 20);
  clip.setBounds(width - 140 - (width - 150) / 20 + 2, 28, (width - 150) * 2 / 60, 20);

  selectionToFocus.setBounds(width - 4 - 120, 28, 118, 20);

  highNote.setBounds(2, 50, 144, 20);

  int ankerPresets = 110;
  setSpectrogram.setBounds(2, ankerPresets + 1 * 22, 144, 20);
  setFrequencygram.setBounds(2, ankerPresets + 2 * 22, 144, 20);
  setHistgram.setBounds(2, ankerPresets + 3 * 22, 144, 20);
  setWavegram.setBounds(2, ankerPresets + 4 * 22, 144, 20);
  setNotegram.setBounds(2, ankerPresets + 5 * 22, 144, 20);
  setWaveform.setBounds(2, ankerPresets + 6 * 22, 144, 20);

  int ankerLabels = 300;
  frequencyLabels.setBounds(2, ankerLabels + 1 * 22, 144, 20);
  noteLabels.setBounds(2, ankerLabels + 2 * 22, 144, 20);
  horizontalLables.setBounds(2, ankerLabels + 3 * 22, 144, 20);
  horizontalLines.setBounds(2, ankerLabels + 4 * 22, 144, 20);
  verticalLables.setBounds(2, ankerLabels + 5 * 22, 144, 20);

  int ankerBin = height / 2 - 22;
  notesPerBinInput.setBounds(2, ankerBin + 1 * 22, 144, 20);

  int ankerYAxis = height / 2 + 45;
  scaleVertical.setBounds(2, ankerYAxis + 1 * 22, 144, 20);

  int ankerNorm = height / 2 + 120;
  normalizeFrequencyDim.setBounds(2, ankerNorm + 1 * 22, 144, 20);
  normalizeTimeDim.setBounds(2, ankerNorm + 2 * 22, 144, 20);

  int ankerGlyth = height / 2 + 220;
  drawEllipse.setBounds(2, ankerGlyth + 1 * 22, 144, 20);
  centered.setBounds(2, ankerGlyth + 2 * 22, 144, 20);

  int loudnessAnker = height - 60 - 38 - 20 - 40 - 22;
  loudnessCorrection.setBounds(2, loudnessAnker + 1 * 22, 144, 20);

  lowNote.setBounds(2, height - 28 - 20, 144, 20);

  waveData.setBounds(150, 50, getWidth() - 150, height - 50);
}