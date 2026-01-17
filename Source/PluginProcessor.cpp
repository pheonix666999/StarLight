#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace ParamIDs
{
    static constexpr auto inputGain = "inputGain";
    static constexpr auto delayTimeMs = "delayTimeMs";
    static constexpr auto feedback = "feedback";
    static constexpr auto grainSizeMs = "grainSizeMs";
    static constexpr auto density = "density";
    static constexpr auto jitter = "jitter";
    static constexpr auto pitchSemi = "pitchSemi";
    static constexpr auto spread = "spread";

    static constexpr auto reverbSize = "reverbSize";
    static constexpr auto preDelayMs = "preDelayMs";
    static constexpr auto tone = "tone";
    static constexpr auto shimmerAmt = "shimmerAmt";
    static constexpr auto shimmerPitch = "shimmerPitch";
    static constexpr auto reverbMix = "reverbMix";

    static constexpr auto mix = "mix";
    static constexpr auto outputGain = "outputGain";
    static constexpr auto hpEnable = "hpEnable";
    static constexpr auto hpFreq = "hpFreq";
    static constexpr auto lpEnable = "lpEnable";
    static constexpr auto lpFreq = "lpFreq";

    static constexpr auto drift = "drift";
    static constexpr auto modRate = "modRate";
    static constexpr auto modDepth = "modDepth";
    static constexpr auto freeze = "freeze";

    static constexpr auto air = "air";
    static constexpr auto glass = "glass";
}

static float dbToLin (float db) { return juce::Decibels::decibelsToGain (db); }

StarlightDriftAudioProcessor::StarlightDriftAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout StarlightDriftAudioProcessor::createParameterLayout()
{
    using namespace juce;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::inputGain, "Input", NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::delayTimeMs, "Time", NormalisableRange<float> (1.0f, 2000.0f, 0.01f, 0.35f), 450.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::feedback, "Feedback", NormalisableRange<float> (0.0f, 0.95f, 0.0001f), 0.35f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::grainSizeMs, "Size", NormalisableRange<float> (10.0f, 250.0f, 0.01f, 0.5f), 70.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::density, "Density", NormalisableRange<float> (0.2f, 40.0f, 0.01f, 0.5f), 40.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::jitter, "Jitter", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.15f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::pitchSemi, "Pitch", NormalisableRange<float> (-12.0f, 12.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::spread, "Spread", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.35f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::reverbSize, "Reverb Size", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.55f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::preDelayMs, "PreDelay", NormalisableRange<float> (0.0f, 250.0f, 0.01f, 0.5f), 20.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::tone, "Tone", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.55f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::shimmerAmt, "Shimmer", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.25f));
    params.push_back (std::make_unique<AudioParameterChoice> (ParamIDs::shimmerPitch, "Shimmer Pitch", StringArray { "+5", "+7", "+12", "+24" }, 2));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::reverbMix, "Reverb Mix", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 1.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::mix, "Mix", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::outputGain, "Output", NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<AudioParameterBool> (ParamIDs::hpEnable, "HP Enable", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::hpFreq, "HP Freq", NormalisableRange<float> (20.0f, 20000.0f, 0.01f, 0.5f), 120.0f));
    params.push_back (std::make_unique<AudioParameterBool> (ParamIDs::lpEnable, "LP Enable", false));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::lpFreq, "LP Freq", NormalisableRange<float> (20.0f, 20000.0f, 0.01f, 0.5f), 14000.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::drift, "Drift", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.25f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::modRate, "Mod Rate", NormalisableRange<float> (0.01f, 4.0f, 0.0001f, 0.5f), 0.35f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::modDepth, "Mod Depth", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.25f));
    params.push_back (std::make_unique<AudioParameterBool> (ParamIDs::freeze, "Freeze", false));

    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::air, "Air", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (ParamIDs::glass, "Glass", NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.0f));

    return { params.begin(), params.end() };
}

void StarlightDriftAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = 2;

    granular.prepare (spec);
    shimmer.prepare (spec);

    limiter.prepare (spec);
    limiter.setThreshold (-0.5f);

    stereoBuffer.setSize (2, samplesPerBlock);
    wetBuffer.setSize (2, samplesPerBlock);
    lastBuffer.setSize (2, samplesPerBlock);

    wetHP.prepare (spec);
    wetLP.prepare (spec);

    updateDSPFromParams();
}

void StarlightDriftAudioProcessor::releaseResources() {}

bool StarlightDriftAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();

    if (in.isDisabled() || out.isDisabled())
        return false;

    if (in != out)
        return false;

    return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
}

