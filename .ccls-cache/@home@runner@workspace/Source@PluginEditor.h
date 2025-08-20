#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"

class AutoTuneAudioProcessorEditor : public AudioProcessorEditor,
                                     public Timer,
                                     public Button::Listener
{
public:
    AutoTuneAudioProcessorEditor(AutoTuneAudioProcessor&);
    ~AutoTuneAudioProcessorEditor() override;

    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void buttonClicked(Button* button) override;

private:
    AutoTuneAudioProcessor& audioProcessor;
    
    // Custom Look and Feel
    ProAutoTuneLookAndFeel lookAndFeel;
    
    // Main controls
    Slider speedSlider;
    Label speedLabel;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    
    Slider amountSlider;
    Label amountLabel;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    
    ComboBox modeSelector;
    Label modeLabel;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    
    ComboBox keySelector;
    Label keyLabel;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> keyAttachment;
    
    juce::ComboBox scaleSelector;
    juce::Label scaleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttachment;
    
    // Preset controls
    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;
    juce::ComboBox presetSelector;
    
    // Visual elements
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> controlsArea;
    juce::Rectangle<int> presetArea;
    
    // Real-time display
    float currentInputLevel = 0.0f;
    float currentOutputLevel = 0.0f;
    std::vector<float> pitchHistory;
    static constexpr int pitchHistorySize = 100;
    
    // Helper methods
    void setupControls();
    void setupLayout();
    void updatePresetList();
    void drawHeader(juce::Graphics& g, const juce::Rectangle<int>& area);
    void drawControls(juce::Graphics& g, const juce::Rectangle<int>& area);
    void drawPresetSection(juce::Graphics& g, const juce::Rectangle<int>& area);
    void drawPitchDisplay(juce::Graphics& g, const juce::Rectangle<int>& area);
    void drawLevelMeters(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoTuneAudioProcessorEditor)
};
