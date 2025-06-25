#pragma once

#include "PluginProcessor.h"




//==============================================================================
class XyclesAudioProcessorEditor final
    : public juce::AudioProcessorEditor, public juce::FileDragAndDropTarget
{
public:
  explicit XyclesAudioProcessorEditor(XyclesAudioProcessor &);
  ~XyclesAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void filesDropped(	const StringArray &	files, int	x, int	y ) override;
  bool isInterestedInFileDrag(const StringArray &files) override;
  void loadFileThumbnail(const String& fileName);

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  XyclesAudioProcessor &processorRef;

  juce::Slider gainSlider, speedSlider;
    juce::TextButton playButton{"Play"}, stopButton{"Stop"};
  juce::Slider startTime, endTime;

  juce::AudioFormatManager formatManager;
  juce::AudioThumbnailCache thumbnailCache;
  juce::AudioThumbnail thumbnail;
  //juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
