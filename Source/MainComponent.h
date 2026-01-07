//
// Created by Audio on 8/24/2025.
//
#pragma once
#include <JuceHeader.h>

#include "AudioRecorder.h"

#include "juce_audio_devices/juce_audio_devices.h"
#include "topbar/TopBarComponent.h"
#include "track/TrackEditor.h"

class MainComponent : public juce::AudioAppComponent {
public:
  //==============================================================================
  MainComponent();
  ~MainComponent() override;

  //==============================================================================
  void paint(juce::Graphics &g) override;
  void resized() override;

  void prepareToPlay(int samplesPerBlock, double sampleRate) override;
  void
  getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;
  void showAudioSettings();

  void playAll() const noexcept;
  void stopAll() const noexcept;

private:
  std::vector<float> m_interleavedBuffer;
  std::vector<rust::Box<rust_part::Engine>> m_trackEngines;
  std::shared_ptr<AudioRecorder> m_recorder = std::make_shared<AudioRecorder>();
  //==============================================================================
  juce::AudioDeviceManager m_deviceManager;
  juce::Viewport m_trackViewport;
  TrackEditor m_trackEditor;
  TopBarComponent m_topBar;
  juce::Image m_backgroundImage;
  juce::Image m_darkBackgroundImage;
  Theme m_theme = Theme::Light;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
