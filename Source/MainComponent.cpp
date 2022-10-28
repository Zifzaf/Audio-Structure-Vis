#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : notes(), energy()

{
    std::cout << kfr::library_version() << std::endl;


    addAndMakeVisible(&openFile);

    addAndMakeVisible(&notes);

    notes.addFileHandler(&openFile);
    openFile.addChangeListener(&notes);

    for (auto i = 0; i < numVis; i++)
    {
        addAndMakeVisible(&visSelect[i]);
        visSelect[i].setButtonText(visNames[i]);
        visSelect[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        visSelect[i].onClick = [this, i]
        { loadVis(visNames[i]); };
    }

    setSize(1600, 1000);

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio) && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                          [&](bool granted)
                                          { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
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
    if (openFile.isAudioPlaying())
    {
        openFile.getNextAudioBlock(bufferToFill);

        if (bufferToFill.buffer->getNumChannels() > 0)
        {
            // visualize(bufferToFill.buffer->getArrayOfReadPointers()[0], bufferToFill.startSample, bufferToFill.numSamples);
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::loadVis(std::string visName)
{
    if (visName == "Note Shower")
    {
        activeVis = VisNoteShower;
    }
    else if (visName == "Energy Bands")
    {
        activeVis = VisEnergyBand;
    }
    else
    {
        activeVis = None;
    }
    resized();
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
    openFile.setBounds(0, 0, getWidth(), 50);
    int visWidth = getWidth();
    int visHeight = getHeight() - 50;
    int visStartWidth = 0;
    int visStartHeight = 50;
    switch (activeVis)
    {
    case None:
    {
        int buttonHeight = (visHeight - (numVis + 1) * 2) / numVis;
        for (auto i = 0; i < numVis; i++)
        {
            visSelect[i].setBounds(visStartWidth + 2, visStartHeight + i * buttonHeight + (i + 1) * 2, visWidth - 4, buttonHeight);
        }
        break;
    }

    case VisNoteShower:
    {
        for (auto i = 0; i < numVis; i++)
        {
            visSelect[i].setBounds(0, 0, 0, 0);
        }
        notes.setBounds(visStartWidth, visStartHeight, visWidth, visHeight);
        break;
    }

    case VisEnergyBand:
    {
        for (auto i = 0; i < numVis; i++)
        {
            visSelect[i].setBounds(0, 0, 0, 0);
        }
        energy.setBounds(visStartWidth, visStartHeight, visWidth, visHeight);
        break;
    }
    }
}
