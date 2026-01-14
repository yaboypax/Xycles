#pragma once

#include "../base/EffectComponent.h"
#include "../track/TrackKnob.h"
#include <JuceHeader.h>

class HDelay : public EffectComponent {
public:
  HDelay();
  void resized() override;
  void setTheme(const Theme theme) override;

private:
  void layoutSliders() override;
  void loadTheme() override;

  juce::Label m_delayLabel;
  TrackKnob m_delayTime, m_delayFeedback, m_delayAmount;
  juce::Label m_delayTimeLabel, m_delayAmountLabel, m_delayFeedbackLabel;
};
