#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(20.0, 10000);
    frequencySlider.setValue(440.0);
    frequencySlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 100, 20);
    frequencySlider.setSkewFactorFromMidPoint(500.0);
    frequencySlider.onValueChange = [this]
    {
        if(currentAngle > 0)
            updateAngleDelta();
    };
    
    setSize (800, 600);
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto level = 0.125f;
    auto* pLeftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* pRightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    for(auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        auto currentSample = (float) std::sin(currentAngle);
        currentAngle += angleDelta;
        pLeftBuffer[sample] = currentSample * level;
        pRightBuffer[sample] = currentSample * level;
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    frequencySlider.setBounds (10, 10, getWidth() - 20, 20);
}

void MainComponent::updateAngleDelta()
{
    auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
    angleDelta = cyclesPerSample * 2 * juce::MathConstants<double>::pi;
}
