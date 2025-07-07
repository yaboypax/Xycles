//
// Created by Audio on 7/6/2025.
//

#include "TrackEditor.h"

namespace {
    constexpr int kTrackHeight = 400;
    constexpr int kButtonHeight = 50;
}

TrackEditor::TrackEditor(XyclesAudioProcessor &p) : processorRef(p) {
    addAndMakeVisible(&m_plusButton);
    m_plusButton.setButtonText("+");
    m_plusButton.onClick = [&] {
        processorRef.addTrack();
        std::unique_ptr<TrackComponent> track = std::make_unique<TrackComponent>(processorRef, m_trackCount);
        addAndMakeVisible(track.get());
        m_tracks.push_back(std::move(track));
        m_trackCount++;

        const int totalHeight = m_trackCount * kTrackHeight + kButtonHeight;
        setSize(getWidth(), totalHeight);
    };

    addAndMakeVisible(&m_removeButton);
    m_removeButton.setButtonText("-");
    m_removeButton.onClick = [&] {
        processorRef.removeTrack();
        m_tracks.pop_back();
        m_trackCount--;
        const int totalHeight = m_trackCount * kTrackHeight + kButtonHeight;
        setSize(getWidth(), totalHeight);
    };
}

void TrackEditor::resized()
    {
    int Y = 0;
    for (auto& track : m_tracks) {
        track->setBounds(0, Y, getWidth(), kTrackHeight);
        Y+= kTrackHeight;
    }

    if (m_tracks.empty()) {
        m_plusButton.setBounds(getLocalBounds());
    } else {
        const auto lastBounds = m_tracks.back()->getBounds();
        m_plusButton.setBounds(0, lastBounds.getBottom(), getWidth()/2-2, kButtonHeight);
        m_removeButton.setBounds(getWidth()/2, lastBounds.getBottom(), getWidth()/2-2, kButtonHeight);
    }

}