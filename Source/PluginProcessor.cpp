#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

AutoTuneAudioProcessor::AutoTuneAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, juce::Identifier("AutoTuneParameters"), pluginParameters.createParameterLayout()),
    pluginParameters(),
    presetManager(parameters),
    pitchEngine(),
    modeSelector(),
    aiModelLoader()
{
    // Add parameter listeners
    parameters.addParameterListener(Parameters::SPEED_ID, this);
    parameters.addParameterListener(Parameters::AMOUNT_ID, this);
    parameters.addParameterListener(Parameters::MODE_ID, this);
    parameters.addParameterListener(Parameters::KEY_ID, this);
    parameters.addParameterListener(Parameters::SCALE_ID, this);

    // Initialize FFT
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::hann);
    frequencyData.allocate(fftSize, true);

    // Initialize smoothed values
    speedSmoothed.reset(44100.0, 0.05); // 50ms smoothing time
    amountSmoothed.reset(44100.0, 0.05);
}

AutoTuneAudioProcessor::~AutoTuneAudioProcessor()
{
    parameters.removeParameterListener(Parameters::SPEED_ID, this);
    parameters.removeParameterListener(Parameters::AMOUNT_ID, this);
    parameters.removeParameterListener(Parameters::MODE_ID, this);
    parameters.removeParameterListener(Parameters::KEY_ID, this);
    parameters.removeParameterListener(Parameters::SCALE_ID, this);
}

const juce::String AutoTuneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoTuneAudioProcessor::acceptsMidi() const
{
    return false;
}

bool AutoTuneAudioProcessor::producesMidi() const
{
    return false;
}

bool AutoTuneAudioProcessor::isMidiEffect() const
{
    return false;
}

double AutoTuneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutoTuneAudioProcessor::getNumPrograms()
{
    return 1;
}

int AutoTuneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoTuneAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AutoTuneAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AutoTuneAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void AutoTuneAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Prepare pitch correction engine
    pitchEngine.prepare(sampleRate, samplesPerBlock);

    // Initialize buffers
    pitchBuffer.setSize(2, samplesPerBlock);
    correctedBuffer.setSize(2, samplesPerBlock);
    overlapBuffer.setSize(2, overlapSize);
    fftBuffer.setSize(1, fftSize);

    overlapBuffer.clear();
    overlapPosition = 0;

    // Initialize smoothed values
    speedSmoothed.reset(sampleRate, 0.05);
    amountSmoothed.reset(sampleRate, 0.05);
    
    speedSmoothed.setCurrentAndTargetValue(*parameters.getRawParameterValue(Parameters::SPEED_ID));
    amountSmoothed.setCurrentAndTargetValue(*parameters.getRawParameterValue(Parameters::AMOUNT_ID));

#ifdef USE_RUBBERBAND
    // Initialize Rubber Band stretcher
    rubberBand = std::make_unique<RubberBand::RubberBandStretcher>(
        sampleRate, 2,
        RubberBand::RubberBandStretcher::OptionProcessRealTime |
        RubberBand::RubberBandStretcher::OptionPitchHighQuality
    );
#endif
}

void AutoTuneAudioProcessor::releaseResources()
{
    pitchBuffer.setSize(0, 0);
    correctedBuffer.setSize(0, 0);
    overlapBuffer.setSize(0, 0);
    fftBuffer.setSize(0, 0);

#ifdef USE_RUBBERBAND
    rubberBand.reset();
#endif
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoTuneAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void AutoTuneAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (buffer.getNumSamples() == 0)
        return;

    // Update smoothed parameter values
    speedSmoothed.setTargetValue(*parameters.getRawParameterValue(Parameters::SPEED_ID));
    amountSmoothed.setTargetValue(*parameters.getRawParameterValue(Parameters::AMOUNT_ID));

    // Get current mode
    auto currentMode = static_cast<Parameters::Mode>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::MODE_ID))
    );

    // Process based on selected mode
    switch (currentMode)
    {
        case Parameters::Mode::Classic:
            processClassicMode(buffer);
            break;
        case Parameters::Mode::Hard:
            processHardMode(buffer);
            break;
        case Parameters::Mode::AI:
            processAIMode(buffer);
            break;
    }
}

void AutoTuneAudioProcessor::processClassicMode(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameter values
    float speed = speedSmoothed.getNextValue();
    float amount = amountSmoothed.getNextValue();
    auto key = static_cast<Parameters::Key>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::KEY_ID))
    );
    auto scale = static_cast<Parameters::Scale>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::SCALE_ID))
    );

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        // Pitch detection and correction
        std::vector<float> pitches(numSamples);
        pitchEngine.detectPitch(channelData, numSamples, pitches.data());
        
        // Apply pitch correction
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float currentPitch = pitches[sample];
            
            if (currentPitch > 0.0f) // Valid pitch detected
            {
                // Convert to MIDI note number
                float midiNote = Utils::frequencyToMidiNote(currentPitch);
                
                // Quantize to scale
                float targetNote = Utils::quantizeToScale(midiNote, key, scale);
                float targetFrequency = Utils::midiNoteToFrequency(targetNote);
                
                // Calculate correction amount
                float correction = (targetFrequency - currentPitch) * amount;
                
                // Apply smooth correction with speed control
                float smoothedCorrection = correction * speed * 0.1f; // Scale speed appropriately
                
                // Apply correction (simplified - real implementation would use pitch shifting)
                channelData[sample] *= (1.0f + smoothedCorrection * 0.001f); // Very subtle effect for demo
            }
        }
    }
}

