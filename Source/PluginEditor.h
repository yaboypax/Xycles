#pragma once

#include "PluginProcessor.h"
#include "TrackComponent.h"
#include "TrackEditor.h"
//==============================================================================

class TopBarComponent : public Component
{
public:

    std::function<void()> onGlobalPlay = nullptr;
    std::function<void()> onGlobalStop = nullptr;
    TopBarComponent()
    {
        addAndMakeVisible(m_globalPlay);
        m_globalPlay.setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
        m_globalPlay.onClick = [&]{
            if (onGlobalPlay)
                onGlobalPlay();
        };

        addAndMakeVisible(m_globalStop);
        m_globalPlay.setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
        m_globalStop.onClick = [&]{
            if (onGlobalStop)
                onGlobalStop();
        };
    }

    void paint (Graphics& g) override
    {
        g.fillAll (juce::Colours::white);
        g.setColour (juce::Colours::black);
        g.setFont (25.f);
        g.drawText("Xycles", getLocalBounds(), juce::Justification::centred, true);

        g.setColour (juce::Colours::black);
        g.drawLine(0.f,static_cast<float>(getHeight()), static_cast<float>(getWidth()), static_cast<float>(getHeight()), 2.f);
    }
    void resized() override
    {
        constexpr int margin = 5;
        const int buttonSize = getHeight() - margin*2;
        m_globalPlay.setBounds(getWidth() - buttonSize*2 - margin*2, margin, buttonSize, buttonSize);
        m_globalStop.setBounds(getWidth() - buttonSize - margin, margin, buttonSize, buttonSize);
    }
private:
    Xycles::StopButton m_globalStop;
    Xycles::PlayButton m_globalPlay;
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
