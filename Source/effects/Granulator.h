#pragma once

#include "../base/EffectComponent.h"
#include "../track/TrackKnob.h"
#include <JuceHeader.h>
#include <rust_part.h>

class Granulator : public EffectComponent {
public:
  Granulator();
  void resized() override;
  void setTheme(const Theme theme) override;

private:
  void layoutSliders() override;
  void loadTheme() override;

  juce::Label m_granulatorLabel;
  TrackKnob m_grainSpeed, m_grainLength, m_grainOverlap, m_grainsCount,
      m_grainSpread;
  juce::Label m_grainSpeedLabel, m_grainLengthLabel, m_grainOverlapLabel,
      m_grainsCountLabel, m_grainSpreadLabel;
};
