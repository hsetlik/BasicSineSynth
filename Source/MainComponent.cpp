#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(20.0, 10000); //sets the freq. range from 20Hz to 10k Hz
    frequencySlider.setValue(440.0); //initializes the synth with a freq of A440
    frequencySlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 100, 20);
    frequencySlider.setSkewFactorFromMidPoint(500.0); //sets up logarithmic response for slider
    frequencySlider.onValueChange = [this]
    {
        if(currentAngle > 0)
            updateAngleDelta();//angleDelta determines pitch, so the slider callback sets the angleDelta based on the slider's new value
    };
    
    setSize (400, 200);
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
    //gets pointer to the first element of the array which stores the samples for the left buffer
    auto* pRightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    //same for the right buffer
    for(auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        //the currentSample float sets its value based the stl function which returns the value of sin(currentAngle)
        auto currentSample = (float) std::sin(currentAngle);
        /*the currentAngle is incremented based on the frequency controller angleDelta
         where the value of a given sample is f_n:
         f_n = sin((f_n - 1) + angleDelta)-- the constant angleDelta determines the phase difference between two points spaced equally in time (the frequency)
         */
        currentAngle += angleDelta;
        pLeftBuffer[sample] = currentSample * level; //sending the sample to both channel buffers
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
    //determines how many (fractions of) cycles of the sin(freqSlider.getValue()) wave are completed in each sample
    auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
    //angleDelta is equal to the change in phase angle over the period of 1 sample 
    angleDelta = cyclesPerSample * 2 * juce::MathConstants<double>::pi;
}
