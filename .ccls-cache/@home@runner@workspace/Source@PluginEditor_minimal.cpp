#include "PluginProcessor.h"

// Minimal editor implementation - audio processing only
class AutoTuneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AutoTuneAudioProcessorEditor(AutoTuneAudioProcessor& p)
        : AudioProcessorEditor(p), audioProcessor(p)
    {
        setSize(400, 300);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawText("AutoTune Plugin - Audio Processing Mode", getLocalBounds(), juce::Justification::centred);
    }

    void resized() override {}

private:
    AutoTuneAudioProcessor& audioProcessor;
};

juce::AudioProcessorEditor* AutoTuneAudioProcessor::createEditor()
{
    return new AutoTuneAudioProcessorEditor(*this);
}