/*
  ==============================================================================

    FileHandler.cpp
    Created: 21 Oct 2022 10:04:54am
    Author:  andreas

  ==============================================================================
*/

#include "FileHandler.h"

FileHandler::FileHandler() : audioTime()
{
  addAndMakeVisible(&openButton);
  openButton.setButtonText("Open");
  openButton.onClick = [this]
  { openButtonClicked(); };

  addAndMakeVisible(&playButton);
  playButton.setButtonText("Play");
  playButton.onClick = [this]
  { playButtonClicked(); };
  playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
  playButton.setEnabled(false);

  addAndMakeVisible(&stopButton);
  stopButton.setButtonText("Stop");
  stopButton.onClick = [this]
  { stopButtonClicked(); };
  stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
  stopButton.setEnabled(false);

  addAndMakeVisible(&fileName);
  fileName.setText("no file opened", juce::dontSendNotification);
  fileName.setColour(juce::Label::textColourId, juce::Colours::white);

  addAndMakeVisible(&time);
  time.setText("Time:", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&seconds1);
  seconds1.setText("s", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&seconds2);
  seconds2.setText("s", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&to);
  to.setText("to", juce::NotificationType::dontSendNotification);

  addAndMakeVisible(&startTime);
  startTime.setText("0.0");
  startTime.setInputRestrictions(6, "0123456789.");
  startTime.setMultiLine(false);
  startTime.addListener(this);

  addAndMakeVisible(&endTime);
  endTime.setText("0.0");
  endTime.setInputRestrictions(6, "0123456789.");
  endTime.setMultiLine(false);
  endTime.addListener(this);

  transportSource.addChangeListener(this);
  formatManager.registerBasicFormats();

  addAndMakeVisible(&audioTime);
}

FileHandler::~FileHandler()
{
}

void FileHandler::changeListenerCallback(juce::ChangeBroadcaster *source)
{
  if (source == &transportSource)
  {
    if (transportSource.isPlaying())
      changeState(Playing);
    else
      changeState(Stopped);
  }
}

void FileHandler::changeState(TransportState newState)
{
  if (state != newState)
  {
    state = newState;

    switch (state)
    {
    case Stopped: // [3]
      stopButton.setEnabled(false);
      playButton.setEnabled(true);
      transportSource.setPosition(getStartTime());
      break;

    case Starting: // [4]
      playButton.setEnabled(false);
      transportSource.start();
      break;

    case Playing: // [5]
      stopButton.setEnabled(true);
      break;

    case Stopping: // [6]
      transportSource.stop();
      break;
    }
  }
}

void FileHandler::textEditorTextChanged(juce::TextEditor &source)
{
  float newStartTime = getStartTime();
  float newEndTime = getEndTime();
  if (newEndTime != endTimeVal || newStartTime != startTimeVal)
  {
    endTimeVal = newEndTime;
    startTimeVal = newEndTime;
    transportSource.setPosition(newStartTime);
    sendChangeMessage();
  }
}

bool FileHandler::isAudioPlaying()
{
  if (fileLoaded.get())
  {
    return transportSource.isPlaying();
  }
  return false;
}

void FileHandler::openButtonClicked()
{
  chooser = std::make_unique<juce::FileChooser>("Select Audio File ...",
                                                juce::File{},
                                                "*.wav");
  auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

  chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc)
                       {
            auto file = fc.getResult();
 
            if (file != juce::File{})                                                      
            {
                auto* reader = formatManager.createReaderFor (file);                 
 
                if (reader != nullptr)
                {
                    fullPath = file.getFullPathName();
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   
                    transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);       
                    playButton.setEnabled (true);
                    fileName.setText(file.getFileName(), juce::dontSendNotification);                                                     
                    readerSource.reset (newSource.release());
                    fileLoaded.set(true);
                    sendChangeMessage();
                    endTime.setText(juce::String(((float)transportSource.getTotalLength() / reader->sampleRate)));                                      
                }
            } });
}

juce::String FileHandler::getPath()
{
  return fullPath;
}

void FileHandler::stopButtonClicked()
{
  changeState(Stopping);
}

void FileHandler::playButtonClicked()
{
  changeState(Starting);
}

void FileHandler::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
  transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void FileHandler::releaseResources()
{
  transportSource.releaseResources();
}

void FileHandler::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
  if (readerSource.get() == nullptr)
  {
    bufferToFill.clearActiveBufferRegion();
    return;
  }
  transportSource.getNextAudioBlock(bufferToFill);
  float currentTime = transportSource.getCurrentPosition();
  audioTime.time = currentTime;
  if (currentTime > getEndTime())
  {
    transportSource.setPosition(getStartTime());
  }
}

