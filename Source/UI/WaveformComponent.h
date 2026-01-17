#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class WaveformComponent final : public juce::Component, public juce::Timer
{
public:
    WaveformComponent();
    ~WaveformComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setBuffer (const juce::AudioBuffer<float>& buffer);

private:
    void timerCallback() override;

    juce::AudioBuffer<float> displayBuffer;
    juce::CriticalSection bufferLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};