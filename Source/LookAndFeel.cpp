#include "LookAndFeel.h"

// Color definitions
const juce::Colour ProAutoTuneLookAndFeel::Colors::background          = juce::Colour(0xff0D1117);
const juce::Colour ProAutoTuneLookAndFeel::Colors::surface             = juce::Colour(0xff161B22);
const juce::Colour ProAutoTuneLookAndFeel::Colors::surfaceVariant      = juce::Colour(0xff21262D);
const juce::Colour ProAutoTuneLookAndFeel::Colors::primary             = juce::Colour(0xff00D4FF);
const juce::Colour ProAutoTuneLookAndFeel::Colors::primaryVariant      = juce::Colour(0xff0099CC);
const juce::Colour ProAutoTuneLookAndFeel::Colors::secondary           = juce::Colour(0xff7C3AED);
const juce::Colour ProAutoTuneLookAndFeel::Colors::accent              = juce::Colour(0xffFF2D92);
const juce::Colour ProAutoTuneLookAndFeel::Colors::textPrimary         = juce::Colour(0xffffffff);
const juce::Colour ProAutoTuneLookAndFeel::Colors::textSecondary       = juce::Colour(0xffb8b8c8);
const juce::Colour ProAutoTuneLookAndFeel::Colors::textDisabled        = juce::Colour(0xff666666);
const juce::Colour ProAutoTuneLookAndFeel::Colors::border              = juce::Colour(0xff3d3d5c);
const juce::Colour ProAutoTuneLookAndFeel::Colors::borderFocus         = juce::Colour(0xff4CAF50);
const juce::Colour ProAutoTuneLookAndFeel::Colors::highlight           = juce::Colour(0xff4CAF50);
const juce::Colour ProAutoTuneLookAndFeel::Colors::shadow              = juce::Colour(0x80000000);

ProAutoTuneLookAndFeel::ProAutoTuneLookAndFeel()
{
    // Set default colors
    setColour(juce::ResizableWindow::backgroundColourId, Colors::background);
    setColour(juce::DocumentWindow::backgroundColourId, Colors::background);
    
    // Slider colors
    setColour(juce::Slider::backgroundColourId, Colors::surface);
    setColour(juce::Slider::thumbColourId, Colors::primary);
    setColour(juce::Slider::trackColourId, Colors::border);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::primary);
    setColour(juce::Slider::rotarySliderOutlineColourId, Colors::border);
    setColour(juce::Slider::textBoxBackgroundColourId, Colors::surface);
    setColour(juce::Slider::textBoxTextColourId, Colors::textPrimary);
    setColour(juce::Slider::textBoxOutlineColourId, Colors::border);
    
    // ComboBox colors
    setColour(juce::ComboBox::backgroundColourId, Colors::surface);
    setColour(juce::ComboBox::textColourId, Colors::textPrimary);
    setColour(juce::ComboBox::outlineColourId, Colors::border);
    setColour(juce::ComboBox::buttonColourId, Colors::surfaceVariant);
    setColour(juce::ComboBox::arrowColourId, Colors::textSecondary);
    
    // Button colors
    setColour(juce::TextButton::buttonColourId, Colors::surface);
    setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
    setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
    
    // Label colors
    setColour(juce::Label::textColourId, Colors::textPrimary);
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    
    // PopupMenu colors
    setColour(juce::PopupMenu::backgroundColourId, Colors::surface);
    setColour(juce::PopupMenu::textColourId, Colors::textPrimary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::primary);
    setColour(juce::PopupMenu::highlightedTextColourId, Colors::textPrimary);
}

ProAutoTuneLookAndFeel::~ProAutoTuneLookAndFeel()
{
}

void ProAutoTuneLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                             float sliderPosProportional, float rotaryStartAngle,
                                             float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(10.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;
    auto centre = bounds.getCentre();
    
    // Draw outer ring (track)
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                               rotaryStartAngle, rotaryEndAngle, true);
    
    g.setColour(Colors::border);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw filled arc (value)
    if (sliderPosProportional > 0.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                              rotaryStartAngle, toAngle, true);
        
        // Create gradient for the value arc
        juce::ColourGradient gradient(Colors::primary, centre.x - radius, centre.y,
                                     Colors::accent, centre.x + radius, centre.y, false);
        g.setGradientFill(gradient);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    // Draw inner circle
    auto innerRadius = radius * 0.6f;
    juce::Rectangle<float> innerCircle(centre.x - innerRadius, centre.y - innerRadius,
                                      innerRadius * 2.0f, innerRadius * 2.0f);
    
    // Inner circle gradient
    juce::ColourGradient innerGradient(Colors::surfaceVariant.brighter(0.2f), centre.x, centre.y - innerRadius,
                                      Colors::surface.darker(0.3f), centre.x, centre.y + innerRadius, false);
    g.setGradientFill(innerGradient);
    g.fillEllipse(innerCircle);
    
    // Inner circle border
    g.setColour(Colors::border);
    g.drawEllipse(innerCircle, 2.0f);
    
    // Draw pointer
    juce::Path pointer;
    auto pointerLength = radius * 0.4f;
    auto pointerThickness = 3.0f;
    
    pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength + 5.0f, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));
    
    g.setColour(Colors::textPrimary);
    g.fillPath(pointer);
    
    // Draw center dot
    auto dotRadius = 4.0f;
    g.setColour(Colors::primary);
    g.fillEllipse(centre.x - dotRadius, centre.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    
    // Add glow effect when focused
    if (slider.hasKeyboardFocus(false))
    {
        drawGlowEffect(g, bounds, Colors::primary, 0.8f);
    }
}

void ProAutoTuneLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                             float sliderPos, float minSliderPos, float maxSliderPos,
                                             const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackWidth = juce::jmin(6.0f, (float)juce::jmin(width, height) * 0.25f);
    juce::Point<float> startPoint, endPoint;
    
    if (style == juce::Slider::LinearHorizontal)
    {
        startPoint = {(float)x, (float)y + (float)height * 0.5f};
        endPoint = {(float)(x + width), startPoint.y};
    }
    else
    {
        startPoint = {(float)x + (float)width * 0.5f, (float)(y + height)};
        endPoint = {startPoint.x, (float)y};
    }
    
    juce::Path track;
    track.startNewSubPath(startPoint);
    track.lineTo(endPoint);
    g.setColour(Colors::border);
    g.strokePath(track, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});
    
    juce::Path filledTrack;
    filledTrack.startNewSubPath(startPoint);
    filledTrack.lineTo({sliderPos, startPoint.y});
    g.setColour(Colors::primary);
    g.strokePath(filledTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});
    
    // Draw thumb
    auto thumbWidth = getSliderThumbRadius(slider);
    g.setColour(Colors::primary);
    g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre({sliderPos, startPoint.y}));
    
    g.setColour(Colors::textPrimary);
    g.drawEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre({sliderPos, startPoint.y}), 1.0f);
}

juce::Label* ProAutoTuneLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto label = LookAndFeel_V4::createSliderTextBox(slider);
    
    label->setFont(getControlFont());
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::textColourId, Colors::textPrimary);
    label->setColour(juce::Label::backgroundColourId, Colors::surface);
    label->setColour(juce::Label::outlineColourId, Colors::border);
    
    return label;
}

void ProAutoTuneLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                         int buttonX, int buttonY, int buttonW, int buttonH,
                                         juce::ComboBox& comboBox)
{
    auto cornerSize = 6.0f;
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    
    // Background gradient
    juce::ColourGradient gradient(Colors::surface.brighter(0.1f), 0.0f, 0.0f,
                                 Colors::surface.darker(0.1f), 0.0f, (float)height, false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    // Border
    auto borderColor = comboBox.hasKeyboardFocus(false) ? Colors::borderFocus : Colors::border;
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.0f);
    
    // Button area
    auto buttonArea = juce::Rectangle<float>(buttonX, buttonY, buttonW, buttonH).reduced(2.0f);
    
    if (isButtonDown)
    {
        g.setColour(Colors::surfaceVariant.darker(0.2f));
        g.fillRoundedRectangle(buttonArea, cornerSize * 0.5f);
    }
    
    // Draw arrow
    auto arrowArea = buttonArea.reduced(buttonArea.getWidth() * 0.3f, buttonArea.getHeight() * 0.3f);
    juce::Path arrow;
    arrow.addTriangle(arrowArea.getX(), arrowArea.getY(),
                     arrowArea.getRight(), arrowArea.getY(),
                     arrowArea.getCentreX(), arrowArea.getBottom());
    
    g.setColour(Colors::textSecondary);
    g.fillPath(arrow);
    
    // Add subtle inner shadow
    g.setColour(Colors::shadow.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), cornerSize - 1.0f, 1.0f);
}

void ProAutoTuneLookAndFeel::positionComboBoxText(juce::ComboBox& comboBox, juce::Label& labelToPosition)
{
    labelToPosition.setBounds(8, 1, comboBox.getWidth() - 30, comboBox.getHeight() - 2);
    labelToPosition.setFont(getControlFont());
    labelToPosition.setJustificationType(juce::Justification::centredLeft);
}

void ProAutoTuneLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                 const juce::Colour& backgroundColour,
                                                 bool shouldDrawButtonAsHighlighted,
                                                 bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto cornerSize = 6.0f;
    
    juce::Colour baseColor = Colors::surface;
    
    if (shouldDrawButtonAsDown)
    {
        baseColor = Colors::surfaceVariant.darker(0.2f);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        baseColor = Colors::surfaceVariant.brighter(0.1f);
    }
    
    // Background gradient
    juce::ColourGradient gradient(baseColor.brighter(0.1f), bounds.getCentreX(), bounds.getY(),
                                 baseColor.darker(0.1f), bounds.getCentreX(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    // Border
    juce::Colour borderColor = Colors::border;
    if (button.hasKeyboardFocus(false))
        borderColor = Colors::borderFocus;
    
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    
    // Inner highlight
    if (shouldDrawButtonAsHighlighted && !shouldDrawButtonAsDown)
    {
        g.setColour(Colors::highlight.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds.reduced(1.0f), cornerSize - 1.0f);
    }
}

void ProAutoTuneLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown)
{
    auto font = getControlFont();
    g.setFont(font);
    
    juce::Colour textColor = Colors::textPrimary;
    if (!button.isEnabled())
        textColor = Colors::textDisabled;
    
    g.setColour(textColor);
    
    auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
    auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    auto leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    auto rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    auto textWidth = button.getWidth() - leftIndent - rightIndent;
    
    if (textWidth > 0)
    {
        g.drawFittedText(button.getButtonText(),
                        leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                        juce::Justification::centred, 2);
    }
}

void ProAutoTuneLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));
    
    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const auto font = getLabelFont(label);
        
        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);
        
        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
        
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());
        
        g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour(label.findColour(juce::Label::outlineColourId));
    }
    
    g.drawRect(label.getLocalBounds());
}

void ProAutoTuneLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    auto background = findColour(juce::PopupMenu::backgroundColourId);
    g.fillAll(background);
    
    // Add subtle border
    g.setColour(Colors::border);
    g.drawRect(0, 0, width, height, 1);
    
    // Add drop shadow effect
    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(2, 2, width - 4, height - 4, 4.0f);
    juce::DropShadow shadow(Colors::shadow, 8, juce::Point<int>(0, 2));
    shadow.drawForPath(g, shadowPath);
}

void ProAutoTuneLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              bool isSeparator, bool isActive, bool isHighlighted,
                                              bool isTicked, bool hasSubMenu, const juce::String& text,
                                              const juce::String& shortcutKeyText,
                                              const juce::Drawable* icon, const juce::Colour* textColour)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(juce::roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));
        
        g.setColour(Colors::border);
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        auto textColor = (textColour == nullptr ? findColour(juce::PopupMenu::textColourId) : *textColour);
        
        auto maxFontHeight = (float)area.getHeight() / 1.3f;
        
        if (isHighlighted && isActive)
        {
            g.setColour(Colors::primary.withAlpha(0.8f));
            g.fillRect(area.reduced(1));
            
            g.setColour(Colors::textPrimary);
        }
        else
        {
            g.setColour(textColor.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
        }
        
        g.setFont(getControlFont().withHeight(maxFontHeight));
        
        auto iconArea = area.reduced(5).removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();
        
        if (icon != nullptr)
        {
            icon->drawWithin(g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
        }
        else if (isTicked)
        {
            // Draw checkmark
            juce::Path tick;
            tick.startNewSubPath(iconArea.getX() + iconArea.getWidth() * 0.2f, iconArea.getCentreY());
            tick.lineTo(iconArea.getCentreX(), iconArea.getY() + iconArea.getHeight() * 0.8f);
            tick.lineTo(iconArea.getRight() - iconArea.getWidth() * 0.2f, iconArea.getY() + iconArea.getHeight() * 0.3f);
            
            g.strokePath(tick, juce::PathStrokeType(2.0f));
        }
        
        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getHeight();
            auto x = (float)area.getRight() - arrowH * 1.0f;
            auto halfH = (float)area.getHeight() * 0.5f;
            
            juce::Path path;
            path.addTriangle(x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);
            
            g.fillPath(path);
        }
        
        g.drawFittedText(text,
                        area.reduced(iconArea.getWidth() + 5, 0),
                        juce::Justification::centredLeft, 1);
        
        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = getControlFont().withHeight(maxFontHeight * 0.75f);
            g.setFont(f2);
            g.drawText(shortcutKeyText, area, juce::Justification::centredRight, true);
        }
    }
}

void ProAutoTuneLookAndFeel::drawGlowEffect(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                                           juce::Colour glowColor, float intensity)
{
    const int glowRadius = 10;
    const float alpha = 0.3f * intensity;
    
    for (int i = 0; i < glowRadius; ++i)
    {
        float currentAlpha = alpha * (1.0f - static_cast<float>(i) / glowRadius);
        g.setColour(glowColor.withAlpha(currentAlpha));
        g.drawRoundedRectangle(bounds.expanded(i), 8.0f + i, 1.0f);
    }
}

void ProAutoTuneLookAndFeel::drawGradientBackground(juce::Graphics& g, const juce::Rectangle<int>& bounds,
                                                   juce::Colour topColor, juce::Colour bottomColor)
{
    juce::ColourGradient gradient(topColor, 0.0f, (float)bounds.getY(),
                                 bottomColor, 0.0f, (float)bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
}

void ProAutoTuneLookAndFeel::drawRoundedPath(juce::Graphics& g, const juce::Path& path,
                                            juce::Colour fillColor, juce::Colour strokeColor,
                                            float strokeWidth)
{
    g.setColour(fillColor);
    g.fillPath(path);
    
    if (strokeWidth > 0.0f)
    {
        g.setColour(strokeColor);
        g.strokePath(path, juce::PathStrokeType(strokeWidth));
    }
}

juce::Font ProAutoTuneLookAndFeel::getTitleFont() const
{
    return juce::Font("Arial", 24.0f, juce::Font::bold);
}

juce::Font ProAutoTuneLookAndFeel::getControlFont() const
{
    return juce::Font("Arial", 14.0f, juce::Font::plain);
}

juce::Font ProAutoTuneLookAndFeel::getSmallFont() const
{
    return juce::Font("Arial", 12.0f, juce::Font::plain);
}
