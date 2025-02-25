#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DISTROARAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    DISTROARAudioProcessorEditor(DISTROARAudioProcessor&);
    ~DISTROARAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DISTROARAudioProcessor& audioProcessor;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    juce::Slider distortionSlider;
    juce::Label distortionLabel;

    juce::Slider blendSlider;
    juce::Label blendLabel;

    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DISTROARAudioProcessorEditor)
};