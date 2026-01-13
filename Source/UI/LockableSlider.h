#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class StarlightDriftAudioProcessor;

class LockableSlider final : public juce::Slider
{
public:
    enum class Style { Tiny, Small, Large };

    LockableSlider (StarlightDriftAudioProcessor& p, juce::String paramIdIn, juce::String label, Style styleIn)
        : processor (p), paramId (std::move (paramIdIn)), style (styleIn)
    {
        setName (std::move (label));
        setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        setMouseDragSensitivity (200);
        setWantsKeyboardFocus (true);
    }

    const juce::String& getParamId() const { return paramId; }

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    void paintOverChildren (juce::Graphics& g) override;

private:
    StarlightDriftAudioProcessor& processor;
    juce::String paramId;
    Style style {};

    double prevValue = 0.0;
    double valueAtMouseDown = 0.0;
    bool prevInitialised = false;
};
