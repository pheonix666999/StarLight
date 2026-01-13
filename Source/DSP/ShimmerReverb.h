#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class ShimmerReverb final
{
public:
    struct Params
    {
        float roomSize = 0.55f;
        float preDelayMs = 20.0f;
        float tone = 0.55f;
        float shimmerAmount = 0.25f;
        float pitchSemitones = 12.0f;
        float reverbMix = 1.0f;

        float drift = 0.25f;
        float modRateHz = 0.35f;
        float modDepth = 0.25f;
        bool freeze = false;
    };

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        preDelay.reset();
        preDelay.prepare (spec);

        reverb.reset();

        pitchL.prepare (spec);
        pitchR.prepare (spec);
        // feedbackBuffer not used - tmpBuffer handles the pitched feedback
        tmpBuffer.setSize (2, (int) spec.maximumBlockSize);
        lastReverbOut.setSize (2, (int) spec.maximumBlockSize);
        lastReverbOut.clear();
    }

    void setParams (const Params& p) { params = p; }

    void process (juce::AudioBuffer<float>& wetInOut)
    {
        const int numSamples = wetInOut.getNumSamples();
        if (numSamples <= 0) return;

        juce::Reverb::Parameters rp;
        rp.roomSize = juce::jlimit (0.0f, 1.0f, params.roomSize);
        rp.damping = juce::jlimit (0.0f, 1.0f, 1.0f - params.tone);
        rp.wetLevel = params.reverbMix;
        rp.dryLevel = 0.0f;
        rp.width = 0.90f;
        rp.freezeMode = params.freeze ? 1.0f : 0.0f;
        reverb.setParameters (rp);

        preDelay.setDelay ((params.preDelayMs / 1000.0f) * (float) sampleRate);

        tmpBuffer.setSize (2, numSamples, false, false, true);
        tmpBuffer.clear();

        // pitch shift last block's reverb output and feed it back in (shimmer topology approximation)
        tmpBuffer.makeCopyOf (lastReverbOut);

        const float pitchSemi = params.pitchSemitones;
        const float pitchFactor = std::pow (2.0f, pitchSemi / 12.0f);
        pitchL.setPitchFactor (pitchFactor);
        pitchR.setPitchFactor (pitchFactor);

        pitchL.process (tmpBuffer.getWritePointer (0), numSamples);
        pitchR.process (tmpBuffer.getWritePointer (1), numSamples);

        const float shimmer = juce::jlimit (0.0f, 1.0f, params.shimmerAmount);
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* w = wetInOut.getWritePointer (ch);
            auto* p = tmpBuffer.getReadPointer (ch);
            for (int i = 0; i < numSamples; ++i)
                w[i] = w[i] + (0.65f * shimmer) * p[i];
        }

        // predelay then reverb
        juce::dsp::AudioBlock<float> block (wetInOut);
        preDelay.process (juce::dsp::ProcessContextReplacing<float> (block));
        reverb.processStereo (wetInOut.getWritePointer (0), wetInOut.getWritePointer (1), numSamples);

        lastReverbOut.makeCopyOf (wetInOut);
    }

private:
    class DualWindowPitchShifter
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec)
        {
            sampleRate = spec.sampleRate;
            const int maxDelay = (int) (sampleRate * 0.08); // 80ms window
            delay.setSize (1, maxDelay * 2);
            writePos = 0;
            phase = 0.0f;
            setPitchFactor (1.0f);
        }

        void setPitchFactor (float f)
        {
            pitchFactor = juce::jlimit (0.5f, 2.0f, f);
        }

        void process (float* samples, int numSamples)
        {
            const int size = delay.getNumSamples();
            const float window = 0.05f; // 50ms
            const float windowSamples = window * (float) sampleRate;

            const float rate = (pitchFactor - 1.0f) / windowSamples;

            for (int i = 0; i < numSamples; ++i)
            {
                delay.setSample (0, writePos, samples[i]);

                phase += rate;
                if (phase >= 1.0f) phase -= 1.0f;
                if (phase < 0.0f) phase += 1.0f;

                const float p1 = phase;
                const float p2 = std::fmod (phase + 0.5f, 1.0f);

                const float d1 = p1 * windowSamples;
                const float d2 = p2 * windowSamples;

                const float s1 = readDelay ((float) writePos - d1, size);
                const float s2 = readDelay ((float) writePos - d2, size);

                const float w1 = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * p1);
                const float w2 = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * p2);

                samples[i] = (s1 * w1 + s2 * w2) / (w1 + w2 + 1.0e-6f);

                writePos = (writePos + 1) % size;
            }
        }

    private:
        static float wrap (float x, float size)
        {
            while (x < 0.0f) x += size;
            while (x >= size) x -= size;
            return x;
        }

        float readDelay (float pos, int size) const
        {
            pos = wrap (pos, (float) size);
            const int i0 = (int) pos;
            const int i1 = (i0 + 1) % size;
            const float frac = pos - (float) i0;
            const float a = delay.getSample (0, i0);
            const float b = delay.getSample (0, i1);
            return a + frac * (b - a);
        }

        double sampleRate = 48000.0;
        float pitchFactor = 1.0f;
        juce::AudioBuffer<float> delay;
        int writePos = 0;
        float phase = 0.0f;
    };

    double sampleRate = 48000.0;
    Params params;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelay { 200000 };
    juce::Reverb reverb;

    DualWindowPitchShifter pitchL, pitchR;
    juce::AudioBuffer<float> tmpBuffer, lastReverbOut;
};
