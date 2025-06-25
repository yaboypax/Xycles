//
// Created by Audio on 6/25/2025.
//

#pragma once
#include <JuceHeader.h>

#include "PluginProcessor.h"


class TrackComponent: public juce::Component, public juce::FileDragAndDropTarget
{
public:
    explicit TrackComponent(XyclesAudioProcessor &);
    void paint(juce::Graphics &) override;
    void resized() override;

    void filesDropped(	const StringArray &	files, int	x, int	y ) override;
    bool isInterestedInFileDrag(const StringArray &files) override;
    void loadFileThumbnail(const String& fileName);

private:
    XyclesAudioProcessor &m_processorRef;
    juce::Slider m_gainSlider, m_speedSlider;
    juce::TextButton m_playButton{"Play"}, m_stopButton{"Stop"};
    juce::Slider m_startTime, m_endTime;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

