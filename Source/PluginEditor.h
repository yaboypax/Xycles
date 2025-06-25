#pragma once

#include "PluginProcessor.h"

//==============================================================================
class XyclesAudioProcessorEditor final
    : public juce::AudioProcessorEditor {
public:
  explicit XyclesAudioProcessorEditor(XyclesAudioProcessor &);
  ~XyclesAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  XyclesAudioProcessor &processorRef;

  juce::TextEditor textEditor;
  juce::Slider gainSlider, speedSlider;
    juce::TextButton playButton{"Play"}, stopButton{"Stop"};
  juce::Slider startTime, endTime;
  //juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
