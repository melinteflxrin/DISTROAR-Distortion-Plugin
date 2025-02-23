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

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;

    DISTROARAudioProcessor& audioProcessor;

    juce::Slider volumeSlider;
    juce::Slider distortionSlider;
    juce::Label volumeLabel;
    juce::Label distortionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DISTROARAudioProcessorEditor)
};