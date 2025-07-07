#pragma once

#include "PluginProcessor.h"
#include "TrackComponent.h"

namespace {
  const int kTrackHeight = 400;
  const int kButtonHeight = 50;
}



class TrackEditor : public juce::Component {
  public:
  TrackEditor(XyclesAudioProcessor &p) : processorRef(p) {
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
  }
  void paint(juce::Graphics &g) override {}
  void resized() override
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
      m_plusButton.setBounds(0, lastBounds.getBottom(), getWidth(), kButtonHeight);
    }

  }

  private:
  XyclesAudioProcessor &processorRef;
  std::vector<std::unique_ptr<TrackComponent>> m_tracks;
  std::size_t m_trackCount = 0;
  juce::TextButton m_plusButton;
};

//==============================================================================
class XyclesAudioProcessorEditor final
    : public juce::AudioProcessorEditor
{
public:
  explicit XyclesAudioProcessorEditor(XyclesAudioProcessor &);
  ~XyclesAudioProcessorEditor() override;
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  XyclesAudioProcessor &processorRef;
  juce::Viewport m_trackViewport;
  TrackEditor m_trackEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
