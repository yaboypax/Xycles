//
// Created by Audio on 7/6/2025.
//

#include "TrackEditor.h"

namespace {
    constexpr int kTrackHeight = 400;
    constexpr int kButtonHeight = 50;
}

TrackEditor::TrackEditor() {
    addAndMakeVisible(&m_plusButton);
    m_plusButton.setButtonText("+");

    m_plusButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    m_plusButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    m_plusButton.onClick = [&] {
        assert(addTrackCallback);
        std::unique_ptr<TrackComponent> track = addTrackCallback();
        addAndMakeVisible(track.get());
        m_tracks.push_back(std::move(track));
        m_trackCount++;

        const int totalHeight = m_trackCount * kTrackHeight + kButtonHeight;
        setSize(getWidth(), totalHeight);
    };

    addAndMakeVisible(&m_removeButton);
    m_removeButton.setButtonText("-");
    m_removeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    m_removeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    m_removeButton.onClick = [&] {
        assert(addTrackCallback);
        removeTrackCallback();
        m_tracks.pop_back();
        m_trackCount--;
        const int totalHeight = static_cast<int>(m_trackCount) * kTrackHeight + kButtonHeight;
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

void TrackEditor::playAll() const noexcept
{
    for (const auto& track : m_tracks)
        track->play();
}

void TrackEditor::stopAll() const noexcept
{
    for (const auto& track : m_tracks)
        track->stop();
}
