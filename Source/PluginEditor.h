#pragma once

#include "PluginProcessor.h"
#include "TrackComponent.h"
#include "TrackEditor.h"
//==============================================================================

class TopBarComponent : public Component
{
public:
    TopBarComponent()
    {
    }

    void paint (Graphics& g)
    {
        g.fillAll (juce::Colours::white);
        g.setColour (juce::Colours::black);
        g.setFont (25.f);
        g.drawText("Xycles", getLocalBounds(), juce::Justification::centred, true);

        g.setColour (juce::Colours::black);
        g.drawLine(0.f,static_cast<float>(getHeight()), static_cast<float>(getWidth()), static_cast<float>(getHeight()), 2.f);
    }
    void resized()
    {

    }
};
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessorEditor)
};
