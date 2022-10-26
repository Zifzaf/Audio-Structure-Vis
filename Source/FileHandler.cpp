/*
  ==============================================================================

    FileHandler.cpp
    Created: 21 Oct 2022 10:04:54am
    Author:  andreas

  ==============================================================================
*/

#include "FileHandler.h"

FileHandler::FileHandler()
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

  formatManager.registerBasicFormats();
}

FileHandler::~FileHandler()
{
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
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   
                    transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);       
                    playButton.setEnabled (true);
                    fileName.setText(file.getFileName(), juce::dontSendNotification);                                                    
                    readerSource.reset (newSource.release());                                          
                }
            } });
}

void FileHandler::stopButtonClicked()
{
  stopButton.setEnabled(false);
  transportSource.stop();
  playButton.setEnabled(true);
  transportSource.setPosition(0.0);
}

void FileHandler::playButtonClicked()
{
  playButton.setEnabled(false);
  transportSource.start();
  stopButton.setEnabled(true);
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

void FileHandler::getAudioBlock(juce::AudioBuffer<float> *bufferToFill, int startSample, int numSamples, int channelNum)
{
  if (readerSource != nullptr)
  {
    auto fileReader = readerSource->getAudioFormatReader();
    fileReader->read(bufferToFill, channelNum, startSample, numSamples, true, true);
  }
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
      getAudioBlock(out, start, numSamples);
      return out;
    }
  }
  return nullptr;
}

void FileHandler::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void FileHandler::resized()
{
  int width = getWidth() - 30;
  int heigth = getHeight() - 30;
  openButton.setBounds(10, 10, width / 2, heigth / 2);
  playButton.setBounds(10, 20 + heigth / 2, width / 2, heigth / 2);
  stopButton.setBounds(20 + width / 2, 20 + heigth / 2, width / 2, heigth / 2);
  fileName.setBounds(20 + width / 2, 10, width / 2, heigth / 2);
}
