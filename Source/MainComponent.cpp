#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : spectro(numLevles), dft(fftSize), notes()

{
    std::cout << kfr::library_version() << std::endl;

    temp = new kfr::univector<kfr::u8>(dft.temp_size);

    addAndMakeVisible(&openFile);

    addAndMakeVisible(&notes);
    
    notes.addFileHandler(&openFile);
    openFile.addChangeListener(&notes);

    setSize(1600, 900);

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
            visualize(bufferToFill.buffer->getArrayOfReadPointers()[0], bufferToFill.startSample, bufferToFill.numSamples);
        }
    }else{
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::visualize(const float *buffer, int startSample, int numSamples)
{
    if (inBufferIndex + numSamples < fftSize)
    {
        for (auto i = startSample; i < numSamples + startSample; ++i)
        {
            inBuffer[inBufferIndex++] = buffer[i];
        }
    }
    else
    {
        auto samplesMissing = fftSize - inBufferIndex;
        for (auto i = 0; i < samplesMissing; ++i)
        {
            inBuffer[inBufferIndex + i] = buffer[startSample + i];
        }

        dft.execute(outBuffer, inBuffer, *(temp));

        auto levelBuffer = new float[numLevles];
        int level = 0;
        int next = 2;
        float div = fftSize;
        for (auto i = 0; i < fftSize / 2; ++i)
        {
            if (i == next)
            {
                level++;
                next *= 2;
                levelBuffer[level] = 0.0;
                levelBuffer[level] += cabs(outBuffer[i] / div);
            }
            else
            {
                levelBuffer[level] += cabs(outBuffer[i] / div);
            }
        }
        // std::cout << numLevles << "  " << level << std::endl;
        for (auto i = 0; i < numLevles; ++i)
        {
            levelBuffer[i] = 20 * std::log10(levelBuffer[i] + 1.0);
        }
        spectro.addDataLine(levelBuffer, false);

        delete levelBuffer;

        inBufferIndex = 0;
        for (auto i = 0; i < numSamples - samplesMissing; ++i)
        {
            inBuffer[inBufferIndex++] = buffer[startSample + samplesMissing + i];
        }
    }
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
    openFile.setBounds(0, 0, getWidth(), 100);
    notes.setBounds(0, 100, getWidth(), getHeight() - 100);
}
