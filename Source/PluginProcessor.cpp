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
    addParameter(blendParameter = new juce::AudioParameterFloat("blend", "Blend", 0.0f, 1.0f, 0.5f));
    addParameter(driveParameter = new juce::AudioParameterFloat("drive", "Drive", 0.0f, 1.0f, 0.5f));
    addParameter(toneParameter = new juce::AudioParameterFloat("tone", "Tone", 600.0f, 20000.0f, 10300.0f));
    addParameter(gateParameter = new juce::AudioParameterFloat("gate", "Gate", -90.0f, 0.0f, -80.0f));

    // Initialize crossover filters
    lowPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

	currentGainReduction = 1.0f;
	smoothingFactor = 0.005f;
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

    // Prepare tone control low pass filter
    toneLowPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    toneLowPassFilter.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    toneLowPassFilter.reset(); // Reset the filter to clear any previous state

    // Initialize pre-distortion compressor
    preDistortionCompressor.setThreshold(-20.0f); // Threshold in dB
    preDistortionCompressor.setRatio(2.0f); // Ratio
    preDistortionCompressor.setAttack(10.0f); // Attack time in ms
    preDistortionCompressor.setRelease(100.0f); // Release time in ms

    // Initialize post-distortion compressor
    postDistortionCompressor.setThreshold(-10.0f); // Threshold in dB
    postDistortionCompressor.setRatio(12.0f); // Ratio
    postDistortionCompressor.setAttack(10.0f); // Attack time in ms
    postDistortionCompressor.setRelease(80.0f); // Release time in ms

    // Prepare compressors
    preDistortionCompressor.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    postDistortionCompressor.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });

    // Initialize input gain
    inputGain.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    inputGain.setGainDecibels(15.0f); // Apply a fixed gain boost

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

void DISTROARAudioProcessor::setEffectEnabled(bool enabled)
{
    effectEnabled = enabled;
}

void DISTROARAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (effectEnabled) {
        // Apply input gain boost
        juce::dsp::AudioBlock<float> gainBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> gainContext(gainBlock);
        inputGain.process(gainContext);

        // Apply gate effect before distortion
        float gateThreshold = juce::Decibels::decibelsToGain(gateParameter->get());
        float targetGainReduction = 0.0f;
        float attackTime = 0.01f; // Attack time in seconds
        float releaseTime = 0.1f; // Release time in seconds
        float attackCoeff = std::exp(-1.0f / (attackTime * getSampleRate()));
        float releaseCoeff = std::exp(-1.0f / (releaseTime * getSampleRate()));

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float absSample = std::abs(channelData[sample]);
                if (absSample < gateThreshold)
                    currentGainReduction = attackCoeff * currentGainReduction + (1.0f - attackCoeff) * targetGainReduction;
                else
                    currentGainReduction = releaseCoeff * currentGainReduction + (1.0f - releaseCoeff) * 1.0f;

                channelData[sample] *= currentGainReduction;
            }
        }

        // Apply pre-distortion compression
        juce::dsp::AudioBlock<float> preCompBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> preCompContext(preCompBlock);
        preDistortionCompressor.process(preCompContext);

        // Store the signal after pre-distortion compression
        juce::AudioBuffer<float> preDistortionCompressedBuffer;
        preDistortionCompressedBuffer.makeCopyOf(buffer);

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

        // Apply different distortion algorithms to each band 
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* lowBandData = lowBandBuffer.getWritePointer(channel);
            auto* midBandData = midBandBuffer.getWritePointer(channel);
            auto* highBandData = highBandBuffer.getWritePointer(channel);
            auto* originalData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float drive = *driveParameter * 7.0f;
                float inputSample = originalData[sample];

                // Adaptive Gain Compensation for Sustain
                float inputGainComp = 1.2f + (0.6f / (0.25f + std::abs(inputSample)));
                float adaptiveDrive = drive * inputGainComp;

                // Low Band
                float lowSample = lowBandData[sample] * (1.0f + adaptiveDrive * 0.9f);
                lowSample = std::tanh(lowSample * 5.0f); // Stronger saturation for punch
                lowSample *= 1.3f;
                lowSample = juce::jlimit<float>(-0.75f, 0.75f, lowSample); // Control excessive boom

                // Mid Band
                float midSample = midBandData[sample] * (1.0f + adaptiveDrive * 1.15f);
                midSample = midSample / (1.0f + std::abs(midSample) * 0.5f); // Softer limiting
                midSample = std::tanh(midSample * 4.8f); // Increased saturation for bite
                midSample += 0.1f * std::sin(midSample * 3.0f); // Even harmonics
                midSample *= 1.1f;

                // High Band
                float highSample = highBandData[sample] * (1.0f + adaptiveDrive * 0.9f);
                highSample = juce::jlimit<float>(-0.35f, 0.35f, highSample); // Keep highs controlled
                highSample = highSample * 0.85f + std::atan(highSample * 1.8f); // high-end roll-off
                highSample = (highSample * 0.65f) + (originalData[sample] * 0.35f); // Retain clarity

                // Cabinet Simulation: warm
                float cabSim = (lowSample * 1.0f) + (midSample * 1.1f) + (highSample * 0.85f);
                cabSim = juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 4000)
                    ->getMagnitudeForFrequency(4000, 44100) * cabSim; // Lowered LPF for warmth
                cabSim = std::tanh(cabSim * 1.25f); // Stronger final saturation for sustain

                // Assign modified samples back
                lowBandData[sample] = lowSample;
                midBandData[sample] = midSample;
                highBandData[sample] = highSample;
                originalData[sample] = (cabSim * 0.92f) + (originalData[sample] * 0.08f); //dry blend
            }
        }

        // Recombine the bands into the final output
        buffer.makeCopyOf(lowBandBuffer);
        buffer.addFrom(0, 0, midBandBuffer, 0, 0, buffer.getNumSamples());
        buffer.addFrom(1, 0, midBandBuffer, 1, 0, buffer.getNumSamples());
        buffer.addFrom(0, 0, highBandBuffer, 0, 0, buffer.getNumSamples());
        buffer.addFrom(1, 0, highBandBuffer, 1, 0, buffer.getNumSamples());

        // Mix the pre-distortion compressed signal and distorted signals based on the blend parameter
        float blend = blendParameter->get();
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* preCompData = preDistortionCompressedBuffer.getReadPointer(channel);
            auto* distortedData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                distortedData[sample] = (1.0f - blend) * preCompData[sample] + blend * distortedData[sample];
            }
        }

        // Apply tone control using low pass filter
        float toneFrequency = *toneParameter;
        toneLowPassFilter.setCutoffFrequency(toneFrequency);

        juce::dsp::AudioBlock<float> bufferBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> toneContext(bufferBlock);
        toneLowPassFilter.process(toneContext);

        // Apply post-distortion compression
        juce::dsp::ProcessContextReplacing<float> postCompContext(bufferBlock);
        postDistortionCompressor.process(postCompContext);

        // Apply gate effect after distortion
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float absSample = std::abs(channelData[sample]);
                if (absSample < gateThreshold)
                    currentGainReduction = attackCoeff * currentGainReduction + (1.0f - attackCoeff) * targetGainReduction;
                else
                    currentGainReduction = releaseCoeff * currentGainReduction + (1.0f - releaseCoeff) * 1.0f;

                channelData[sample] *= currentGainReduction;
            }
        }

        // Apply volume control
        float volume = *volumeParameter;
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.applyGain(channel, 0, buffer.getNumSamples(), volume);
        }
    }
    else {
        // Bypass the effect, just pass the clean signal
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