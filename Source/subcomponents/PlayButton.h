//
// Created by R&F Dev on 7/22/25.
//

#pragma once
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

namespace Xycles {

struct PlayButton : juce::TextButton {
  PlayButton() {
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

    const auto area = getLocalBounds().toFloat().reduced(getWidth() * 0.3f);
    juce::Path triangle;
    triangle.addTriangle(area.getX(), area.getY(), area.getRight(),
                         area.getCentreY(), area.getX(), area.getBottom());

    g.fillPath(triangle);
  }
};

} // namespace Xycles
