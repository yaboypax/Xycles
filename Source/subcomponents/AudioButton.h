
#pragma once
#include "../static/Utilities.h"
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

namespace Xycles {

struct AudioButton : juce::TextButton {
  AudioButton() {
    setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    setColour(juce::TextButton::buttonColourId,
              juce::Colours::transparentWhite);
    setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
  }
  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override 
  {
    juce::TextButton::paintButton(g, shouldDrawButtonAsHighlighted,
                                  shouldDrawButtonAsDown);
    juce::Colour color;
    switch (m_theme) {
    case Theme::Light: {
      color = juce::Colours::black;
      break;
    }
    case Theme::Dark: {
      color = juce::Colours::white;
      break;
    }
    }
    g.setColour(shouldDrawButtonAsDown ? color.withMultipliedAlpha(0.8f)
                                       : color);
    auto bounds = getLocalBounds().reduced(10,10);
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();

    // 3 vertical slider tracks
    const float x1 = bounds.getX() + w * 0.25f;
    const float x2 = bounds.getX() + w * 0.50f;
    const float x3 = bounds.getX() + w * 0.75f;

    const float top = bounds.getY();
    const float bot = bounds.getBottom();

    const float trackThickness = juce::jmax(1.0f, juce::jmin(w, h) * 0.08f);
    const float knobRadius = juce::jmax(2.0f, juce::jmin(w, h) * 0.11f);

    auto stroke =
        juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved,
                             juce::PathStrokeType::rounded);

    juce::Path tracks;
    tracks.startNewSubPath(x1, top);
    tracks.lineTo(x1, bot);
    tracks.startNewSubPath(x2, top);
    tracks.lineTo(x2, bot);
    tracks.startNewSubPath(x3, top);
    tracks.lineTo(x3, bot);

    g.strokePath(tracks, stroke);

    const float k1 = top + h * 0.32f;
    const float k2 = top + h * 0.62f;
    const float k3 = top + h * 0.42f;

    g.fillEllipse(x1 - knobRadius, k1 - knobRadius, knobRadius * 2.0f,
                  knobRadius * 2.0f);
    g.fillEllipse(x2 - knobRadius, k2 - knobRadius, knobRadius * 2.0f,
                  knobRadius * 2.0f);
    g.fillEllipse(x3 - knobRadius, k3 - knobRadius, knobRadius * 2.0f,
                  knobRadius * 2.0f);
  }

  Theme m_theme;
  void setTheme(const Theme theme) {
    m_theme = theme;

    switch (m_theme) {
    case Theme::Light: {
      setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
      break;
    }

    case Theme::Dark: {
      setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
      break;
    }
    }
  }
};
}

