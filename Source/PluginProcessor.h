#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "DSP/GranularDelay.h"
#include "DSP/ShimmerReverb.h"

class StarlightDriftAudioProcessor final : public juce::AudioProcessor
{
public:
    StarlightDriftAudioProcessor();
    ~StarlightDriftAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override;

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    bool isParamLocked (const juce::String& paramId) const;
    void setParamLocked (const juce::String& paramId, bool locked);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void updateDSPFromParams();

    juce::AudioProcessorValueTreeState apvts;
    juce::ValueTree locksState { "Locks" };

    GranularDelay granular;
    ShimmerReverb shimmer;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> wetHP;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> wetLP;
    juce::dsp::Limiter<float> limiter;

    juce::AudioBuffer<float> wetBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarlightDriftAudioProcessor)
};

