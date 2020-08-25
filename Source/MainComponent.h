#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    void updateAngleDelta();
    
private:
    //==============================================================================
    // Your private member variables go here...
    double currentSampleRate = 0.0, currentAngle = 0.0, angleDelta = 0.0;
    juce::Slider frequencySlider;
    double currentFrequency = 440.0, targetFrequency = 440.0;
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
