#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(20.0, 10000); //sets the freq. range from 20Hz to 10k Hz
    frequencySlider.setValue(currentFrequency); //initializes the synth with a freq of A440
    frequencySlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 100, 20);
    frequencySlider.setSkewFactorFromMidPoint(500.0); //sets up logarithmic response for slider
    frequencySlider.onValueChange = [this]
    {
        if(currentAngle > 0)
            targetFrequency = frequencySlider.getValue();
    };
    
    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0.0f, .175f);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 100, 20);
    gainSlider.setSkewFactorFromMidPoint(0.0625);
    gainSlider.setValue(0.0625);
    gainSlider.onValueChange = [this]
    {
        updateCurrentGain();
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
    auto level = currentGain;
    auto* pLeftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    //gets pointer to the first element of the array which stores the samples for the left buffer
    auto* pRightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    //same for the right buffer
    auto localTargetFreq = targetFrequency;
    if(currentFrequency != localTargetFreq)
    {
    // with this code, the frequency just drifts toward that of the target over the course of one buffer length
    auto freqIncrement = (localTargetFreq - currentFrequency) / bufferToFill.numSamples;
    for(auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
        //the currentSample float sets its value based the stl function which returns the value of sin(currentAngle)
        auto currentSample = (float) std::sin(currentAngle);
        currentFrequency += freqIncrement;
        /*the currentAngle is incremented based on the frequency controller angleDelta
         where the value of a given sample is f_n:
         f_n = sin((f_n - 1) + angleDelta)-- the constant angleDelta determines the phase difference between two points spaced equally in time (the frequency)
         */
        updateAngleDelta();
        currentAngle += angleDelta;
        pLeftBuffer[sample] = currentSample * level; //sending the sample to both channel buffers
        pRightBuffer[sample] = currentSample * level;
        }
    currentFrequency = localTargetFreq;
    } else
    {
        for(auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            auto currentSample = (float) std::sin(currentAngle);
            currentAngle += angleDelta;
            pLeftBuffer[sample] = currentSample * level; //sending the sample to both channel buffers
            pRightBuffer[sample] = currentSample * level;
        }
        
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
    gainSlider.setBounds(10, 40, getWidth() - 20, 20);
}

void MainComponent::updateAngleDelta()
{
    //determines how many (fractions of) cycles of the sin(freqSlider.getValue()) wave are completed in each sample
    auto cyclesPerSample = currentFrequency / currentSampleRate;
    //angleDelta is equal to the change in phase angle over the period of 1 sample 
    angleDelta = cyclesPerSample * 2 * juce::MathConstants<double>::pi;
}


void MainComponent::updateCurrentGain()
{
    currentGain = gainSlider.getValue();
}
