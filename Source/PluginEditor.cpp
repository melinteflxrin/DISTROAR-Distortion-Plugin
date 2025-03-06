#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
DISTROARAudioProcessorEditor::DISTROARAudioProcessorEditor(DISTROARAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), effectEnabled(true) // Initialize effectEnabled
{
    // Load background image
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::distroarBackground_png, BinaryData::distroarBackground_pngSize);

    // Load images for the button states
    buttonOnImage = juce::ImageFileFormat::loadFrom(BinaryData::distroarON_png, BinaryData::distroarON_pngSize);
    buttonOffImage = juce::ImageFileFormat::loadFrom(BinaryData::distroarOFF_png, BinaryData::distroarOFF_pngSize);

    // Set initial images to the toggleButton based on effectEnabled state
    toggleButton.setImages(false, true, true,
        effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack,
        effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack,
        effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack);

    addAndMakeVisible(toggleButton);
    toggleButton.addListener(this);

    // Set initial bounds for the toggleButton
    toggleButton.setBounds(120, 240, 100, 100);

    // Volume Slider
    volumeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(*audioProcessor.volumeParameter);
    volumeSlider.setLookAndFeel(&customLookAndFeel);
    volumeSlider.addListener(this);
    volumeSlider.addMouseListener(this, false);
    volumeSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    volumeSlider.setMouseDragSensitivity(300); // Increase sensitivity
    addAndMakeVisible(&volumeSlider);

    // Volume Label
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&volumeLabel);

    // Distortion Slider
    distortionSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    distortionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionSlider.setRange(0.0, 1.0, 0.01);
    distortionSlider.setValue(*audioProcessor.driveParameter);
    distortionSlider.setLookAndFeel(&customLookAndFeel);
    distortionSlider.addListener(this);
    distortionSlider.addMouseListener(this, false);
    distortionSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    distortionSlider.setMouseDragSensitivity(300);
    addAndMakeVisible(&distortionSlider);

    // Distortion Label
    distortionLabel.setText("Drive", juce::dontSendNotification);
    distortionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&distortionLabel);

    // Blend Slider
    blendSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    blendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    blendSlider.setRange(0.0, 1.0, 0.01);
    blendSlider.setValue(*audioProcessor.blendParameter);
    blendSlider.setLookAndFeel(&customLookAndFeel);
    blendSlider.addListener(this);
    blendSlider.addMouseListener(this, false);
    blendSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    blendSlider.setMouseDragSensitivity(300);
    addAndMakeVisible(&blendSlider);

    // Blend Label
    blendLabel.setText("Blend", juce::dontSendNotification);
    blendLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&blendLabel);

    // Tone Slider
    toneSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    toneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    toneSlider.setRange(600.0, 20000.0, 25.0);
    toneSlider.setValue(*audioProcessor.toneParameter);
    toneSlider.setLookAndFeel(&customLookAndFeel);
    toneSlider.addListener(this);
    toneSlider.addMouseListener(this, false);
    toneSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    toneSlider.setMouseDragSensitivity(300);
    addAndMakeVisible(&toneSlider);

    // Tone Label
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&toneLabel);

    // Gate Slider
    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setRange(-90.0, 0.0, 0.1);
    gateSlider.setValue(*audioProcessor.gateParameter);
    gateSlider.setLookAndFeel(&customLookAndFeel);
    gateSlider.addListener(this);
    gateSlider.addMouseListener(this, false);
    gateSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    gateSlider.setMouseDragSensitivity(300);
    addAndMakeVisible(&gateSlider);

    // Gate Label
    gateLabel.setText("Gate", juce::dontSendNotification);
    gateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&gateLabel);

    setSize(300, 450);
}

DISTROARAudioProcessorEditor::~DISTROARAudioProcessorEditor()
{
    volumeSlider.setLookAndFeel(nullptr);
    distortionSlider.setLookAndFeel(nullptr);
    blendSlider.setLookAndFeel(nullptr);
    toneSlider.setLookAndFeel(nullptr);
    toggleButton.removeListener(this);
}

void DISTROARAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImageAt(backgroundImage, 0, 0);
}

void DISTROARAudioProcessorEditor::resized()
{
    int bigKnobSize = 120;  
    int smallKnobSize = 80; 
    
    // Big Knobs (Volume & Drive)
    volumeSlider.setBounds(10, 20, bigKnobSize, bigKnobSize);
    ////volumeLabel.setBounds(centerX - bigKnobSpacing - bigKnobSize / 2, topY + bigKnobSize, bigKnobSize, labelHeight);

    distortionSlider.setBounds(300-10-bigKnobSize, 20, bigKnobSize, bigKnobSize);
    //distortionLabel.setBounds(centerX + bigKnobSpacing - bigKnobSize / 2, topY + bigKnobSize, bigKnobSize, labelHeight);


    // Small Knobs (Gate, Blend & Tone)
    gateSlider.setBounds(20, 160, smallKnobSize, smallKnobSize);
    //gateLabel.setBounds(centerX - smallKnobSpacing, bottomY + smallKnobSize, smallKnobSize, labelHeight);

    blendSlider.setBounds(110, 160, smallKnobSize, smallKnobSize);
    //blendLabel.setBounds(centerX - smallKnobSize / 2, bottomY + smallKnobSize, smallKnobSize, labelHeight);

    toneSlider.setBounds(200, 160, smallKnobSize, smallKnobSize);
    //toneLabel.setBounds(centerX + smallKnobSpacing - smallKnobSize, bottomY + smallKnobSize, smallKnobSize, labelHeight);


    //On/Off Switch
    toggleButton.setBounds(100, 450-20-100, 100, 100);
}




void DISTROARAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        *audioProcessor.volumeParameter = (float)slider->getValue();
    }
    else if (slider == &distortionSlider)
    {
        *audioProcessor.driveParameter = (float)slider->getValue();
    }
    else if (slider == &blendSlider)
    {
        *audioProcessor.blendParameter = (float)slider->getValue();
    }
    else if (slider == &toneSlider)
    {
        *audioProcessor.toneParameter = (float)slider->getValue();
    }
    else if (slider == &gateSlider)
    {
        *audioProcessor.gateParameter = (float)slider->getValue();
    }
}

void DISTROARAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &toggleButton)
    {
        // Toggle the effect state
        effectEnabled = !effectEnabled;

        // Update the button images based on the effect state
        toggleButton.setImages(false, true, true,
            effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack,
            effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack,
            effectEnabled ? buttonOnImage : buttonOffImage, 1.0f, juce::Colours::transparentBlack);

        audioProcessor.setEffectEnabled(effectEnabled);
    }
}

void DISTROARAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &volumeSlider || event.eventComponent == &distortionSlider || event.eventComponent == &blendSlider || event.eventComponent == &toneSlider || event.eventComponent == &gateSlider)
    {
        // Store the initial mouse position
        initialMousePosition = event.getScreenPosition();

        // Change the mouse cursor to a blank cursor to lock the mouse in place
        juce::MouseCursor::showWaitCursor();
        // Lock the mouse position
        juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(true);
    }
}

void DISTROARAudioProcessorEditor::mouseUp(const juce::MouseEvent& event)
{
    if (event.eventComponent == &volumeSlider || event.eventComponent == &distortionSlider || event.eventComponent == &blendSlider || event.eventComponent == &toneSlider || event.eventComponent == &gateSlider)
    {
        // Restore the default mouse cursor
        juce::MouseCursor::hideWaitCursor();
        // Unlock the mouse position
        juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(false);

        // Restore the initial mouse position
        juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(initialMousePosition.toFloat());
    }
}