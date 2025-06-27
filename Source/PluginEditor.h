#pragma once

#include "PluginProcessor.h"
#include "TrackComponent.h"


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
    std::vector<std::unique_ptr<TrackComponent>> m_tracks;
  std::size_t m_trackCount = 0;
  juce::TextButton m_plusButton;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
