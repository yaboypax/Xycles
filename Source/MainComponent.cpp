//
// Created by Audio on 8/24/2025.
//

#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1200, 1000);
    setAudioChannels(0,8);
    //setResizable(true, true);
    addAndMakeVisible(m_trackViewport);
    m_trackEditor.setSize(1200, 1000);
    m_trackViewport.setViewedComponent(&m_trackEditor, false);

    m_trackEditor.addTrackCallback = [&]() ->std::unique_ptr<TrackComponent> {
        m_trackEngines.emplace_back(rust_part::new_engine());
        rust_part::Engine* eng = &*m_trackEngines.back();
        return std::make_unique<TrackComponent>(eng);
    };

    m_trackEditor.removeTrackCallback = [&]{
        m_trackEngines.pop_back();
    };

    addAndMakeVisible(m_topBar);
    m_topBar.onGlobalPlay = [&] {
        playAll();
    };
    m_topBar.onGlobalStop = [&] {
        stopAll();
    };

    m_topBar.setRecorder(m_recorder);

    m_backgroundImage = juce::Image(
        juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize));

}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

    //==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
    g.drawImage(m_backgroundImage, getLocalBounds().toFloat() );
}

void MainComponent::resized()
{
    constexpr int kTopBarHeight = 50;
    m_topBar.setBounds(0, 0, getWidth(), kTopBarHeight);
    m_trackViewport.setBounds(getLocalBounds().withTrimmedTop(kTopBarHeight));
    m_trackEditor.setSize(getWidth(), m_trackEditor.getHeight());
}

void MainComponent::prepareToPlay (int samplesPerBlock, double sampleRate)
{
    for (int i = 0; i < samplesPerBlock*2; i++) {
        m_interleavedBuffer.push_back(0.f);
    }
    m_recorder->prepareToPlay(sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::ScopedNoDenormals noDenormals;

    if (m_trackEngines.empty())
        return;
    bufferToFill.clearActiveBufferRegion();

    using Format = AudioData::Format<AudioData::Float32, AudioData::NativeEndian>;
    juce::AudioData::interleaveSamples (AudioData::NonInterleavedSource<Format> { bufferToFill.buffer->getArrayOfReadPointers(), bufferToFill.buffer->getNumChannels() },
                                        AudioData::InterleavedDest<Format>      { &m_interleavedBuffer[0],   bufferToFill.buffer->getNumChannels() }, bufferToFill.buffer->getNumSamples());

    for (auto& track: m_trackEngines) {
        track->process_block(m_interleavedBuffer);
    }


    juce::AudioData::deinterleaveSamples(AudioData::InterleavedSource<Format>{&m_interleavedBuffer[0], bufferToFill.buffer->getNumChannels()},
                                          AudioData::NonInterleavedDest<Format>{bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels()}, bufferToFill.buffer->getNumSamples());

    if (m_recorder->isRecording())
        m_recorder->processBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    m_recorder->releaseResources();
}

void MainComponent::playAll() const noexcept
{
    m_trackEditor.playAll();
}

void MainComponent::stopAll() const noexcept
{
  m_trackEditor.stopAll();
}