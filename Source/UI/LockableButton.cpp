#include "LockableButton.h"

#include "../PluginProcessor.h"

void LockableButton::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        juce::PopupMenu m;
        const bool locked = processor.isParamLocked (paramId);
        m.addItem (1, locked ? "Unlock" : "Lock", true, locked);
        
        m.showMenuAsync (juce::PopupMenu::Options(),
            [this, locked] (int res)
            {
                if (res == 1)
                    processor.setParamLocked (paramId, ! locked);
                repaint();
            });

        return;
    }

    if (processor.isParamLocked (paramId))
        return;

    ToggleButton::mouseDown (e);
}

void LockableButton::paintButton (juce::Graphics& g, bool, bool)
{
    auto b = getLocalBounds().toFloat();
    const bool on = getToggleState();
    const bool locked = processor.isParamLocked (paramId);

    g.setColour (juce::Colours::white.withAlpha (locked ? 0.25f : 0.45f));
    g.drawRoundedRectangle (b.reduced (1.0f), 6.0f, 1.0f);

    if (on)
    {
        g.setColour (juce::Colours::white.withAlpha (locked ? 0.20f : 0.30f));
        g.fillRoundedRectangle (b.reduced (2.0f), 5.0f);
    }

    g.setColour (juce::Colours::white.withAlpha (locked ? 0.55f : 0.85f));
    g.setFont (12.0f);
    g.drawText (getButtonText(), getLocalBounds(), juce::Justification::centred);
}

