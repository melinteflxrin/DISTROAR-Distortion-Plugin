#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
class DISTROARAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    DISTROARAudioProcessorEditor(DISTROARAudioProcessor&);
    ~DISTROARAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DISTROARAudioProcessor& audioProcessor;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    juce::Slider distortionSlider;
    juce::Label distortionLabel;

    juce::Slider blendSlider;
    juce::Label blendLabel;

    // Custom LookAndFeel
    CustomLookAndFeel customLookAndFeel;

    // Background image
    juce::Image backgroundImage;

    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DISTROARAudioProcessorEditor)
};