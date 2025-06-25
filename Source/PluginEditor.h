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
    TrackComponent m_trackComponent;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
