//
// Created by Audio on 8/24/2025.
//

#include "MainComponent.h"
#include "juce_graphics/juce_graphics.h"

MainComponent::MainComponent() {
  setSize(1200, 1000);
  setAudioChannels(0, 2);
  // setResizable(true, true);
  addAndMakeVisible(m_trackViewport);
  m_trackEditor.setSize(1200, 1000);
  m_trackViewport.setViewedComponent(&m_trackEditor, false);

  m_trackEditor.addTrackCallback = [&]() -> std::unique_ptr<TrackComponent> {
    m_trackEngines.emplace_back(rust_part::new_engine());
    rust_part::Engine *eng = &*m_trackEngines.back();
    auto track = std::make_unique<TrackComponent>(eng);
    track->setTheme(m_theme);
    return track;
  };

  m_trackEditor.removeTrackCallback = [&] { m_trackEngines.pop_back(); };

  addAndMakeVisible(m_topBar);
  m_topBar.onGlobalPlay = [&] { playAll(); };
  m_topBar.onGlobalStop = [&] { stopAll(); };

  m_topBar.setRecorder(m_recorder);

  m_topBar.onLightDarkToggle = [&] {
    switch (m_theme) {
    case Theme::Light: {
      m_theme = Theme::Dark;
      break;
    }
    case Theme::Dark: {
      m_theme = Theme::Light;
      break;
    }
    }
    m_trackEditor.setTheme(m_theme);
    repaint();
  };

  m_topBar.onAudioSettings = [&] { showAudioSettings(); };

  m_backgroundImage = juce::Image(juce::ImageCache::getFromMemory(
      BinaryData::background_png, BinaryData::background_pngSize));
  m_darkBackgroundImage = juce::Image(juce::ImageCache::getFromMemory(
      BinaryData::darkbackground_png, BinaryData::darkbackground_pngSize));
}

MainComponent::~MainComponent() { shutdownAudio(); }

//==============================================================================
void MainComponent::paint(juce::Graphics &g) {

  switch (m_theme) {
  case Theme::Light: {
    g.fillAll(juce::Colours::white);
    g.drawImage(m_backgroundImage, getLocalBounds().toFloat());
    break;
  }
  case Theme::Dark: {
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
  m_interleavedBuffer.clear();
  m_interleavedBuffer.reserve(static_cast<size_t>(maxFrames * 2));
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

  const int frames = bufferToFill.numSamples;
  const int channels = bufferToFill.buffer->getNumChannels();

  // Slice from valid large stack buffer - Linux update
  const size_t valid = (size_t)frames * (size_t)channels;
  jassert(valid <= m_interleavedBuffer.size());

#ifdef BUFFER_DEBUG
  juce::Logger::writeToLog("frames=" + juce::String(frames) +
                           " ch=" + juce::String(channels) +
                           " valid=" + juce::String(valid));
#endif

  float *base = &m_interleavedBuffer[0];
  using Format = AudioData::Format<AudioData::Float32, AudioData::NativeEndian>;
  juce::AudioData::interleaveSamples(
      AudioData::NonInterleavedSource<Format>{
          bufferToFill.buffer->getArrayOfReadPointers(), channels},
      AudioData::InterleavedDest<Format>{base, channels}, frames);

  rust::Slice<float> slice{base, valid};

  for (auto &track : m_trackEngines) {
    track->process_block(slice);
  }

  juce::AudioData::deinterleaveSamples(
      AudioData::InterleavedSource<Format>{
          base, bufferToFill.buffer->getNumChannels()},
      AudioData::NonInterleavedDest<Format>{
          bufferToFill.buffer->getArrayOfWritePointers(),
          bufferToFill.buffer->getNumChannels()},
      bufferToFill.buffer->getNumSamples());

  if (m_recorder->isRecording())
    m_recorder->processBlock(bufferToFill);
}

void MainComponent::showAudioSettings() {
  auto *dm = &m_deviceManager;
  auto selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
      *dm, 0, 256, // min/max inputs
      0, 256,      // min/max outputs
      true, true, true, false);

  selector->setSize(500, 400);

  juce::DialogWindow::LaunchOptions o;
  o.dialogTitle = "Audio Settings";
  o.content.setOwned(selector.release());
  o.componentToCentreAround = this;
  o.escapeKeyTriggersCloseButton = true;
  o.useNativeTitleBar = true;
  o.resizable = false;

  o.launchAsync();
}

void MainComponent::releaseResources() { m_recorder->releaseResources(); }

void MainComponent::playAll() const noexcept { m_trackEditor.playAll(); }

void MainComponent::stopAll() const noexcept { m_trackEditor.stopAll(); }
