//
// Created by R&F Dev on 7/20/25.
//

#pragma once
#include "../static/Utilities.h"
#include <JuceHeader.h>

class TrackKnob : public juce::Slider {
public:
  TrackKnob();
  void setTrackColor(juce::Colour color);
  void paint(juce::Graphics &g) override;
  void setTheme(Theme theme);

private:
  juce::Colour m_trackColor = juce::Colours::black;
  Theme m_theme = Light;
};
