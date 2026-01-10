#pragma once

#include "../track/TrackKnob.h"
#include "juce_graphics/juce_graphics.h"
#include "rust_part.h"
#include <JuceHeader.h>

class Granulator : public juce::Component {
public:
  Granulator();
  void paint(juce::Graphics &) override {}
  void resized() override;
  void setTheme(const Theme theme);
  void setEngine(rust_part::Engine *engine);
  void setColor(const juce::Colour color);

private:
  void layoutSliders();
  void loadTheme();

  rust_part::Engine *m_engine;
  Theme m_theme = Theme::Light;
  juce::Colour m_color;

  juce::Label m_granulatorLabel;
  TrackKnob m_grainSpeed, m_grainLength, m_grainOverlap, m_grainsCount,
      m_grainSpread;
  juce::Label m_grainSpeedLabel, m_grainLengthLabel, m_grainOverlapLabel,
      m_grainsCountLabel, m_grainSpreadLabel;
};
