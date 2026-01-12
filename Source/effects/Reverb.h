#pragma once

#include "../base/EffectComponent.h"
#include "../track/TrackKnob.h"
#include <JuceHeader.h>

class Freeverb : public EffectComponent {
public:
  Freeverb();
  void resized() override;
  void setTheme(const Theme theme) override;

private:
  void layoutSliders() override;
  void loadTheme() override;

  juce::Label m_reverbLabel;
  TrackKnob m_reverbSize, m_reverbAmount, m_reverbDamp;
  juce::Label m_reverbSizeLabel, m_reverbAmountLabel, m_reverbDampLabel;
};
