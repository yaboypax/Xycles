//
// Created by Audio on 6/25/2025.
//

#pragma once
#include <JuceHeader.h>
#include "TrackSlider.h"
#include "TrackKnob.h"
#include "../subcomponents/PlayButton.h"
#include "../subcomponents/StopButton.h"
#include "../PluginProcessor.h"


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

    void mouseEnter(const MouseEvent &e) override;

private:
    void layoutSliders();
    void layoutButtons();
    const size_t m_id;
    juce::Colour m_color;

    XyclesAudioProcessor &m_processorRef;

    juce::Label m_playHeadLabel;
    TrackKnob m_gainSlider, m_speedSlider;
    juce::Label m_gainLabel, m_speedLabel;

    juce::Label m_granulatorLabel;
    TrackKnob m_grainSpeed, m_grainLength, m_grainOverlap;
    juce::Label m_grainSpeedLabel, m_grainLengthLabel, m_grainOverlapLabel;

    TrackSlider m_startTime, m_endTime;
    Xycles::PlayButton m_playButton;
    Xycles::StopButton m_stopButton;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