void AutoTuneAudioProcessor::processHardMode(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameter values
    float speed = speedSmoothed.getNextValue();
    float amount = amountSmoothed.getNextValue();
    auto key = static_cast<Parameters::Key>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::KEY_ID))
    );
    auto scale = static_cast<Parameters::Scale>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::SCALE_ID))
    );

    // Hard mode applies immediate, aggressive correction
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        // Detect pitch
        std::vector<float> pitches(numSamples);
        pitchEngine.detectPitch(channelData, numSamples, pitches.data());
        
        // Apply hard correction
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float currentPitch = pitches[sample];
            
            if (currentPitch > 0.0f)
            {
                float midiNote = Utils::frequencyToMidiNote(currentPitch);
                float targetNote = Utils::quantizeToScale(midiNote, key, scale);
                float targetFrequency = Utils::midiNoteToFrequency(targetNote);
                
                // Hard correction - immediate snap to target
                float correction = (targetFrequency - currentPitch) * amount;
                float hardCorrection = correction * juce::jmin(speed * 10.0f, 1.0f); // Faster, harder correction
                
                // Apply more aggressive correction
                channelData[sample] *= (1.0f + hardCorrection * 0.01f);
            }
        }
    }
}

void AutoTuneAudioProcessor::processAIMode(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameter values
    float speed = speedSmoothed.getNextValue();
    float amount = amountSmoothed.getNextValue();
    auto key = static_cast<Parameters::Key>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::KEY_ID))
    );
    auto scale = static_cast<Parameters::Scale>(
        static_cast<int>(*parameters.getRawParameterValue(Parameters::SCALE_ID))
    );

    // AI-enhanced processing with CREPE/DDSP integration
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        if (aiModelLoader.areModelsLoaded())
        {
            // Use AI models for pitch detection and synthesis
            auto pitchPrediction = aiModelLoader.predictPitch(channelData, numSamples, currentSampleRate);
            
            if (pitchPrediction.confidence > 0.3f) // Only process if confident
            {
                float currentPitch = pitchPrediction.frequency;
                float midiNote = Utils::frequencyToMidiNote(currentPitch);
                float targetNote = Utils::quantizeToScale(midiNote, key, scale);
                float targetFrequency = Utils::midiNoteToFrequency(targetNote);
                
                // Create synthesis parameters for DDSP
                AIModelLoader::SynthesisParams synthParams;
                synthParams.fundamentalFreq = targetFrequency;
                synthParams.harmonicAmplitudes = pitchPrediction.harmonics;
                synthParams.loudness = amount * 0.01f;
                
                // Process with DDSP for natural-sounding correction
                std::vector<float> tempBuffer(numSamples);
                std::copy(channelData, channelData + numSamples, tempBuffer.begin());
                
                if (aiModelLoader.processWithDDSP(tempBuffer.data(), channelData, numSamples, synthParams))
                {
                    // Blend original and AI-processed audio based on speed parameter
                    float blendFactor = speed * 0.01f;
                    for (int i = 0; i < numSamples; ++i)
                    {
                        channelData[i] = tempBuffer[i] * (1.0f - blendFactor) + channelData[i] * blendFactor;
                    }
                }
            }
        }
        else
        {
            // Fallback to advanced pitch detection without AI models
            std::vector<float> pitches(numSamples);
            pitchEngine.detectPitchAdvanced(channelData, numSamples, pitches.data());
            
            // AI-style correction with formant preservation
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float currentPitch = pitches[sample];
                
                if (currentPitch > 0.0f)
                {
                    float midiNote = Utils::frequencyToMidiNote(currentPitch);
                    float targetNote = Utils::quantizeToScale(midiNote, key, scale);
                    float targetFrequency = Utils::midiNoteToFrequency(targetNote);
                    
                    // AI-style smooth correction with natural timing
                    float pitchDiff = std::abs(targetFrequency - currentPitch);
                    float aiSpeed = speed * (1.0f - std::exp(-pitchDiff * 0.1f)); // Adaptive speed
                    float correction = (targetFrequency - currentPitch) * amount * aiSpeed * 0.01f;
                    
                    // Apply with formant considerations (simplified)
                    channelData[sample] *= (1.0f + correction * 0.005f);
                }
            }
        }
    }

#ifdef USE_RUBBERBAND
    // Use Rubber Band for high-quality time/pitch manipulation in AI mode
    if (rubberBand)
    {
        // Configure rubber band for pitch shifting
        float pitchShift = amount * 0.1f; // Convert amount to pitch shift ratio
        rubberBand->setPitchScale(1.0f + pitchShift);
        
        // Process through rubber band (simplified integration)
        // Real implementation would handle proper buffering and processing
    }
#endif
}

void AutoTuneAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == Parameters::SPEED_ID)
    {
        speedSmoothed.setTargetValue(newValue);
    }
    else if (parameterID == Parameters::AMOUNT_ID)
    {
        amountSmoothed.setTargetValue(newValue);
    }
    else if (parameterID == Parameters::MODE_ID)
    {
        // Mode changed - could trigger additional setup
        auto newMode = static_cast<Parameters::Mode>(static_cast<int>(newValue));
        modeSelector.setCurrentMode(newMode);
    }
}

juce::AudioProcessorEditor* AutoTuneAudioProcessor::createEditor()
{
    return new AutoTuneAudioProcessorEditor(*this);
}

void AutoTuneAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AutoTuneAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoTuneAudioProcessor();
}
