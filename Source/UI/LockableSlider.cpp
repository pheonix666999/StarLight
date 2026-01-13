#include "LockableSlider.h"

#include "../PluginProcessor.h"

static juce::Path makeLockIcon()
{
    juce::Path p;
    p.addRoundedRectangle (juce::Rectangle<float> (3.0f, 8.0f, 10.0f, 9.0f), 2.0f);
    p.startNewSubPath (5.0f, 8.0f);
    p.cubicTo (5.0f, 2.5f, 11.0f, 2.5f, 11.0f, 8.0f);
    return p;
}

void LockableSlider::mouseDown (const juce::MouseEvent& e)
{
    if (! prevInitialised)
    {
        prevValue = getValue();
        prevInitialised = true;
    }

    if (e.mods.isRightButtonDown())
    {
        juce::PopupMenu m;
        const bool locked = processor.isParamLocked (paramId);
        m.addItem (1, locked ? "Unlock" : "Lock", true, locked);
        m.addSeparator();
        m.addItem (2, "Reset to default");

        m.showMenuAsync (juce::PopupMenu::Options(),
            [this, locked] (int res)
            {
                if (res == 1)
                    processor.setParamLocked (paramId, ! locked);
                else if (res == 2)
                    setValue (getDoubleClickReturnValue(), juce::sendNotificationSync);

                repaint();
            });

        return;
    }

    if (processor.isParamLocked (paramId))
        return;

    if (e.mods.isShiftDown() && e.mods.isLeftButtonDown())
    {
        const auto current = getValue();
        setValue (prevValue, juce::sendNotificationSync);
        prevValue = current;
        return;
    }

    valueAtMouseDown = getValue();
    Slider::mouseDown (e);
}

void LockableSlider::mouseUp (const juce::MouseEvent& e)
{
    if (processor.isParamLocked (paramId))
        return;

    Slider::mouseUp (e);

    const auto current = getValue();
    if (current != valueAtMouseDown)
        prevValue = valueAtMouseDown;
}

void LockableSlider::paintOverChildren (juce::Graphics& g)
{
    const bool locked = processor.isParamLocked (paramId);
    
    // Draw label below knob
    auto labelArea = getLocalBounds();
    labelArea.removeFromTop(labelArea.getHeight() * 0.75f); // Keep bottom 25% for label
    
    auto name = getName();
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font(10.0f, juce::Font::plain));
    g.drawText(name, labelArea, juce::Justification::centredTop, true);
    
    // Draw value text
    juce::String valueText;
    if (name.contains("FREQ"))
    {
        valueText = juce::String(getValue(), 0) + " Hz";
    }
    else if (name.contains("TIME") || name.contains("PRE-DLY") || name.contains("SIZE"))
    {
        valueText = juce::String(getValue(), 1) + " ms";
    }
    else if (name.contains("MIX") || name.contains("GAIN") || name.contains("OUTPUT") || name.contains("INPUT"))
    {
        valueText = juce::String(getValue(), 1) + " dB";
    }
    else
    {
        valueText = juce::String(getValue(), 2);
    }
    
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::Font(9.0f, juce::Font::plain));
    auto valueArea = labelArea.removeFromTop(labelArea.getHeight());
    g.drawText(valueText, valueArea, juce::Justification::centred, true);
    
    // Draw lock overlay if locked
    if (! locked)
        return;

    auto b = getLocalBounds().toFloat().reduced (6.0f);
    g.setColour (juce::Colours::black.withAlpha (0.30f));
    g.fillEllipse (b);

    auto icon = makeLockIcon();
    icon.applyTransform (juce::AffineTransform::scale (1.6f).translated (b.getCentreX() - 11.0f, b.getCentreY() - 13.0f));
    g.setColour (juce::Colours::white.withAlpha (0.65f));
    g.fillPath (icon);
}
