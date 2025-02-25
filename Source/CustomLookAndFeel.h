#pragma once

#include <JuceHeader.h>
#include "BinaryData.h" // Include the BinaryData header

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        knobImage = juce::ImageCache::getFromMemory(BinaryData::distroarKnob_png, BinaryData::distroarKnob_pngSize);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        const float radius = juce::jmin(width / 2.0f, height / 2.0f) - 4.0f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius;
        const float ry = centerY - radius;
        const float rw = radius * 2.0f;

        g.drawImage(knobImage, rx, ry, rw, rw, 0, 0, knobImage.getWidth(), knobImage.getHeight());

        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        juce::Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));

        g.setColour(juce::Colours::black);
        g.fillPath(p);
    }

private:
    juce::Image knobImage;
};