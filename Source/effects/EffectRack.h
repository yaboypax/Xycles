#pragma once
#include "../effects/Delay.h"
#include "../effects/Granulator.h"
#include "../effects/Reverb.h"
#include <JuceHeader.h>

class EffectRack : public juce::Component {
public:
  EffectRack();
  void paint(juce::Graphics &) override {}
  void resized() override;

  void setEngine(rust_part::Engine *engine);
  void setTheme(const Theme theme);
  void setColor(const juce::Colour color);

  std::function<void()> playbackCallback;

private:
  Granulator m_granulator;
  juce::TextButton m_granulatorButton;

  Freeverb m_reverb;
  juce::TextButton m_reverbButton;

  HDelay m_delay;
  juce::TextButton m_delayButton;
};
