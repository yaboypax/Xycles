//
// Created by Audio on 6/25/2025.
//

#pragma once
#include <JuceHeader.h>
#include "TrackSlider.h"
#include "TrackKnob.h"
#include "../subcomponents/PlayButton.h"
#include "../subcomponents/StopButton.h"
#include "rust_part.h"

enum PlayMode {
    Regular,
    Granular
  };

class TrackComponent: public juce::AnimatedAppComponent, public juce::FileDragAndDropTarget
{
public:
    explicit TrackComponent(rust_part::Engine*);
    void paint(juce::Graphics &) override;
    void resized() override;

    void filesDropped(	const StringArray &	files, int	x, int	y ) override;
    bool isInterestedInFileDrag(const StringArray &files) override;
    void loadFileThumbnail(const String& fileName);
    void drawTrack(juce::Graphics &g);
    void animate(juce::Graphics &g);
    void update() override;


    void mouseEnter(const MouseEvent &e) override;

private:
    rust_part::Engine* m_engine;
    void layoutSliders();
    void layoutButtons();

    PlayMode m_playMode = PlayMode::Regular;
    void togglePlayMode();

    juce::Colour m_color;

    juce::Label m_playHeadLabel;
    TrackKnob m_gainSlider, m_speedSlider;
    juce::Label m_gainLabel, m_speedLabel;

    juce::Label m_granulatorLabel;
    TrackKnob m_grainSpeed, m_grainLength, m_grainOverlap, m_grainsCount, m_grainSpread;
    juce::Label m_grainSpeedLabel, m_grainLengthLabel, m_grainOverlapLabel, m_grainsCountLabel, m_grainSpreadLabel;
    juce::TextButton m_granulatorButton;


    juce::Label m_effectsLabel;
    TrackKnob m_reverbTime, m_reverbAmount, m_delayTime, m_delayFeedback, m_delayAmount;
    juce::Label m_reverbTimeLabel, m_reverbAmountLabel, m_delayTimeLabel, m_delayFeedbackLabel, m_delayAmountLabel;
    juce::TextButton m_effectsButton;


    TrackSlider m_startTime, m_endTime;
    Xycles::PlayButton m_playButton;
    Xycles::StopButton m_stopButton;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

