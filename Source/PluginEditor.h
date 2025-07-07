#pragma once

#include "PluginProcessor.h"
#include "TrackComponent.h"
#include "TrackEditor.h"
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
