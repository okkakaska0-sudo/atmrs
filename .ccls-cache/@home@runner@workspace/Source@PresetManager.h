#pragma once

#include "JuceHeader.h"
#include <vector>

class PresetManager
{
public:
    struct Preset
    {
        String name;
        float speed;
        float amount;
        int mode;
        int key;
        int scale;
        String description;
        Time dateCreated;
        
        Preset() : speed(50.0f), amount(50.0f), mode(0), key(0), scale(0) {}
        
        Preset(const String& presetName, float speedVal, float amountVal, 
               int modeVal, int keyVal, int scaleVal, const String& desc = String())
            : name(presetName), speed(speedVal), amount(amountVal), mode(modeVal), 
              key(keyVal), scale(scaleVal), description(desc), dateCreated(Time::getCurrentTime()) {}
    };

    PresetManager(AudioProcessorValueTreeState& parameters);
    ~PresetManager();

    // Preset management
    void savePreset(const String& name, const String& description = String());
    bool loadPreset(int index);
    bool loadPreset(const juce::String& name);
    void deletePreset(int index);
    void deletePreset(const juce::String& name);
    
    // Factory presets
    void loadFactoryPresets();
    void resetToDefaults();
    
    // Preset access
    int getNumPresets() const { return static_cast<int>(presets.size()); }
    const Preset& getPreset(int index) const;
    juce::StringArray getPresetNames() const;
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    
    // File operations
    void savePresetsToFile();
    void loadPresetsFromFile();
    juce::File getPresetDirectory() const;
    juce::File getPresetFile() const;
    
    // Import/Export
    bool exportPreset(int index, const juce::File& file);
    bool importPreset(const juce::File& file);
    bool exportAllPresets(const juce::File& file);
    bool importPresetsFromFile(const juce::File& file);
    
    // Validation
    bool isValidPresetIndex(int index) const;
    bool presetExists(const juce::String& name) const;
    
    // Callbacks
    std::function<void()> onPresetChanged;
    std::function<void(const juce::String&)> onPresetSaved;
    std::function<void(const juce::String&)> onPresetDeleted;

private:
    juce::AudioProcessorValueTreeState& parameters;
    std::vector<Preset> presets;
    int currentPresetIndex = -1;
    
    // File management
    static const juce::String presetFileExtension;
    static const juce::String presetFileName;
    
    // Helper methods
    Preset createPresetFromCurrentState(const juce::String& name, const juce::String& description = juce::String());
    void applyPresetToParameters(const Preset& preset);
    juce::ValueTree presetToValueTree(const Preset& preset);
    Preset valueTreeToPreset(const juce::ValueTree& tree);
    
    // Factory preset definitions
    void addFactoryPreset(const juce::String& name, float speed, float amount, int mode, int key, int scale, const juce::String& description);
    
    // File I/O helpers
    bool writePresetToXml(const Preset& preset, juce::XmlElement& xml);
    bool readPresetFromXml(const juce::XmlElement& xml, Preset& preset);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
