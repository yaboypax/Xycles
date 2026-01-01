//
// Created by R&F Dev on 7/22/25.
//

#pragma once
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
    g.setColour(shouldDrawButtonAsDown
                    ? juce::Colours::black.withMultipliedAlpha(0.8f)
                    : juce::Colours::black);
    g.fillRect(getLocalBounds().reduced(getWidth() * 0.3f));
  }
};

} // namespace Xycles
