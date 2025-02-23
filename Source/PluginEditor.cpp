#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DISTROARAudioProcessorEditor::DISTROARAudioProcessorEditor(DISTROARAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Volume Slider
    volumeSlider.setSliderStyle(juce::Slider::Rotary);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(*audioProcessor.volumeParameter);
    volumeSlider.addListener(this);
    addAndMakeVisible(&volumeSlider);

    // Volume Label
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&volumeLabel);

    // Distortion Slider
    distortionSlider.setSliderStyle(juce::Slider::Rotary);
    distortionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionSlider.setRange(0.0, 1.0, 0.01);
    distortionSlider.setValue(audioProcessor.distortionAmount);
    distortionSlider.addListener(this);
    addAndMakeVisible(&distortionSlider);

    // Distortion Label
    distortionLabel.setText("Drive", juce::dontSendNotification);
    distortionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&distortionLabel);

    setSize(400, 300);
}

DISTROARAudioProcessorEditor::~DISTROARAudioProcessorEditor()
{
}

void DISTROARAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DISTROARAudioProcessorEditor::resized()
{
    int sliderWidth = 100;
    int sliderHeight = 100;
    int labelHeight = 20;
    int padding = 10;

    volumeSlider.setBounds(padding, padding, sliderWidth, sliderHeight);
    volumeLabel.setBounds(padding, padding + sliderHeight, sliderWidth, labelHeight);

    distortionSlider.setBounds(padding + sliderWidth + padding, padding, sliderWidth, sliderHeight);
    distortionLabel.setBounds(padding + sliderWidth + padding, padding + sliderHeight, sliderWidth, labelHeight);
}

void DISTROARAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        *audioProcessor.volumeParameter = (float)slider->getValue();
    }
    else if (slider == &distortionSlider)
    {
        audioProcessor.distortionAmount = (float)slider->getValue();
    }
}