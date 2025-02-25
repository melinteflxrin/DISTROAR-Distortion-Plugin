#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
DISTROARAudioProcessorEditor::DISTROARAudioProcessorEditor(DISTROARAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Load background image
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::distroarBackground_png, BinaryData::distroarBackground_pngSize);

    // Volume Slider
    volumeSlider.setSliderStyle(juce::Slider::Rotary);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(*audioProcessor.volumeParameter);
    volumeSlider.setLookAndFeel(&customLookAndFeel);
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
    distortionSlider.setLookAndFeel(&customLookAndFeel);
    distortionSlider.addListener(this);
    addAndMakeVisible(&distortionSlider);

    // Distortion Label
    distortionLabel.setText("Drive", juce::dontSendNotification);
    distortionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&distortionLabel);

    // Blend Slider
    blendSlider.setSliderStyle(juce::Slider::Rotary);
    blendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    blendSlider.setRange(0.0, 1.0, 0.01);
    blendSlider.setValue(*audioProcessor.blendParameter);
    blendSlider.setLookAndFeel(&customLookAndFeel);
    blendSlider.addListener(this);
    addAndMakeVisible(&blendSlider);

    // Blend Label
    blendLabel.setText("Blend", juce::dontSendNotification);
    blendLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&blendLabel);

    setSize(300, 450);
}

DISTROARAudioProcessorEditor::~DISTROARAudioProcessorEditor()
{
    volumeSlider.setLookAndFeel(nullptr);
    distortionSlider.setLookAndFeel(nullptr);
    blendSlider.setLookAndFeel(nullptr);
}

void DISTROARAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImageAt(backgroundImage, 0, 0);
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

    blendSlider.setBounds(padding, padding + sliderHeight + labelHeight + padding, sliderWidth, sliderHeight);
    blendLabel.setBounds(padding, padding + 2 * (sliderHeight + labelHeight) + padding, sliderWidth, labelHeight);
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
    else if (slider == &blendSlider)
    {
        *audioProcessor.blendParameter = (float)slider->getValue();
    }
}