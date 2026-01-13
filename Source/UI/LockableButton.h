#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class StarlightDriftAudioProcessor;

class LockableButton final : public juce::ToggleButton
{
public:
    LockableButton (StarlightDriftAudioProcessor& p, juce::String paramIdIn, juce::String label)
        : processor (p), paramId (std::move (paramIdIn))
    {
        setButtonText (std::move (label));
    }

    const juce::String& getParamId() const { return paramId; }

    void mouseDown (const juce::MouseEvent& e) override;
    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    StarlightDriftAudioProcessor& processor;
    juce::String paramId;
};

