#include "PluginEditor.h"

// ... ParamIDs namespace (same as before, omitted for brevity if duplicate, but must include for full file) ...
// Actually better to include the namespace to ensure compiling.
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

StarlightDriftAudioProcessorEditor::StarlightDriftAudioProcessorEditor (StarlightDriftAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), apvts (p.getAPVTS()),
      drift (p, ParamIDs::drift, "DRIFT", LockableSlider::Style::Large),
      air (p, ParamIDs::air, "AIR", LockableSlider::Style::Small),
      glass (p, ParamIDs::glass, "GLASS", LockableSlider::Style::Small),
      freeze (p, ParamIDs::freeze, "FREEZE"),
      mix (p, ParamIDs::mix, "MIX", LockableSlider::Style::Small),
      output (p, ParamIDs::outputGain, "OUTPUT", LockableSlider::Style::Small),
      hpEnable (p, ParamIDs::hpEnable, "HP FILTER"),
      hpFreq (p, ParamIDs::hpFreq, "HP FREQ", LockableSlider::Style::Tiny),
      lpEnable (p, ParamIDs::lpEnable, "LP FILTER"),
      lpFreq (p, ParamIDs::lpFreq, "LP FREQ", LockableSlider::Style::Tiny),
      inputGain (p, ParamIDs::inputGain, "INPUT", LockableSlider::Style::Tiny),
      timeMs (p, ParamIDs::delayTimeMs, "TIME", LockableSlider::Style::Tiny),
      feedback (p, ParamIDs::feedback, "FEEDBACK", LockableSlider::Style::Tiny),
      grainSize (p, ParamIDs::grainSizeMs, "GRAIN SIZE", LockableSlider::Style::Tiny),
      density (p, ParamIDs::density, "DENSITY", LockableSlider::Style::Tiny),
      jitter (p, ParamIDs::jitter, "JITTER", LockableSlider::Style::Tiny),
      pitch (p, ParamIDs::pitchSemi, "PITCH", LockableSlider::Style::Tiny),
      spread (p, ParamIDs::spread, "SPREAD", LockableSlider::Style::Tiny),
      reverbSize (p, ParamIDs::reverbSize, "SIZE", LockableSlider::Style::Tiny),
      preDelay (p, ParamIDs::preDelayMs, "PRE-DLY", LockableSlider::Style::Tiny),
      tone (p, ParamIDs::tone, "TONE", LockableSlider::Style::Tiny),
      shimmerAmt (p, ParamIDs::shimmerAmt, "SHIM AMT", LockableSlider::Style::Tiny),
      reverbMix (p, ParamIDs::reverbMix, "VERB MIX", LockableSlider::Style::Tiny),
      modRate (p, ParamIDs::modRate, "MOD RATE", LockableSlider::Style::Tiny),
      modDepth (p, ParamIDs::modDepth, "MOD DEPTH", LockableSlider::Style::Tiny)
{
    setLookAndFeel (&lnf);

    for (auto* c : { &drift, &air, &glass, &mix, &output, &hpFreq, &lpFreq,
                     &inputGain, &timeMs, &feedback, &grainSize, &density, &jitter, &pitch, &spread,
                     &reverbSize, &preDelay, &tone, &shimmerAmt, &reverbMix, &modRate, &modDepth })
        addAndMakeVisible (*c);

    addAndMakeVisible (freeze);
    addAndMakeVisible (hpEnable);
    addAndMakeVisible (lpEnable);

    shimmerPitch.addItem ("+5th", 1);
    shimmerPitch.addItem ("+7th", 2);
    shimmerPitch.addItem ("+1 Oct", 3);
    shimmerPitch.addItem ("+2 Oct", 4);
    shimmerPitch.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (shimmerPitch);

    // Attachments
    attDrift = std::make_unique<SliderAttachment> (apvts, ParamIDs::drift, drift);
    attAir = std::make_unique<SliderAttachment> (apvts, ParamIDs::air, air);
    attGlass = std::make_unique<SliderAttachment> (apvts, ParamIDs::glass, glass);
    attFreeze = std::make_unique<ButtonAttachment> (apvts, ParamIDs::freeze, freeze);

    attMix = std::make_unique<SliderAttachment> (apvts, ParamIDs::mix, mix);
    attOutput = std::make_unique<SliderAttachment> (apvts, ParamIDs::outputGain, output);

    attHpEnable = std::make_unique<ButtonAttachment> (apvts, ParamIDs::hpEnable, hpEnable);
    attLpEnable = std::make_unique<ButtonAttachment> (apvts, ParamIDs::lpEnable, lpEnable);
    attHpFreq = std::make_unique<SliderAttachment> (apvts, ParamIDs::hpFreq, hpFreq);
    attLpFreq = std::make_unique<SliderAttachment> (apvts, ParamIDs::lpFreq, lpFreq);

    attInputGain = std::make_unique<SliderAttachment> (apvts, ParamIDs::inputGain, inputGain);
    attTimeMs = std::make_unique<SliderAttachment> (apvts, ParamIDs::delayTimeMs, timeMs);
    attFeedback = std::make_unique<SliderAttachment> (apvts, ParamIDs::feedback, feedback);
    attGrainSize = std::make_unique<SliderAttachment> (apvts, ParamIDs::grainSizeMs, grainSize);
    attDensity = std::make_unique<SliderAttachment> (apvts, ParamIDs::density, density);
    attJitter = std::make_unique<SliderAttachment> (apvts, ParamIDs::jitter, jitter);
    attPitch = std::make_unique<SliderAttachment> (apvts, ParamIDs::pitchSemi, pitch);
    attSpread = std::make_unique<SliderAttachment> (apvts, ParamIDs::spread, spread);

    attReverbSize = std::make_unique<SliderAttachment> (apvts, ParamIDs::reverbSize, reverbSize);
    attPreDelay = std::make_unique<SliderAttachment> (apvts, ParamIDs::preDelayMs, preDelay);
    attTone = std::make_unique<SliderAttachment> (apvts, ParamIDs::tone, tone);
    attShimmerAmt = std::make_unique<SliderAttachment> (apvts, ParamIDs::shimmerAmt, shimmerAmt);
    attShimmerPitch = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::shimmerPitch, shimmerPitch);
    attReverbMix = std::make_unique<SliderAttachment> (apvts, ParamIDs::reverbMix, reverbMix);

    attModRate = std::make_unique<SliderAttachment> (apvts, ParamIDs::modRate, modRate);
    attModDepth = std::make_unique<SliderAttachment> (apvts, ParamIDs::modDepth, modDepth);

    // Callbacks
    auto refreshFilters = [this]
    {
        hpFreq.setEnabled (hpEnable.getToggleState());
        lpFreq.setEnabled (lpEnable.getToggleState());
    };
    hpEnable.onClick = refreshFilters;
    lpEnable.onClick = refreshFilters;
    refreshFilters();
    
    // Set Double Click Defaults
    auto setDefault = [this] (LockableSlider& s)
    {
        if (auto* param = apvts.getParameter (s.getParamId()))
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
                s.setDoubleClickReturnValue (true, ranged->convertFrom0to1 (param->getDefaultValue()));
    };
    for (auto* s : { &drift, &air, &glass, &mix, &output, &hpFreq, &lpFreq,
                     &inputGain, &timeMs, &feedback, &grainSize, &density, &jitter, &pitch, &spread,
                     &reverbSize, &preDelay, &tone, &shimmerAmt, &reverbMix, &modRate, &modDepth })
        setDefault (*s);

    setResizable (true, true);
    setResizeLimits (1000, 650, 1920, 1200);
    // Bigger starting size "Full Screen"-ish feel
    setSize (1400, 850);
}

