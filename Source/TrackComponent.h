//
// Created by Audio on 6/25/2025.
//

#pragma once
#include <JuceHeader.h>
#include "TrackSlider.h"
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
    void animate(juce::Graphics &g);
    void update() override;

    size_t getID() const {return m_id;}

private:
    void layoutSliders();
    const size_t m_id;
    juce::Colour m_color;
    XyclesAudioProcessor &m_processorRef;
    juce::Slider m_gainSlider, m_speedSlider;
    juce::TextButton m_playButton{"Play"}, m_stopButton{"Stop"};
    TrackSlider m_startTime, m_endTime;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

