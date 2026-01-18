#pragma once

#include "../base/EffectComponent.h"
#include "../track/TrackKnob.h"
#include <JuceHeader.h>

class Compressor : public EffectComponent {
public:
  Compressor();
  void resized() override;
  void setTheme(const Theme theme) override;

private:
  void layoutSliders() override;
  void loadTheme() override;

  juce::Label m_compressorLabel;
  TrackKnob m_compressorThreshold, m_compressorAttack, m_compressorRelease;
  juce::Label m_compressorThresholdLabel, m_compressorAttackLabel,
      m_compressorReleaseLabel;
};
