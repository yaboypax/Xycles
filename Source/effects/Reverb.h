#pragma once

#include "../track/TrackKnob.h"
#include "juce_graphics/juce_graphics.h"
#include "rust_part.h"
#include <JuceHeader.h>

class Freeverb : public juce::Component {
public:
  Freeverb();
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

  juce::Label m_reverbLabel;
  TrackKnob m_reverbSize, m_reverbAmount, m_reverbDamp;
  juce::Label m_reverbSizeLabel, m_reverbAmountLabel, m_reverbDampLabel;
};
