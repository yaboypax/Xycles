//
// Created by Audio on 8/24/2025.
//

#include "MainComponent.h"
#include "juce_graphics/juce_graphics.h"

MainComponent::MainComponent() {
  setSize(1200, 1000);
  setAudioChannels(0, 8);
  // setResizable(true, true);
  addAndMakeVisible(m_trackViewport);
  m_trackEditor.setSize(1200, 1000);
  m_trackViewport.setViewedComponent(&m_trackEditor, false);

  m_trackEditor.addTrackCallback = [&]() -> std::unique_ptr<TrackComponent> {
    m_trackEngines.emplace_back(rust_part::new_engine());
    rust_part::Engine *eng = &*m_trackEngines.back();
    return std::make_unique<TrackComponent>(eng);
  };

  m_trackEditor.removeTrackCallback = [&] { m_trackEngines.pop_back(); };

  addAndMakeVisible(m_topBar);
  m_topBar.onGlobalPlay = [&] { playAll(); };
  m_topBar.onGlobalStop = [&] { stopAll(); };

  m_topBar.setRecorder(m_recorder);

  m_topBar.onLightDarkToggle = [&] {
    switch (m_theme) {
    case Theme::LIGHT: {
      m_theme = Theme::DARK;
      break;
    }
    case Theme::DARK: {
      m_theme = Theme::LIGHT;
      break;
    }
    }
    m_trackEditor.setTheme(m_theme);
    repaint();
  };

  m_backgroundImage = juce::Image(juce::ImageCache::getFromMemory(
      BinaryData::background_png, BinaryData::background_pngSize));
  m_darkBackgroundImage = juce::Image(juce::ImageCache::getFromMemory(
      BinaryData::darkbackground_png, BinaryData::darkbackground_pngSize));
}

MainComponent::~MainComponent() { shutdownAudio(); }

//==============================================================================
void MainComponent::paint(juce::Graphics &g) {

  switch (m_theme) {
  case Theme::LIGHT: {
    g.fillAll(juce::Colours::white);
    g.drawImage(m_backgroundImage, getLocalBounds().toFloat());
    break;
  }
  case Theme::DARK: {
    g.fillAll(juce::Colour(0xFF383838));
    // g.setOpacity(0.8f);
    g.drawImage(m_darkBackgroundImage, getLocalBounds().toFloat());
    break;
  }
  }
}

void MainComponent::resized() {
  constexpr int kTopBarHeight = 50;
  m_topBar.setBounds(0, 0, getWidth(), kTopBarHeight);
  m_trackViewport.setBounds(getLocalBounds().withTrimmedTop(kTopBarHeight));
  m_trackEditor.setSize(getWidth(), m_trackEditor.getHeight());
}

constexpr int MAX_FRAMES = 4096;

void MainComponent::prepareToPlay(int samplesPerBlock, double sampleRate) {
  const int maxFrames = juce::jmax(samplesPerBlock, MAX_FRAMES);
  for (int i = 0; i < maxFrames * 2; i++) {
    m_interleavedBuffer.push_back(0.f);
  }
  m_recorder->prepareToPlay(sampleRate);
}

void MainComponent::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  juce::ScopedNoDenormals noDenormals;

  if (m_trackEngines.empty())
    return;
  bufferToFill.clearActiveBufferRegion();

  /*
  const int frames = bufferToFill.numSamples;
  const int ch = bufferToFill.buffer->getNumChannels();
  const int needed = frames * ch;

  juce::Logger::writeToLog(
      "frames=" + juce::String(frames) + " ch=" + juce::String(ch) +
      " needed=" + juce::String(needed) +
      " vecSize=" + juce::String((int)m_interleavedBuffer.size()));
  */

  using Format = AudioData::Format<AudioData::Float32, AudioData::NativeEndian>;
  juce::AudioData::interleaveSamples(
      AudioData::NonInterleavedSource<Format>{
          bufferToFill.buffer->getArrayOfReadPointers(),
          bufferToFill.buffer->getNumChannels()},
      AudioData::InterleavedDest<Format>{&m_interleavedBuffer[0],
                                         bufferToFill.buffer->getNumChannels()},
      bufferToFill.buffer->getNumSamples());

  for (auto &track : m_trackEngines) {
    track->process_block(m_interleavedBuffer);
  }

  juce::AudioData::deinterleaveSamples(
      AudioData::InterleavedSource<Format>{
          &m_interleavedBuffer[0], bufferToFill.buffer->getNumChannels()},
      AudioData::NonInterleavedDest<Format>{
          bufferToFill.buffer->getArrayOfWritePointers(),
          bufferToFill.buffer->getNumChannels()},
      bufferToFill.buffer->getNumSamples());

  if (m_recorder->isRecording())
    m_recorder->processBlock(bufferToFill);
}

void MainComponent::releaseResources() { m_recorder->releaseResources(); }

void MainComponent::playAll() const noexcept { m_trackEditor.playAll(); }

void MainComponent::stopAll() const noexcept { m_trackEditor.stopAll(); }
