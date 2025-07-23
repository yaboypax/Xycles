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


class PlayHead : public juce::Component {

public:
    PlayHead(const size_t id, const juce::Colour color) : m_id(id)
    {
        addAndMakeVisible(m_gainSlider);
        m_gainSlider.setRange(0.01, 1.00, 0.01);
        m_gainSlider.onValueChange = [&]() {
            if (onGainCallback)
                onGainCallback(m_gainSlider.getValue());
        };

        addAndMakeVisible(m_gainLabel);
        m_gainLabel.attachToComponent(&m_gainSlider, false);
        m_gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        m_gainLabel.setText("Gain", juce::dontSendNotification);
        m_gainLabel.setJustificationType(juce::Justification::centredTop);



        addAndMakeVisible(m_speedSlider);
        m_speedSlider.setRange(-2.0, 2.0, 0.01);
        m_speedSlider.setValue(1.0);
        m_speedSlider.onValueChange = [&]() {
            if (onSpeedCallback)
                onSpeedCallback(m_speedSlider.getValue());
        };

        addAndMakeVisible(m_speedLabel);
        m_speedLabel.attachToComponent(&m_speedSlider, false);
        m_speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        m_speedLabel.setText("Speed", juce::dontSendNotification);
        m_speedLabel.setJustificationType(juce::Justification::centredTop);


        m_gainSlider.setTrackColor(color);
        m_speedSlider.setTrackColor(color);
    }


    std::function<void(float)> onGainCallback;
    std::function<void(float)> onSpeedCallback;

    void paint (juce::Graphics& g) override {
    }
    void resized() override {
        constexpr auto labelHeight = 25;
        const auto sliderSize = getHeight() - labelHeight;

        m_gainSlider.setBounds(0, 0, sliderSize, sliderSize);
        m_speedSlider.setBounds(m_gainSlider.getRight() + 20, 0, sliderSize, sliderSize);
        m_gainLabel.setBounds(0, m_gainSlider.getBottom(), sliderSize, labelHeight);
        m_speedLabel.setBounds(m_gainSlider.getRight() + 20, m_gainSlider.getBottom(), sliderSize, labelHeight);
    }
private:
    const size_t m_id;
    TrackKnob m_gainSlider, m_speedSlider;
    juce::Label m_gainLabel, m_speedLabel;
};
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
    void addPlayhead();

private:
    void layoutSliders();
    void layoutButtons();
    const size_t m_id;
    juce::Colour m_color;

    XyclesAudioProcessor &m_processorRef;
    TrackSlider m_startTime, m_endTime;
    Xycles::PlayButton m_playButton;
    Xycles::StopButton m_stopButton;

    std::vector<std::unique_ptr<PlayHead>> m_playHeads;

    juce::AudioFormatManager m_formatManager;
    juce::AudioThumbnailCache m_thumbnailCache;
    juce::AudioThumbnail m_thumbnail;
    juce::Rectangle<int> m_thumbnailBounds;
};

