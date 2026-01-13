#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class StarlightLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    // Blended Palette: Deep Space Purple + Vibrant Accents
    
    // Backgrounds
    const juce::Colour bgDeep        { 0xff120e16 }; // Deep Purple-Black (Concatenator vibe)
    const juce::Colour bgPanel       { 0xff1a1620 }; 
    const juce::Colour bgInset       { 0xff0f0b12 }; 
    
    // Vibrant Accents (The Blend)
    const juce::Colour accGold       { 0xfffab005 }; // Retro Gold (Drift/Main)
    const juce::Colour accCyan       { 0xff22b8cf }; // Sci-Fi Cyan (Grains/Time)
    const juce::Colour accPink       { 0xffe64980 }; // Vaporwave Pink (Shimmer/Mod)
    const juce::Colour accOrange     { 0xfffd7e14 }; // Warm Orange (Filters/Analogue)
    
    // Text
    const juce::Colour txtWhite      { 0xffffffff };
    const juce::Colour txtDim        { 0xff868e96 };

    StarlightLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, txtWhite);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        
        setColour (juce::ComboBox::backgroundColourId, bgInset);
        setColour (juce::ComboBox::outlineColourId, txtDim.withAlpha(0.2f));
        setColour (juce::ComboBox::arrowColourId, txtDim);
        setColour (juce::ComboBox::textColourId, txtWhite);
        
        setColour (juce::PopupMenu::backgroundColourId, bgPanel);
        setColour (juce::PopupMenu::textColourId, txtWhite);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, accPink.withAlpha(0.2f));
    }

    // Dynamic Coloring based on Name
    juce::Colour getColorForParam(const juce::String& name)
    {
        juce::String n = name.toLowerCase();
        if (n.contains("drift") || n.contains("warm") || n.contains("gold")) return accGold;
        if (n.contains("grain") || n.contains("time") || n.contains("freeze")) return accCyan;
        if (n.contains("shimmer") || n.contains("verb") || n.contains("mod")) return accPink;
        if (n.contains("input") || n.contains("filter") || n.contains("mix")) return accOrange;
        return accCyan; // Default
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, 
                           float sliderPosProportional, float rotaryStartAngle, 
                           float rotaryEndAngle, juce::Slider& s) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(4.0f);
        auto center = bounds.getCentre();
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        float lineW = radius * 0.12f;
        float arcRadius = radius - lineW * 0.5f;
        
        juce::Colour prim = getColorForParam(s.getName());

        // Outer shadow/glow ring
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillEllipse(center.x - radius - 2, center.y - radius - 2, (radius + 2) * 2, (radius + 2) * 2);

        // Base circle with gradient
        juce::ColourGradient baseGrad(bgInset.brighter(0.1f), center.x - radius * 0.3f, center.y - radius * 0.3f,
                                      bgInset.darker(0.3f), center.x + radius * 0.3f, center.y + radius * 0.3f, false);
        g.setGradientFill(baseGrad);
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
        
        // Inner highlight ring
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawEllipse(center.x - radius + 2, center.y - radius + 2, (radius - 2) * 2, (radius - 2) * 2, 1.0f);

        // 1. Inset Track (Darker with depth)
        juce::Path track;
        track.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(bgInset.darker(0.4f));
        g.strokePath(track, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Track highlight
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(track, juce::PathStrokeType(lineW * 0.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // 2. Active Arc (Vibrant Gradient with glow)
        if (sliderPosProportional > 0.0f)
        {
            float toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
            juce::Path valuePath;
            valuePath.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);
            
            // Outer glow
            g.setColour(prim.withAlpha(0.15f));
            g.strokePath(valuePath, juce::PathStrokeType(lineW * 2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            // Main arc with gradient
            juce::ColourGradient grad(prim.darker(0.3f), center.x - radius, center.y + radius,
                                      prim.brighter(0.3f), center.x + radius, center.y - radius, false);
            g.setGradientFill(grad);
            g.strokePath(valuePath, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            // Inner highlight
            g.setColour(prim.brighter(0.5f).withAlpha(0.6f));
            g.strokePath(valuePath, juce::PathStrokeType(lineW * 0.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        
        // 3. Center dot with gradient
        float centerDotRadius = radius * 0.15f;
        juce::ColourGradient centerGrad(bgInset.brighter(0.2f), center.x - centerDotRadius * 0.5f, center.y - centerDotRadius * 0.5f,
                                        bgInset.darker(0.2f), center.x + centerDotRadius * 0.5f, center.y + centerDotRadius * 0.5f, false);
        g.setGradientFill(centerGrad);
        g.fillEllipse(center.x - centerDotRadius, center.y - centerDotRadius, centerDotRadius * 2, centerDotRadius * 2);
        
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawEllipse(center.x - centerDotRadius, center.y - centerDotRadius, centerDotRadius * 2, centerDotRadius * 2, 1.0f);
        
        // 4. Thumb Marker (Enhanced with glow)
        float thumbR = arcRadius - lineW * 0.5f;
        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        float tx = center.x + thumbR * std::cos(angle - juce::MathConstants<float>::halfPi);
        float ty = center.y + thumbR * std::sin(angle - juce::MathConstants<float>::halfPi);
        
        // Glow
        g.setColour(prim.withAlpha(0.4f));
        g.fillEllipse(tx - 5, ty - 5, 10, 10);
        
        // Main dot
        g.setColour(prim);
        g.fillEllipse(tx - 3.5f, ty - 3.5f, 7, 7);
        
        // Highlight
        g.setColour(prim.brighter(0.8f));
        g.fillEllipse(tx - 2, ty - 2, 4, 4);
    }
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool) override
    {
        auto bounds = b.getLocalBounds().toFloat().reduced(1.0f);
        bool on = b.getToggleState();
        juce::Colour c = getColorForParam(b.getButtonText());
        
        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(bounds.translated(0, 2), 6.0f);
        
        // Base gradient
        juce::ColourGradient baseGrad(
            on ? bgInset.brighter(0.1f) : bgInset.darker(0.1f), bounds.getX(), bounds.getY(),
            on ? bgInset.darker(0.2f) : bgInset.darker(0.3f), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(baseGrad);
        g.fillRoundedRectangle(bounds, 6.0f);
        
        // Border with glow when on
        if (on)
        {
            g.setColour(c.withAlpha(0.6f));
            g.drawRoundedRectangle(bounds, 6.0f, 2.0f);
            
            // Inner glow
            g.setColour(c.withAlpha(0.2f));
            g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f, 1.0f);
        }
        else
        {
            g.setColour(txtDim.withAlpha(0.3f));
            g.drawRoundedRectangle(bounds, 6.0f, 1.5f);
        }
        
        // Glowing Center when on
        if (on)
        {
            // Main glow
            g.setColour(c.withAlpha(0.25f));
            g.fillRoundedRectangle(bounds.reduced(3.0f), 3.0f);
            
            // Bright LED strip at bottom
            auto ledStrip = bounds.removeFromBottom(5.0f).reduced(6.0f, 1.0f);
            juce::ColourGradient ledGrad(c.withAlpha(0.0f), ledStrip.getX(), ledStrip.getY(),
                                         c, ledStrip.getCentreX(), ledStrip.getY(), false);
            g.setGradientFill(ledGrad);
            g.fillRoundedRectangle(ledStrip, 2.0f);
            
            // Highlight on LED
            g.setColour(c.brighter(0.5f));
            g.fillRoundedRectangle(ledStrip.reduced(2.0f, 0), 1.5f);
        }
        
        // Text with shadow when on
        g.setColour(on ? txtWhite : txtDim);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        if (on)
        {
            g.setColour(juce::Colours::black.withAlpha(0.3f));
            g.drawText(b.getButtonText(), bounds.translated(0, 1), juce::Justification::centred);
            g.setColour(txtWhite);
        }
        g.drawText(b.getButtonText(), bounds, juce::Justification::centred);
    }
    
    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float>(0,0,width,height).reduced(1.0f);
        g.setColour(bgInset);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        g.setColour(accPink.withAlpha(0.3f)); // Pink hint for menus
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        // Arrow
        juce::Path p;
        p.addTriangle(width-15, height*0.4f, width-10, height*0.4f, width-12.5f, height*0.7f);
        g.setColour(txtDim);
        g.fillPath(p);
    }

    // Module Panel with Colored Header
    void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> area, juce::String title, juce::Colour accent)
    {
        auto bounds = area.toFloat();
        
        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillRoundedRectangle(bounds.translated(0, 3), 10.0f);
        
        // Main panel with gradient
        juce::ColourGradient panelGrad(
            bgPanel.brighter(0.05f), bounds.getX(), bounds.getY(),
            bgPanel.darker(0.1f), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(panelGrad);
        g.fillRoundedRectangle(bounds, 10.0f);
        
        // Outer border
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, 10.0f, 1.5f);
        
        // Inner highlight
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 9.0f, 1.0f);
        
        // Header Strip
        auto header = area.removeFromTop(32).toFloat();
        
        // Header background with gradient
        juce::ColourGradient headerGrad(
            accent.withAlpha(0.15f), header.getX(), header.getY(),
            accent.withAlpha(0.05f), header.getX(), header.getBottom(), false);
        g.setGradientFill(headerGrad);
        g.fillRoundedRectangle(header, 10.0f);
        
        // Accent underline with gradient
        auto underline = header.removeFromBottom(3.0f).reduced(12.0f, 0);
        juce::ColourGradient lineGrad(accent.withAlpha(0.3f), underline.getX(), 0,
                                      accent, underline.getCentreX(), 0, false);
        g.setGradientFill(lineGrad);
        g.fillRoundedRectangle(underline, 1.5f);
        
        // Fade out edges
        g.setColour(accent.withAlpha(0.3f));
        g.fillRoundedRectangle(underline.withWidth(underline.getWidth() * 0.3f), 1.5f);
        g.fillRoundedRectangle(underline.withX(underline.getRight() - underline.getWidth() * 0.3f).withWidth(underline.getWidth() * 0.3f), 1.5f);
        
        // Accent glow
        g.setColour(accent.withAlpha(0.2f));
        g.fillRoundedRectangle(underline.expanded(2.0f, 0), 2.0f);
        
        // Title with shadow
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.setFont(juce::Font(13.0f, juce::Font::bold));
        g.drawText(title.toUpperCase(), header.translated(0, 1), juce::Justification::centred);
        
        g.setColour(accent.brighter(0.6f));
        g.drawText(title.toUpperCase(), header, juce::Justification::centred);
        
        // "Screws" or Tech details with glow
        float screwSize = 5.0f;
        float screwY = header.getCentreY() - screwSize * 0.5f;
        
        // Left screw
        g.setColour(accent.withAlpha(0.4f));
        g.fillEllipse(header.getX() + 8 - screwSize * 0.5f, screwY - 1, screwSize + 2, screwSize + 2);
        g.setColour(accent.withAlpha(0.6f));
        g.fillEllipse(header.getX() + 8 - screwSize * 0.5f, screwY, screwSize, screwSize);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawEllipse(header.getX() + 8 - screwSize * 0.5f, screwY, screwSize, screwSize, 0.5f);
        
        // Right screw
        g.setColour(accent.withAlpha(0.4f));
        g.fillEllipse(header.getRight() - 12 - screwSize * 0.5f, screwY - 1, screwSize + 2, screwSize + 2);
        g.setColour(accent.withAlpha(0.6f));
        g.fillEllipse(header.getRight() - 12 - screwSize * 0.5f, screwY, screwSize, screwSize);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawEllipse(header.getRight() - 12 - screwSize * 0.5f, screwY, screwSize, screwSize, 0.5f);
    }
    
    // The "Galaxy" Background
    void drawStarfield(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        // Deep Vertical Gradient with more depth
        juce::ColourGradient mainGrad(
            bgDeep.darker(0.1f), 0, 0,
            bgDeep, 0, bounds.getHeight(), false);
        mainGrad.addColour(0.6f, bgDeep.brighter(0.15f));
        g.setGradientFill(mainGrad);
        g.fillAll();
        
        // Additional radial gradients for depth
        juce::ColourGradient radialGrad1(
            accGold.withAlpha(0.03f), bounds.getCentreX(), bounds.getCentreY(),
            juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getCentreY() + 300, true);
        g.setGradientFill(radialGrad1);
        g.fillEllipse(bounds.getCentreX() - 300, bounds.getCentreY() - 300, 600, 600);
        
        // Nebula Clouds (Simulated with large soft circles and gradients)
        juce::Random r(999);
        
        // Pink Cloud (top left) with gradient
        juce::ColourGradient pinkGrad(
            accPink.withAlpha(0.08f), -100, -100,
            juce::Colours::transparentBlack, 200, 200, false);
        g.setGradientFill(pinkGrad);
        g.fillEllipse(-150, -150, 700, 600);
        
        // Cyan Cloud (bottom right) with gradient
        juce::ColourGradient cyanGrad(
            accCyan.withAlpha(0.06f), bounds.getWidth() - 200, bounds.getHeight() - 200,
            juce::Colours::transparentBlack, bounds.getWidth() - 600, bounds.getHeight() - 600, false);
        g.setGradientFill(cyanGrad);
        g.fillEllipse(bounds.getWidth() - 500, bounds.getHeight() - 400, 700, 600);
        
        // Orange accent cloud (middle right)
        juce::ColourGradient orangeGrad(
            accOrange.withAlpha(0.04f), bounds.getWidth() * 0.7f, bounds.getHeight() * 0.3f,
            juce::Colours::transparentBlack, bounds.getWidth() * 0.7f, bounds.getHeight() * 0.3f + 200, true);
        g.setGradientFill(orangeGrad);
        g.fillEllipse(bounds.getWidth() * 0.7f - 200, bounds.getHeight() * 0.3f - 200, 400, 400);

        // Stars with varying sizes and brightness
        for (int i=0; i<180; ++i)
        {
            float x = r.nextFloat() * bounds.getWidth();
            float y = r.nextFloat() * bounds.getHeight();
            float sz = 0.5f + r.nextFloat() * 2.5f;
            float a = 0.3f + r.nextFloat() * 0.7f;
            
            // Some stars are colored
            if (r.nextFloat() > 0.85f)
            {
                juce::Colour starCol = (r.nextFloat() > 0.5f) ? accCyan : accGold;
                g.setColour(starCol.withAlpha(a * 0.6f));
            }
            else
            {
                g.setColour(juce::Colours::white.withAlpha(a));
            }
            
            g.fillEllipse(x, y, sz, sz);
            
            // Add glow to brighter stars
            if (a > 0.8f)
            {
                g.setColour(juce::Colours::white.withAlpha(a * 0.3f));
                g.fillEllipse(x - 1, y - 1, sz + 2, sz + 2);
            }
        }
        
        // Subtle vignette
        juce::ColourGradient vignette(
            juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getCentreY(),
            juce::Colours::black.withAlpha(0.15f), bounds.getCentreX(), bounds.getCentreY(), true);
        g.setGradientFill(vignette);
        g.fillEllipse(bounds.getCentreX() - bounds.getWidth() * 0.7f, 
                     bounds.getCentreY() - bounds.getHeight() * 0.7f,
                     bounds.getWidth() * 1.4f, bounds.getHeight() * 1.4f);
    }
};
