#pragma once

#include "PluginProcessor.h"
#include "topbar/TopBarComponent.h"
#include "track/TrackEditor.h"
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
  TopBarComponent m_topBar;
  juce::Image m_backgroundImage;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
