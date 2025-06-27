//
// Created by Audio on 6/25/2025.
//

#pragma once
#include <JuceHeader.h>

#include "PluginProcessor.h"


class TrackComponent: public juce::AnimatedAppComponent, public juce::FileDragAndDropTarget
{
public:
    explicit TrackComponent(XyclesAudioProcessor &, size_t);
    void paint(juce::Graphics &) override;
    void resized() override;

    void filesDropped(	const StringArray &	files, int	x, int	y ) override;
    bool isInterestedInFileDrag(const StringArray &files) override;
    void loadFileThumbnail(const String& fileName);
    void drawTrack(juce::Graphics &g);
    void update() override;

private:
    const size_t id;
    XyclesAudioProcessor &m_processorRef;
    juce::Slider m_gainSlider, m_speedSlider;
    juce::TextButton m_playButton{"Play"}, m_stopButton{"Stop"};
    juce::Slider m_startTime, m_endTime;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

