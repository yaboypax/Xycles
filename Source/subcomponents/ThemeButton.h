#pragma once
#include "../static/Utilities.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>

namespace Xycles {

struct ThemeButton : juce::TextButton {
  ThemeButton() {
    setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    setColour(juce::TextButton::buttonColourId,
              juce::Colours::transparentWhite);
    setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
  }
  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    juce::TextButton::paintButton(g, shouldDrawButtonAsHighlighted,
                                  shouldDrawButtonAsDown);
  }

  Theme m_theme;
  void setTheme(const Theme theme) {
    m_theme = theme;

    switch (m_theme) {
    case Theme::Light: {
      setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
      setColour(juce::TextButton::textColourOffId, juce::Colours::black);
      break;
    }

    case Theme::Dark: {
      setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
      setColour(juce::TextButton::textColourOffId, juce::Colours::white);
      break;
    }
    }
  }
};
} // namespace Xycles
