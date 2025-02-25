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

        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.saveState();
        g.addTransform(juce::AffineTransform::rotation(angle, centerX, centerY));
        g.drawImage(knobImage, rx, ry, rw, rw, 0, 0, knobImage.getWidth(), knobImage.getHeight());
        g.restoreState();
    }

private:
    juce::Image knobImage;
};