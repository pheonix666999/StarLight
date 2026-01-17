#include "WaveformComponent.h"

WaveformComponent::WaveformComponent()
{
    startTimerHz (30); // Update at 30 FPS
}

WaveformComponent::~WaveformComponent()
{
    stopTimer();
}

void WaveformComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour (juce::Colours::black.withAlpha (0.1f));
    g.fillRoundedRectangle (bounds, 8.0f);

    // Border
    g.setColour (juce::Colour::fromRGB (255, 215, 0).withAlpha (0.5f)); // Gold
    g.drawRoundedRectangle (bounds, 8.0f, 2.0f);

    juce::ScopedLock sl (bufferLock);
    if (displayBuffer.getNumSamples() > 0)
    {
        const int numSamples = displayBuffer.getNumSamples();
        const int numCh = displayBuffer.getNumChannels();
        const float* left = numCh > 0 ? displayBuffer.getReadPointer (0) : nullptr;
        const float* right = numCh > 1 ? displayBuffer.getReadPointer (1) : left;
        if (left == nullptr)
            return;

        juce::Path path;
        path.startNewSubPath (0, bounds.getCentreY());

        for (int i = 0; i < numSamples; ++i)
        {
            float x = (float) i / (float) numSamples * bounds.getWidth();
            float y = bounds.getCentreY() - (left[i] + right[i]) * 0.5f * bounds.getHeight() * 0.4f;
            path.lineTo (x, y);
        }

        // Glow effect
        g.setColour (juce::Colour::fromRGB (0, 255, 255).withAlpha (0.8f)); // Cyan
        g.strokePath (path, juce::PathStrokeType (2.0f));

        g.setColour (juce::Colour::fromRGB (0, 255, 255).withAlpha (0.3f));
        g.strokePath (path, juce::PathStrokeType (4.0f));
    }
}

void WaveformComponent::resized()
{
    // Nothing to resize
}

void WaveformComponent::setBuffer (const juce::AudioBuffer<float>& buffer)
{
    juce::ScopedLock sl (bufferLock);
    displayBuffer.makeCopyOf (buffer);
}

void WaveformComponent::timerCallback()
{
    repaint();
}
