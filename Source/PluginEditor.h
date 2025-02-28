#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

class DISTROARAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::MouseListener, private juce::Button::Listener
{
public:
    DISTROARAudioProcessorEditor(DISTROARAudioProcessor&);
    ~DISTROARAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void buttonClicked(juce::Button* button) override;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider distortionSlider;
    juce::Label distortionLabel;
    juce::Slider blendSlider;
    juce::Label blendLabel;

    juce::TextButton toggleButton;
    bool effectEnabled;

    CustomLookAndFeel customLookAndFeel;
    juce::Image backgroundImage;
    juce::Point<int> initialMousePosition;

    DISTROARAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DISTROARAudioProcessorEditor)
};