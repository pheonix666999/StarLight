#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class GranularDelay final
{
public:
    struct Params
    {
        float inputGain = 1.0f;
        float delayTimeMs = 450.0f;
        float feedback = 0.35f;
        float grainSizeMs = 70.0f;
        float density = 8.0f;
        float jitter = 0.15f;
        float pitchSemitones = 0.0f;
        float spread = 0.35f;

        float drift = 0.25f;
        float modRateHz = 0.35f;
        float modDepth = 0.25f;
        bool freeze = false;
    };

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        const int maxDelaySamples = (int) juce::jmax (1.0, sampleRate * 4.0); // 4 seconds
        for (auto& b : delayBuffer)
            b.setSize (1, maxDelaySamples);

        writePos = 0;
        rng.setSeedRandomly();

        activeGrains.clear();
        activeGrains.reserve (128);

        spawnAccumulator = 0.0;
        driftReadOffset = 0.0f;
        driftDetune = 0.0f;
    }

    void setParams (const Params& p) { params = p; }

    void process (juce::AudioBuffer<float>& dryInOut, juce::AudioBuffer<float>& wetOut)
    {
        const int numSamples = dryInOut.getNumSamples();
        const int maxDelay = delayBuffer[0].getNumSamples();
        if (maxDelay <= 1 || sampleRate <= 0.0)
            return;

        const float inputGain = params.inputGain;

        auto* inL = dryInOut.getReadPointer (0);
        auto* inR = dryInOut.getReadPointer (1);
        auto* wetL = wetOut.getWritePointer (0);
        auto* wetR = wetOut.getWritePointer (1);

        const float baseDelaySamples = (params.delayTimeMs / 1000.0f) * (float) sampleRate;

        const float basePitch = std::pow (2.0f, params.pitchSemitones / 12.0f);
        const float density = juce::jmax (0.001f, params.density);
        const float grainSizeMs = juce::jlimit (10.0f, 250.0f, params.grainSizeMs);
        const int grainSamples = (int) juce::jmax (8.0, (grainSizeMs / 1000.0f) * sampleRate);

        const float drift = params.drift;
        const float modRate = params.modRateHz;
        const float modDepth = params.modDepth;

        const float jitterSamplesMax = (params.jitter + 0.2f * drift) * (float) grainSamples;
        const float spread = juce::jlimit (0.0f, 1.0f, params.spread);

        const float feedback = params.freeze ? 0.985f : params.feedback;

        const float driftStep = (0.00002f + 0.0002f * modRate) * drift;
        const float detuneStep = (0.000001f + 0.00002f * modRate) * drift;

        for (int i = 0; i < numSamples; ++i)
        {
            // non-LFO "drift": random walk, very slow
            driftReadOffset = juce::jlimit (-1.0f, 1.0f, driftReadOffset + driftStep * (rng.nextFloat() * 2.0f - 1.0f));
            driftDetune = juce::jlimit (-1.0f, 1.0f, driftDetune + detuneStep * (rng.nextFloat() * 2.0f - 1.0f));

            const float inSampleL = inL[i] * inputGain;
            const float inSampleR = inR[i] * inputGain;
            const float inMono = 0.5f * (inSampleL + inSampleR);

            // write (unless frozen)
            if (! params.freeze)
            {
                const float fb = feedbackSample;
                delayBuffer[0].setSample (0, writePos, inMono + fb * feedback);
            }

            // spawn grains
            spawnAccumulator += density / sampleRate;
            while (spawnAccumulator >= 1.0)
            {
                spawnAccumulator -= 1.0;

                Grain g;
                g.length = grainSamples;
                g.age = 0;

                const float jitter = (rng.nextFloat() * 2.0f - 1.0f) * jitterSamplesMax;
                const float driftOffset = driftReadOffset * (modDepth * 0.15f) * (float) grainSamples;
                const float readPos = (float) writePos - baseDelaySamples + jitter + driftOffset;
                g.readPos = wrapRead (readPos, (float) maxDelay);

                const float detune = (rng.nextFloat() * 2.0f - 1.0f) * (0.02f * drift * modDepth) + driftDetune * (0.04f * drift * modDepth);
                g.readInc = basePitch * std::pow (2.0f, detune);

                const float pan = (rng.nextFloat() * 2.0f - 1.0f) * spread;
                g.panL = juce::jlimit (0.0f, 1.0f, 0.5f - 0.5f * pan);
                g.panR = juce::jlimit (0.0f, 1.0f, 0.5f + 0.5f * pan);

                activeGrains.push_back (g);
            }

            float outL = 0.0f, outR = 0.0f;
            feedbackSample = 0.0f;

            for (int gi = (int) activeGrains.size() - 1; gi >= 0; --gi)
            {
                auto& g = activeGrains[(size_t) gi];
                if (g.age >= g.length)
                {
                    activeGrains.erase (activeGrains.begin() + gi);
                    continue;
                }

                const float s = readDelay (g.readPos, maxDelay);
                const float w = hann (g.age, g.length);
                const float v = s * w;

                outL += v * g.panL;
                outR += v * g.panR;
                feedbackSample += v * 0.5f;

                g.readPos = wrapRead (g.readPos + g.readInc, (float) maxDelay);
                ++g.age;
            }

            wetL[i] = outL;
            wetR[i] = outR;

            writePos = (writePos + 1) % maxDelay;
        }
    }

private:
    struct Grain
    {
        int age = 0;
        int length = 0;
        float readPos = 0.0f;
        float readInc = 1.0f;
        float panL = 0.5f;
        float panR = 0.5f;
    };

    static float hann (int pos, int len)
    {
        if (len <= 1) return 1.0f;
        const float x = (float) pos / (float) (len - 1);
        return 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * x);
    }

    static float wrapRead (float p, float size)
    {
        while (p < 0.0f) p += size;
        while (p >= size) p -= size;
        return p;
    }

    float readDelay (float pos, int size) const
    {
        const int i0 = (int) pos;
        const int i1 = (i0 + 1) % size;
        const float frac = pos - (float) i0;
        const float a = delayBuffer[0].getSample (0, i0);
        const float b = delayBuffer[0].getSample (0, i1);
        return a + frac * (b - a);
    }

    double sampleRate = 48000.0;
    Params params;

    juce::AudioBuffer<float> delayBuffer[1];
    int writePos = 0;

    double spawnAccumulator = 0.0;
    float feedbackSample = 0.0f;

    float driftReadOffset = 0.0f;
    float driftDetune = 0.0f;

    juce::Random rng;
    std::vector<Grain> activeGrains;
};
