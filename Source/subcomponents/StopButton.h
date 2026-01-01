//
// Created by R&F Dev on 7/22/25.
//

#pragma once
#include "../static/Utilities.h"
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

namespace Xycles {

struct StopButton : juce::TextButton {
  StopButton() {
    setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    setColour(juce::TextButton::buttonColourId,
              juce::Colours::transparentWhite);
    setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
  }
  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
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
    g.fillRect(getLocalBounds().reduced(getWidth() * 0.3f));
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

} // namespace Xycles
