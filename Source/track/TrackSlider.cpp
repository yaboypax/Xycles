//
// Created by R&F Dev on 7/20/25.
//

#include "TrackSlider.h"

TrackSlider::TrackSlider() { setScrollWheelEnabled(false); }
void TrackSlider::setTrackColor(juce::Colour colour) {
  m_trackColor = colour;
  repaint();
}

void TrackSlider::setTrackDirection(TrackDirection direction) {
  m_trackDirection = direction;
}
void TrackSlider::setEndPosition(double endValue) { m_endPosition = endValue; }
void TrackSlider::setStartPosition(double startValue) {
  m_startPosition = startValue;
}

void TrackSlider::setTheme(const Theme theme) { m_theme = theme; }
void TrackSlider::paint(juce::Graphics &g) {
  // g.fillAll (juce::Colours::black);
  auto trackWidth = jmin(2.0f, (float)getHeight() * 0.25f);
  const Point<float> startPoint(0, static_cast<float>(getHeight()) * 0.5f);
  const Point<float> endPoint(static_cast<float>(getWidth()), startPoint.y);

  Path backgroundTrack;
  backgroundTrack.startNewSubPath(startPoint);
  backgroundTrack.lineTo(endPoint);

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
  g.setColour(color);
  g.strokePath(backgroundTrack,
               {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});

  Path valueTrack;
  Point<float> thumbPoint;

  const auto kx = getPositionOfValue(getValue());
  const auto ky = (float)getHeight() * 0.5f;

  const Point<float> minPoint = startPoint;
  Point<float> maxPoint = {kx, ky};

  auto thumbWidth = 5;

  if (m_trackDirection == Start) {
    const Point<float> trackEnd = {getPositionOfValue(m_endPosition), ky};
    valueTrack.startNewSubPath(maxPoint);
    valueTrack.lineTo(trackEnd);
  } else if (m_trackDirection == End) {
    const Point<float> trackStart = {getPositionOfValue(m_startPosition), ky};
    valueTrack.startNewSubPath(trackStart);
    valueTrack.lineTo(maxPoint);
  }
  g.setColour(m_trackColor);
  g.strokePath(valueTrack,
               {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});

  const float halfLineLength = 10;
  const Point<float> lineStart = {maxPoint.x, maxPoint.y - halfLineLength};
  const Point<float> lineEnd = {maxPoint.x, maxPoint.y + halfLineLength};

  g.setColour(Colours::black);
  g.drawLine(Line<float>(lineStart, lineEnd), trackWidth);
}