double StarlightDriftAudioProcessor::getTailLengthSeconds() const
{
    return 20.0;
}

bool StarlightDriftAudioProcessor::isParamLocked (const juce::String& paramId) const
{
    return locksState.getProperty (paramId, false);
}

void StarlightDriftAudioProcessor::setParamLocked (const juce::String& paramId, bool locked)
{
    locksState.setProperty (paramId, locked, nullptr);
}

void StarlightDriftAudioProcessor::copyLastBuffer (juce::AudioBuffer<float>& dest) const
{
    const juce::SpinLock::ScopedLockType sl (lastBufferLock);
    dest.makeCopyOf (lastBuffer, true);
}

void StarlightDriftAudioProcessor::updateDSPFromParams()
{
    const auto inputDb = apvts.getRawParameterValue (ParamIDs::inputGain)->load();
    const auto delayTimeMs = apvts.getRawParameterValue (ParamIDs::delayTimeMs)->load();
    const auto feedback = apvts.getRawParameterValue (ParamIDs::feedback)->load();
    const auto grainSizeMs = apvts.getRawParameterValue (ParamIDs::grainSizeMs)->load();
    const auto density = apvts.getRawParameterValue (ParamIDs::density)->load();
    const auto jitter = apvts.getRawParameterValue (ParamIDs::jitter)->load();
    const auto pitchSemi = apvts.getRawParameterValue (ParamIDs::pitchSemi)->load();
    const auto spread = apvts.getRawParameterValue (ParamIDs::spread)->load();

    const auto reverbSize = apvts.getRawParameterValue (ParamIDs::reverbSize)->load();
    const auto preDelayMs = apvts.getRawParameterValue (ParamIDs::preDelayMs)->load();
    const auto tone = apvts.getRawParameterValue (ParamIDs::tone)->load();
    const auto shimmerAmt = apvts.getRawParameterValue (ParamIDs::shimmerAmt)->load();
    const auto shimmerPitchChoice = (int) apvts.getRawParameterValue (ParamIDs::shimmerPitch)->load();
    const auto reverbMix = apvts.getRawParameterValue (ParamIDs::reverbMix)->load();

    const auto drift = apvts.getRawParameterValue (ParamIDs::drift)->load();
    const auto modRate = apvts.getRawParameterValue (ParamIDs::modRate)->load();
    const auto modDepth = apvts.getRawParameterValue (ParamIDs::modDepth)->load();
    const auto freeze = apvts.getRawParameterValue (ParamIDs::freeze)->load() > 0.5f;

    const auto air = apvts.getRawParameterValue (ParamIDs::air)->load();
    const auto glass = apvts.getRawParameterValue (ParamIDs::glass)->load();

    const auto densityEff = isParamLocked (ParamIDs::density) ? density
                            : density * (1.0f + 0.8f * air);
    const auto toneEff = isParamLocked (ParamIDs::tone) ? tone
                         : juce::jlimit (0.0f, 1.0f, tone + 0.25f * air);
    const auto shimmerAmtEff = isParamLocked (ParamIDs::shimmerAmt) ? shimmerAmt
                              : juce::jlimit (0.0f, 1.0f, shimmerAmt + 0.35f * air);
    const auto grainSizeEff = isParamLocked (ParamIDs::grainSizeMs) ? grainSizeMs
                              : grainSizeMs * (1.0f - 0.35f * glass);
    const auto spreadEff = isParamLocked (ParamIDs::spread) ? spread
                         : juce::jlimit (0.0f, 1.0f, spread + 0.5f * glass);
    const auto pitchSemiEff = isParamLocked (ParamIDs::pitchSemi) ? pitchSemi
                            : pitchSemi + 2.0f * glass;

    GranularDelay::Params g;
    g.inputGain = dbToLin (inputDb);
    g.delayTimeMs = delayTimeMs;
    g.feedback = feedback;
    g.grainSizeMs = grainSizeEff;
    g.density = densityEff;
    g.jitter = jitter;
    g.pitchSemitones = pitchSemiEff;
    g.spread = spreadEff;
    g.drift = drift;
    g.modRateHz = modRate;
    g.modDepth = modDepth;
    g.freeze = freeze;

    granular.setParams (g);

    ShimmerReverb::Params r;
    r.roomSize = reverbSize;
    r.preDelayMs = preDelayMs;
    r.tone = toneEff;
    r.shimmerAmount = shimmerAmtEff;
    r.reverbMix = reverbMix;
    r.drift = drift;
    r.modRateHz = modRate;
    r.modDepth = modDepth;
    r.freeze = freeze;
    const float baseShimmerPitch = (shimmerPitchChoice == 0 ? 5.0f
                      : shimmerPitchChoice == 1 ? 7.0f
                      : shimmerPitchChoice == 2 ? 12.0f
                                               : 24.0f);
    r.pitchSemitones = baseShimmerPitch + (glass * 12.0f);

    shimmer.setParams (r);

    const bool hpEnabled = apvts.getRawParameterValue (ParamIDs::hpEnable)->load() > 0.5f;
    const bool lpEnabled = apvts.getRawParameterValue (ParamIDs::lpEnable)->load() > 0.5f;
    const float hpFreq = apvts.getRawParameterValue (ParamIDs::hpFreq)->load();
    const float lpFreq = apvts.getRawParameterValue (ParamIDs::lpFreq)->load();

    if (hpEnabled)
    {
        *wetHP.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), hpFreq);
    }

    if (lpEnabled)
    {
        *wetLP.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (getSampleRate(), lpFreq);
    }
}

void StarlightDriftAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    if (numSamples <= 0)
        return;

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    stereoBuffer.setSize (2, numSamples, false, false, true);
    stereoBuffer.clear();

    if (totalNumInputChannels > 0)
    {
        stereoBuffer.copyFrom (0, 0, buffer, 0, 0, numSamples);

        if (totalNumInputChannels > 1)
            stereoBuffer.copyFrom (1, 0, buffer, 1, 0, numSamples);
        else
            stereoBuffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }

    // Capture input for waveform display (always stereo for the UI)
    {
        const juce::SpinLock::ScopedLockType sl (lastBufferLock);
        lastBuffer.setSize (2, numSamples, false, false, true);
        lastBuffer.makeCopyOf (stereoBuffer, true);
    }

    updateDSPFromParams();

    wetBuffer.setSize (2, numSamples, false, false, true);
    wetBuffer.clear();

    granular.process (stereoBuffer, wetBuffer);
    shimmer.process (wetBuffer);

    const bool hpEnabled = apvts.getRawParameterValue (ParamIDs::hpEnable)->load() > 0.5f;
    const bool lpEnabled = apvts.getRawParameterValue (ParamIDs::lpEnable)->load() > 0.5f;

    juce::dsp::AudioBlock<float> wetBlock (wetBuffer);
    if (hpEnabled)
    {
        wetHP.process (juce::dsp::ProcessContextReplacing<float> (wetBlock));
    }

    if (lpEnabled)
    {
        wetLP.process (juce::dsp::ProcessContextReplacing<float> (wetBlock));
    }

    const float mix = apvts.getRawParameterValue (ParamIDs::mix)->load();
    const float outGain = dbToLin (apvts.getRawParameterValue (ParamIDs::outputGain)->load());

    for (int ch = 0; ch < 2; ++ch)
    {
        auto* dry = stereoBuffer.getWritePointer (ch);
        auto* wet = wetBuffer.getReadPointer (ch);

        for (int i = 0; i < numSamples; ++i)
            dry[i] = (1.0f - mix) * dry[i] + mix * wet[i];
    }

    stereoBuffer.applyGain (outGain);

    juce::dsp::AudioBlock<float> block (stereoBuffer);
    limiter.process (juce::dsp::ProcessContextReplacing<float> (block));

    if (totalNumOutputChannels == 1)
    {
        buffer.copyFrom (0, 0, stereoBuffer, 0, 0, numSamples);
        buffer.addFrom (0, 0, stereoBuffer, 1, 0, numSamples, 1.0f);
        buffer.applyGain (0.5f);
        return;
    }

    buffer.copyFrom (0, 0, stereoBuffer, 0, 0, numSamples);
    buffer.copyFrom (1, 0, stereoBuffer, 1, 0, numSamples);
}

juce::AudioProcessorEditor* StarlightDriftAudioProcessor::createEditor()
{
    return new StarlightDriftAudioProcessorEditor (*this);
}

void StarlightDriftAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree state ("StarlightDriftState");
    state.appendChild (apvts.copyState(), nullptr);
    state.appendChild (locksState, nullptr);

    juce::MemoryOutputStream stream (destData, false);
    state.writeToStream (stream);
}

void StarlightDriftAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto state = juce::ValueTree::readFromData (data, (size_t) sizeInBytes);
    if (! state.isValid())
        return;

    const auto params = state.getChildWithName ("PARAMS");
    if (params.isValid())
        apvts.replaceState (params);

    const auto locks = state.getChildWithName ("Locks");
    if (locks.isValid())
        locksState = locks;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarlightDriftAudioProcessor();
}
