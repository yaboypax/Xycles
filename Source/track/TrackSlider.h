//
// Created by R&F Dev on 7/20/25.
//

#pragma once
#include "../static/Utilities.h"
#include <JuceHeader.h>

class TrackSlider : public juce::Slider {
public:
  TrackSlider();
  void setTrackColor(juce::Colour colour);
  void setTrackDirection(TrackDirection direction);
  void setEndPosition(double endValue);
  void setStartPosition(double startValue);

  void paint(juce::Graphics &g) override;
  void setTheme(Theme theme);

private:
  TrackDirection m_trackDirection = Start;
  Theme m_theme = Light;

  juce::Colour m_trackColor = juce::Colours::black;
  double m_startPosition = 0.0;
  double m_endPosition = 1.0;
};
