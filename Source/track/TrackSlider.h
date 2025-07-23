//
// Created by R&F Dev on 7/20/25.
//

#pragma once
#include <JuceHeader.h>

enum TrackDirection { Start, End };
class TrackSlider : public juce::Slider
{
public:
    TrackSlider();
    void setTrackColor(juce::Colour colour);
    void setTrackDirection(TrackDirection direction);
    void setEndPosition(double endValue);
    void setStartPosition(double startValue);

    void paint (juce::Graphics& g) override;

private:
    TrackDirection m_trackDirection = Start;
    juce::Colour m_trackColor = juce::Colours::black;
    double m_startPosition = 0.0;
    double m_endPosition = 1.0;
};
