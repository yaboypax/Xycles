//
// Created by Audio on 8/5/2025.
//

#pragma once
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

namespace Xycles {

struct RecordButton : juce::TextButton {
  RecordButton() {
    setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    setColour(juce::TextButton::buttonColourId,
              juce::Colours::transparentWhite);
    setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
  }
  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    juce::TextButton::paintButton(g, shouldDrawButtonAsHighlighted,
                                  shouldDrawButtonAsDown);
    g.setColour(juce::Colours::red);
    g.fillEllipse(getLocalBounds().reduced(getWidth() * 0.3f).toFloat());
  }
};

} // namespace Xycles
