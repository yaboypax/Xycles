#pragma once
#include <JuceHeader.h>

struct PowerButton : juce::TextButton {
  PowerButton() {}
  void paintButton(juce::Graphics &g, bool, bool) override {

    Path powerButton;

    auto bounds = getLocalBounds();
    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    float ang = 30.f;
    size -= 6;

    powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * 0.5,
                              size * 0.5, 0.f, degreesToRadians(ang),
                              degreesToRadians(360.f - ang), true);

    powerButton.startNewSubPath(r.getCentreX(), r.getY());
    powerButton.lineTo(r.getCentre());

    PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

    auto color =
        getToggleState() ? Colours::dimgrey : juce::Colour(188, 198, 206);

    g.setColour(color);
    g.strokePath(powerButton, pst);
    g.drawEllipse(r, 2);
  }
};
