/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DISTROARAudioProcessor::DISTROARAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    addParameter(volumeParameter = new juce::AudioParameterFloat("volume", "Volume", 0.0f, 1.0f, 0.5f));
    distortionAmount = 1.0; // Initialize distortion amount

    // Initialize crossover filters
    lowPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

DISTROARAudioProcessor::~DISTROARAudioProcessor()
{
}

//==============================================================================
const juce::String DISTROARAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DISTROARAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DISTROARAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DISTROARAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DISTROARAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DISTROARAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DISTROARAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DISTROARAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String DISTROARAudioProcessor::getProgramName(int index)
{
    return {};
}

void DISTROARAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void DISTROARAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // Prepare crossover filters
    lowPassFilter.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    highPassFilter.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });

    // Set crossover frequencies
    lowPassFilter.setCutoffFrequency(200.0f); // Low band cutoff frequency
    highPassFilter.setCutoffFrequency(2000.0f); // High band cutoff frequency

    // Prepare buffers for each band
    lowBandBuffer.setSize(2, samplesPerBlock);
    midBandBuffer.setSize(2, samplesPerBlock);
    highBandBuffer.setSize(2, samplesPerBlock);
}

void DISTROARAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DISTROARAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void DISTROARAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Split the input into three bands
    lowBandBuffer.makeCopyOf(buffer);
    highBandBuffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> lowBlock(lowBandBuffer);
    juce::dsp::AudioBlock<float> highBlock(highBandBuffer);

    juce::dsp::ProcessContextReplacing<float> lowContext(lowBlock);
    juce::dsp::ProcessContextReplacing<float> highContext(highBlock);

    lowPassFilter.process(lowContext);
    highPassFilter.process(highContext);

    midBandBuffer.makeCopyOf(buffer);
    midBandBuffer.addFrom(0, 0, lowBandBuffer, 0, 0, buffer.getNumSamples(), -1.0f);
    midBandBuffer.addFrom(1, 0, lowBandBuffer, 1, 0, buffer.getNumSamples(), -1.0f);
    midBandBuffer.addFrom(0, 0, highBandBuffer, 0, 0, buffer.getNumSamples(), -1.0f);
    midBandBuffer.addFrom(1, 0, highBandBuffer, 1, 0, buffer.getNumSamples(), -1.0f);

    // Apply different distortion algorithms to each band with enhanced low-end and smoother highs
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* lowBandData = lowBandBuffer.getWritePointer(channel);
        auto* midBandData = midBandBuffer.getWritePointer(channel);
        auto* highBandData = highBandBuffer.getWritePointer(channel);
        auto* originalData = buffer.getWritePointer(channel); // For clean low-end blend

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float drive = distortionAmount * 7.0f; // Increase drive for stronger distortion

            // === Low Band: Heavy Saturation + Low-end Weight ===
            float lowSample = lowBandData[sample] * (1.0f + drive * 0.8f);
            lowSample = std::tanh(lowSample * 4.0f); // Stronger saturation for thickness
            float cleanLow = originalData[sample] * 0.2f; // Parallel clean blend
            lowSample = (lowSample * 0.8f) + cleanLow;
            lowSample *= 1.2f; // Boost low-end power

            // === Mid Band: Asymmetric Tube Saturation ===
            float midSample = midBandData[sample] * (1.0f + drive);
            midSample = (midSample >= 0.0f) ?
                midSample / (1.0f + std::abs(midSample)) :
                midSample / (1.0f + std::abs(midSample) * 0.5f);
            midSample = std::tanh(midSample * 4.5f); // Richer saturation

            // === High Band: Softer Attack + Presence Smoothness ===
            float highSample = highBandData[sample] * (1.0f + drive * 1.1f);
            highSample = juce::jlimit<float>(-0.7f, 0.7f, highSample); // Less aggressive clipping
            highSample = highSample * 1.1f + std::sin(highSample * 2.0f); // Smoother bite
            highSample = (highSample * 0.8f) + (originalData[sample] * 0.2f); // Subtle blend for attack smoothing

            // === Simple Cabinet Simulation ===
            float cabSim = (lowSample * 0.65f) + (midSample * 1.0f) + (highSample * 0.75f);
            cabSim = juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 5500)->getMagnitudeForFrequency(5500, 44100) * cabSim;

            // Assign modified samples back
            lowBandData[sample] = lowSample;
            midBandData[sample] = midSample;
            highBandData[sample] = highSample;

            // Final Output
            originalData[sample] = (cabSim * 0.92f) + (originalData[sample] * 0.08f); // Subtle dry blend
        }
    }









    // Recombine the bands into the final output
    buffer.makeCopyOf(lowBandBuffer);
    buffer.addFrom(0, 0, midBandBuffer, 0, 0, buffer.getNumSamples());
    buffer.addFrom(1, 0, midBandBuffer, 1, 0, buffer.getNumSamples());
    buffer.addFrom(0, 0, highBandBuffer, 0, 0, buffer.getNumSamples());
    buffer.addFrom(1, 0, highBandBuffer, 1, 0, buffer.getNumSamples());

    // Apply volume control
    float volume = *volumeParameter;
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), volume);
    }
}

//==============================================================================
bool DISTROARAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DISTROARAudioProcessor::createEditor()
{
    return new DISTROARAudioProcessorEditor(*this);
}

//==============================================================================
void DISTROARAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DISTROARAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DISTROARAudioProcessor();
}