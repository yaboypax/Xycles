//
// Created by R&F Dev on 7/20/25.
//

#pragma once
#include <JuceHeader.h>


class TrackKnob : public juce::Slider
{
public:
    TrackKnob() {}
    void setTrackColor(juce::Colour color);
    void paint(juce::Graphics& g) override;
private:
    juce::Colour m_trackColor = juce::Colours::black;

};

