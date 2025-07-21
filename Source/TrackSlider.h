//
// Created by R&F Dev on 7/20/25.
//

#pragma once
#include <JuceHeader.h>


class TrackSlider : public juce::Slider
{
public:
    TrackSlider()
    {
        setColour(juce::Slider::trackColourId, juce::Colours::black);
        setColour(juce::Slider::thumbColourId, juce::Colours::black);
    }
    void paint (juce::Graphics& g) override
    {
        juce::Slider::paint (g);
    }

private:
};
