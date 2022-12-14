#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : waveData(&openFile), toolTips()

{
    std::cout << kfr::library_version() << std::endl;

    addAndMakeVisible(&waveData);
    addAndMakeVisible(&openFile);
    waveData.addFileHandler(&openFile);
    openFile.addChangeListener(&waveData);

    setSize(1600, 1000);

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio) && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                          [&](bool granted)
                                          { setAudioChannels(0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    openFile.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    openFile.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    openFile.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    // g.drawImage(spectrogramImage, 10, 130, factor * spectrogramImage.getWidth(), factor * spectrogramImage.getHeight(), 0, 0, spectrogramImage.getWidth(), spectrogramImage.getHeight());
    //  You can add your drawing code here!
}

void MainComponent::resized()
{
    int visWidth = getWidth();
    int visHeight = getHeight();
    int visStartWidth = 0;
    int visStartHeight = 50;
    waveData.setBounds(0, 0, visWidth, visHeight);
    openFile.setBounds(0, 0, getWidth() * 0.5, 50);
}
