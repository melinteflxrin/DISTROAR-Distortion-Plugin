#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class DISTROARAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DISTROARAudioProcessor();
    ~DISTROARAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setEffectEnabled(bool enabled);
    bool effectEnabled;
    double distortionAmount;
    juce::AudioParameterFloat* volumeParameter;
    juce::AudioParameterFloat* blendParameter;
    juce::AudioParameterFloat* driveParameter;
    juce::AudioParameterFloat* toneParameter;
    juce::AudioParameterFloat* gateParameter;
    float currentGainReduction;
    float smoothingFactor;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DISTROARAudioProcessor)
    juce::dsp::LinkwitzRileyFilter<float> lowPassFilter;
    juce::dsp::LinkwitzRileyFilter<float> highPassFilter;
    juce::AudioBuffer<float> lowBandBuffer;
    juce::AudioBuffer<float> midBandBuffer;
    juce::AudioBuffer<float> highBandBuffer;
    juce::dsp::LinkwitzRileyFilter<float> toneLowPassFilter;
    juce::dsp::Compressor<float> preDistortionCompressor;
    juce::dsp::Compressor<float> postDistortionCompressor;
    juce::dsp::Gain<float> inputGain;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelfFilter;
};