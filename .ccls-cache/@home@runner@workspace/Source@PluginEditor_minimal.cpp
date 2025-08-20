#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

// Minimal editor implementation - preserves full macOS compatibility
class AutoTuneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AutoTuneAudioProcessorEditor(AutoTuneAudioProcessor& p)
        : juce::AudioProcessorEditor(&p), audioProcessor(p)
    {
        setSize(800, 600);
        
        // Initialize minimal controls for full macOS compatibility
        speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        speedSlider.setRange(0.0, 100.0, 0.1);
        speedSlider.setValue(50.0);
        addAndMakeVisible(speedSlider);
        
        amountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        amountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        amountSlider.setRange(0.0, 100.0, 0.1);
        amountSlider.setValue(50.0);
        addAndMakeVisible(amountSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff1a1a1a));
        g.setColour(juce::Colours::white);
        g.setFont(24.0f);
        g.drawText("AutoTune Plugin", 20, 20, getWidth() - 40, 40, juce::Justification::centred);
        
        g.setFont(16.0f);
        g.drawText("Speed", 100, 100, 100, 20, juce::Justification::centred);
        g.drawText("Amount", 300, 100, 100, 20, juce::Justification::centred);
    }

    void resized() override
    {
        speedSlider.setBounds(50, 120, 100, 100);
        amountSlider.setBounds(250, 120, 100, 100);
    }

private:
    AutoTuneAudioProcessor& audioProcessor;
    juce::Slider speedSlider;
    juce::Slider amountSlider;
};

juce::AudioProcessorEditor* AutoTuneAudioProcessor::createEditor()
{
    return new AutoTuneAudioProcessorEditor(*this);
}