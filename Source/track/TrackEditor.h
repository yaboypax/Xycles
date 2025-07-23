//
// Created by Audio on 7/6/2025.
//
#pragma once
#include "TrackComponent.h"

class TrackEditor : public juce::Component {
public:
    explicit TrackEditor(XyclesAudioProcessor &p);
    void paint(juce::Graphics &) override {}
    void resized() override;

private:
    XyclesAudioProcessor &processorRef;
    std::vector<std::unique_ptr<TrackComponent>> m_tracks;
    std::size_t m_trackCount = 0;
    juce::TextButton m_plusButton,m_removeButton;
};
