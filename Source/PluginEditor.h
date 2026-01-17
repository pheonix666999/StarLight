#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/LockableSlider.h"
#include "UI/LockableButton.h"
#include "UI/WaveformComponent.h"

class StarlightDriftAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit StarlightDriftAudioProcessorEditor (StarlightDriftAudioProcessor&);
    ~StarlightDriftAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    StarlightDriftAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    StarlightLookAndFeel lnf;

    // Main macros (top section)
    LockableSlider drift;
    LockableSlider air;
    LockableSlider glass;

    // Global controls
    LockableButton freeze;
    LockableSlider mix;
    LockableSlider output;

    // Filter controls
    LockableButton hpEnable;
    LockableSlider hpFreq;
    LockableButton lpEnable;
    LockableSlider lpFreq;

    // Granular delay controls
    LockableSlider inputGain;
    LockableSlider timeMs;
    LockableSlider feedback;
    LockableSlider grainSize;
    LockableSlider density;
    LockableSlider jitter;
    LockableSlider pitch;
    LockableSlider spread;

    // Reverb controls
    LockableSlider reverbSize;
    LockableSlider preDelay;
    LockableSlider tone;
    LockableSlider shimmerAmt;
    juce::ComboBox shimmerPitch;
    LockableSlider reverbMix;

    // Modulation
    LockableSlider modRate;
    LockableSlider modDepth;

    // Waveform
    WaveformComponent waveform;
    juce::AudioBuffer<float> waveformScratch;
    juce::Label noInputLabel;
    int silentFrameCount = 0;

    // Attachments
    std::unique_ptr<SliderAttachment> attDrift, attAir, attGlass;
    std::unique_ptr<ButtonAttachment> attFreeze;
    std::unique_ptr<SliderAttachment> attMix, attOutput;
    std::unique_ptr<ButtonAttachment> attHpEnable, attLpEnable;
    std::unique_ptr<SliderAttachment> attHpFreq, attLpFreq;
    std::unique_ptr<SliderAttachment> attInputGain, attTimeMs, attFeedback, attGrainSize, attDensity, attJitter, attPitch, attSpread;
    std::unique_ptr<SliderAttachment> attReverbSize, attPreDelay, attTone, attShimmerAmt, attReverbMix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attShimmerPitch;
    std::unique_ptr<SliderAttachment> attModRate, attModDepth;

    // Layout helper
    void layoutKnobGrid (juce::Rectangle<int> area, std::initializer_list<juce::Component*> knobs, int rows, int cols);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarlightDriftAudioProcessorEditor)
};