void FileHandler::setNextReadPosition(juce::int64 newPosition)
{
  if (readerSource.get() != nullptr)
  {
    transportSource.setNextReadPosition(newPosition);
  }
}

juce::int64 FileHandler::getNextReadPosition() const
{
  if (readerSource.get() != nullptr)
  {
    return transportSource.getNextReadPosition();
  }
  return -1;
}

juce::int64 FileHandler::getTotalLength() const
{
  if (readerSource.get() != nullptr)
  {
    return transportSource.getTotalLength();
  }
  return 0;
}

bool FileHandler::isLooping() const
{
  if (readerSource.get() != nullptr)
  {
    return transportSource.isLooping();
  }
  return false;
}

void FileHandler::setLooping(bool shouldLoop)
{
  if (readerSource.get() != nullptr)
  {
    return transportSource.setLooping(shouldLoop);
  }
}

void FileHandler::getAudioBlock(juce::AudioBuffer<float> *bufferToFill)
{
  if (readerSource != nullptr)
  {
    float startTime = getStartTime();
    juce::int64 startSample = startTime * getSampleRate();
    juce::int64 numSamples = getSegmentLength();
    auto fileReader = readerSource->getAudioFormatReader();
    fileReader->read(bufferToFill, 0, std::min(numSamples, (juce::int64)bufferToFill->getNumSamples()), startSample, true, true);
  }
}

juce::int64 FileHandler::getSegmentLength()
{
  float startTime = getStartTime();
  juce::int64 startSample = startTime * getSampleRate();
  float endTime = getEndTime();
  juce::int64 endSample = endTime * getSampleRate();
  if (endSample > getTotalLength())
  {
    endSample = getTotalLength() - 1;
  }
  return std::max(0ll, endSample - startSample);
}

double FileHandler::getCurrentTime()
{
  return transportSource.getCurrentPosition();
}

double FileHandler::getSampleRate()
{
  if (readerSource != nullptr)
  {
    auto fileReader = readerSource->getAudioFormatReader();
    return fileReader->sampleRate;
  }
  return -1.0;
}

juce::AudioBuffer<float> *FileHandler::getAudio(double from, double to)
{
  if (readerSource != nullptr)
  {
    auto fileReader = readerSource->getAudioFormatReader();
    auto samplerate = fileReader->sampleRate;
    int start = from * samplerate;
    int numSamples = to * samplerate - start;
    if (numSamples > 0)
    {
      auto out = new juce::AudioBuffer<float>(1, numSamples);
      fileReader->read(out, 0, numSamples, start, true, true);
      return out;
    }
  }
  return nullptr;
}

float FileHandler::getStartTime()
{
  return startTime.getText().getFloatValue();
}

float FileHandler::getEndTime()
{
  return endTime.getText().getFloatValue();
}

void FileHandler::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void FileHandler::resized()
{
  int width = getWidth() - 6;
  int heigth = getHeight() - 6;
  openButton.setBounds(2, heigth / 2 + 4, width * 0.75 - 2, heigth / 2);
  playButton.setBounds(width * 0.75 + 2, 2, width * 0.125 - 2, heigth + 2);
  stopButton.setBounds(width * 0.875 + 1, 2, width * 0.125 - 1, heigth + 2);
  fileName.setBounds(width / 2 + 2, 2, width / 8, heigth / 2);
  audioTime.setBounds(5 * width / 8 + 2, 2, width / 8, heigth / 2);

  int size = (width / 2) / 3.25;
  time.setBounds(2, 2, 0.5 * size, heigth / 2);
  startTime.setBounds(2 + 0.5 * size, 2, size, heigth / 2);
  seconds1.setBounds(2 + 1.5 * size, 2, 0.25 * size, heigth / 2);
  to.setBounds(2 + 1.75 * size, 2, 0.25 * size, heigth / 2);
  endTime.setBounds(2 + 2.0 * size, 2, size, heigth / 2);
  seconds2.setBounds(2 + 3.0 * size, 2, 0.25 * size, heigth / 2);
}

TimeField::TimeField()
{
  setFramesPerSecond(10);
}

TimeField::~TimeField()
{
}

void TimeField::paint(juce::Graphics &g)
{
  int width = getWidth();
  int heigth = getHeight();
  g.fillAll(juce::Colours::darkgrey);
  g.setColour(juce::Colours::white);
  g.drawText("T: " + std::to_string(time) + " s", 0, 0, width, heigth, juce::Justification::centred, false);
}

void TimeField::resized()
{
}

void TimeField::update()
{
  repaint();
}
