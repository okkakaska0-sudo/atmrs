#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

class ProAutoTuneLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ProAutoTuneLookAndFeel();
    ~ProAutoTuneLookAndFeel() override;

    // Slider customization
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    
    // ComboBox customization
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& comboBox) override;
    
    void positionComboBoxText(juce::ComboBox& comboBox, juce::Label& labelToPosition) override;
    
    // Button customization
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;
    
    // Label customization
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    // Popup menu customization
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const juce::String& text,
                          const juce::String& shortcutKeyText,
                          const juce::Drawable* icon, const juce::Colour* textColour) override;

    // Color scheme
    struct Colors
    {
        static const juce::Colour background;
        static const juce::Colour surface;
        static const juce::Colour surfaceVariant;
        static const juce::Colour primary;
        static const juce::Colour primaryVariant;
        static const juce::Colour secondary;
        static const juce::Colour accent;
        static const juce::Colour textPrimary;
        static const juce::Colour textSecondary;
        static const juce::Colour textDisabled;
        static const juce::Colour border;
        static const juce::Colour borderFocus;
        static const juce::Colour highlight;
        static const juce::Colour shadow;
    };

private:
    // Helper methods
    void drawGlowEffect(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                       juce::Colour glowColor, float intensity = 1.0f);
    
    void drawGradientBackground(juce::Graphics& g, const juce::Rectangle<int>& bounds,
                               juce::Colour topColor, juce::Colour bottomColor);
    
    void drawRoundedPath(juce::Graphics& g, const juce::Path& path,
                        juce::Colour fillColor, juce::Colour strokeColor,
                        float strokeWidth = 1.0f);
    
    // Custom fonts
    juce::Font getTitleFont() const;
    juce::Font getControlFont() const;
    juce::Font getSmallFont() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProAutoTuneLookAndFeel)
};
