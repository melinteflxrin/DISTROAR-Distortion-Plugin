#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DISTROARAudioProcessorEditor::DISTROARAudioProcessorEditor(DISTROARAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Add and configure the distortion knob
    distortionKnob.setSliderStyle(juce::Slider::Rotary);
    distortionKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    distortionKnob.setRange(0.0, 10.0, 0.01);
    distortionKnob.setValue(audioProcessor.distortionAmount); // Set knob to default value
    distortionKnob.addListener(this);
    addAndMakeVisible(&distortionKnob);

    setSize(400, 300);
}

DISTROARAudioProcessorEditor::~DISTROARAudioProcessorEditor()
{
}

void DISTROARAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Distortion Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void DISTROARAudioProcessorEditor::resized()
{
    distortionKnob.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 50, 100, 100);
}

void DISTROARAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &distortionKnob)
    {
        audioProcessor.distortionAmount = distortionKnob.getValue();
    }
}