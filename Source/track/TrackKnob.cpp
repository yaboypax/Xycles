//
// Created by R&F Dev on 7/20/25.
//

#include "TrackKnob.h"

TrackKnob::TrackKnob() {
  setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}
void TrackKnob::setTrackColor(juce::Colour color) {
  m_trackColor = color;
  repaint();
}

void TrackKnob::setTheme(const Theme theme) { m_theme = theme; }
void TrackKnob::paint(juce::Graphics &g) {
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

  const auto outline = isEnabled() ? color : color.withAlpha(0.5f);
  const auto fill =
      isEnabled() ? m_trackColor : juce::Colours::black.withAlpha(0.5f);
  const auto sliderPos = valueToProportionOfLength(getValue());
  auto bounds =
      Rectangle<int>(0, 0, getWidth(), getHeight()).toFloat().reduced(10);

  auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle = getRotaryParameters().startAngleRadians +
                 sliderPos * (getRotaryParameters().endAngleRadians -
                              getRotaryParameters().startAngleRadians);
  auto lineW = jmin(4.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;
  Path backgroundArc;
  backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                              arcRadius, arcRadius, 0.0f,
                              getRotaryParameters().startAngleRadians,
                              getRotaryParameters().endAngleRadians, true);

  g.setColour(outline);
  g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved,
                                             PathStrokeType::rounded));

  if (isEnabled()) {
    Path valueArc;
    valueArc.addCentredArc(
        bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
        getRotaryParameters().startAngleRadians, toAngle, true);

    g.setColour(fill);
    g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved,
                                          PathStrokeType::rounded));
  }
  constexpr float halfLineLength = 10.0f;

  const Point<float> midPoint(
      bounds.getCentreX() +
          arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
      bounds.getCentreY() +
          arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  const Point<float> lineStart(
      midPoint.x -
          halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
      midPoint.y -
          halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));
  const Point<float> lineEnd(
      midPoint.x +
          halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
      midPoint.y +
          halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));

  g.drawLine(Line<float>(bounds.getCentre(), midPoint), 2.f);
}