StarlightDriftAudioProcessorEditor::~StarlightDriftAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void StarlightDriftAudioProcessorEditor::paint (juce::Graphics& g)
{
    lnf.drawStarfield(g, getLocalBounds());
    
    // Calculate Layout Areas (mirrors resized)
    auto area = getLocalBounds().reduced(24);
    area.removeFromTop(60); // Header
    auto topSection = area.removeFromTop(area.getHeight() * 0.42f);
    auto bottomSection = area.reduced(0, 16);
    
    // Header with glow effect
    auto headerArea = juce::Rectangle<int>(30, 20, 500, 50);
    
    // Glow shadow
    g.setColour(lnf.accGold.withAlpha(0.3f));
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("STARLIGHT DRIFT", headerArea.translated(2, 2), juce::Justification::left, true);
    
    // Main text
    g.setColour(lnf.txtWhite);
    g.drawText("STARLIGHT DRIFT", headerArea, juce::Justification::left, true);
    
    // Subtitle
    g.setColour(lnf.txtDim);
    g.setFont(juce::Font(12.0f, juce::Font::plain));
    g.drawText("Granular Delay → Shimmer Reverb", 30, 60, 400, 20, juce::Justification::left, true);
    
    // Draw Module Backgrounds
    int moduleSpacing = 20;
    int moduleW = (bottomSection.getWidth() - moduleSpacing * 2) / 3;
    
    auto box1 = bottomSection.removeFromLeft(moduleW);
    bottomSection.removeFromLeft(moduleSpacing);
    auto box2 = bottomSection.removeFromLeft(moduleW);
    bottomSection.removeFromLeft(moduleSpacing);
    auto box3 = bottomSection;
    
    lnf.drawModuleBackground(g, box1, "GRAIN ENGINE", lnf.accCyan);
    lnf.drawModuleBackground(g, box2, "MASTER & FILTERS", lnf.accOrange);
    lnf.drawModuleBackground(g, box3, "SHIMMER VERB", lnf.accPink);
}

void StarlightDriftAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    area.removeFromTop(60);

    auto topSection = area.removeFromTop(area.getHeight() * 0.42f);
    auto bottomSection = area.reduced(0, 16);

    // === TOP: Drift & Macros ===
    int centerX = topSection.getCentreX();
    int driftSize = juce::jmin(280, topSection.getHeight());
    drift.setBounds(centerX - driftSize/2, topSection.getCentreY() - driftSize/2, driftSize, driftSize);
    
    int sideSize = driftSize / 2;
    int offset = driftSize / 2 + 80;
    
    air.setBounds(centerX - offset - sideSize, topSection.getCentreY() - sideSize/2, sideSize, sideSize);
    glass.setBounds(centerX + offset, topSection.getCentreY() - sideSize/2, sideSize, sideSize);

    // === BOTTOM MODULES ===
    int moduleSpacing = 20;
    int moduleW = (bottomSection.getWidth() - moduleSpacing * 2) / 3;
    
    auto box1 = bottomSection.removeFromLeft(moduleW).reduced(16, 40);
    bottomSection.removeFromLeft(moduleSpacing);
    auto box2 = bottomSection.removeFromLeft(moduleW).reduced(16, 40);
    bottomSection.removeFromLeft(moduleSpacing);
    auto box3 = bottomSection.reduced(16, 40);
    
    // 1. Grains (Cyan Theme)
    layoutKnobGrid(box1, { &inputGain, &timeMs, &feedback, &grainSize, 
                           &density, &jitter, &pitch, &spread }, 2, 4);

    // 2. Global (Orange Theme)
    int rowH = box2.getHeight() / 4;
    freeze.setBounds(box2.removeFromTop(rowH).reduced(box2.getWidth()*0.15f, 10)); // Top Button
    
    auto f1 = box2.removeFromTop(rowH);
    hpEnable.setBounds(f1.removeFromLeft(f1.getWidth()/2).reduced(5, 12));
    hpFreq.setBounds(f1.reduced(5, 0));
    
    auto f2 = box2.removeFromTop(rowH);
    lpEnable.setBounds(f2.removeFromLeft(f2.getWidth()/2).reduced(5, 12));
    lpFreq.setBounds(f2.reduced(5, 0));
    
    auto mx = box2;
    mix.setBounds(mx.removeFromLeft(mx.getWidth()/2).reduced(5,0));
    output.setBounds(mx.reduced(5,0));
    
    // 3. Reverb (Pink Theme)
    // Row 1: Size/Pre/Tone/Amt
    int rH = box3.getHeight() / 3;
    layoutKnobGrid(box3.removeFromTop(rH), {&reverbSize, &preDelay, &tone, &shimmerAmt}, 1, 4);
    
    // Row 2: Pitch Combo / Verb Mix
    auto r2 = box3.removeFromTop(rH);
    shimmerPitch.setBounds(r2.removeFromLeft(r2.getWidth()/2).reduced(12, 18));
    reverbMix.setBounds(r2.reduced(10, 0));
    
    // Row 3: Modulators
    layoutKnobGrid(box3, {&modRate, &modDepth}, 1, 2);
}

void StarlightDriftAudioProcessorEditor::layoutKnobGrid (juce::Rectangle<int> area, std::initializer_list<juce::Component*> knobs, int rows, int cols)
{
    int cellW = area.getWidth() / cols;
    int cellH = area.getHeight() / rows;
    int k = 0;
    int knobSize = juce::jmin(cellW, cellH) - 10;
    
    for (auto* c : knobs)
    {
        if (c)
        {
            int r = k / cols;
            int col = k % cols;
            juce::Rectangle<int> cell(area.getX() + col*cellW, area.getY() + r*cellH, cellW, cellH);
            c->setBounds(cell.withSizeKeepingCentre(knobSize, knobSize));
        }
        k++;
    }
}
