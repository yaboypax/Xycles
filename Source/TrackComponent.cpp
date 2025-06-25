//
// Created by Audio on 6/25/2025.
//

#include "TrackComponent.h"

TrackComponent::TrackComponent(XyclesAudioProcessor &p) :
                                                    m_processorRef(p),
                                                    m_thumbnailCache (5),
                                                    m_thumbnail (512, m_formatManager, m_thumbnailCache)
{
    addAndMakeVisible(m_gainSlider);
    m_gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    m_gainSlider.setRange(0.01, 1.00, 0.01);
    m_gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_gainSlider.onValueChange = [&]() {
        m_processorRef.setGain(static_cast<float>(m_gainSlider.getValue()));
    };

    addAndMakeVisible(m_speedSlider);
    m_speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    m_speedSlider.setRange(-2.0, 2.0, 0.01);
    m_speedSlider.setValue(1.0);
    m_speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_speedSlider.onValueChange = [&]() {
        m_processorRef.setSpeed(static_cast<float>(m_speedSlider.getValue()));
    };

    addAndMakeVisible(m_startTime);
    m_startTime.setSliderStyle(juce::Slider::LinearHorizontal);
    m_startTime.setRange(0.0, 1.0, 0.01);
    m_startTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_startTime.onValueChange = [&]() {
        m_processorRef.setStart(static_cast<float>(m_startTime.getValue()));
    };

    addAndMakeVisible(m_endTime);
    m_endTime.setSliderStyle(juce::Slider::LinearHorizontal);
    m_endTime.setRange(0.0, 1.0, 0.01);
    m_endTime.setValue(1.0);
    m_endTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_endTime.onValueChange = [&]() {
        m_processorRef.setEnd(static_cast<float>(m_endTime.getValue()));
    };


    m_formatManager.registerBasicFormats();

    addAndMakeVisible(m_playButton);
    m_playButton.onClick = [&]() {
        m_processorRef.play();
    };

    addAndMakeVisible(m_stopButton);
    m_stopButton.onClick = [&]() {
        m_processorRef.stop();
    };
}

void TrackComponent::paint(juce::Graphics &g) {
    g.setColour(juce::Colours::white);
    const auto thumbnailBounds = juce::Rectangle<int>(m_speedSlider.getX(), m_speedSlider.getBottom() + 20,m_speedSlider.getWidth(), m_speedSlider.getHeight());
    if (m_thumbnail.getNumChannels() != 0) {
        m_thumbnail.drawChannels (g, thumbnailBounds,0.0, m_thumbnail.getTotalLength(), 1.0f);
    }
}

void TrackComponent::resized() {

    m_gainSlider.setBounds(50, 50, getWidth()-100, 30);
    m_speedSlider.setBounds(m_gainSlider.getX(), m_gainSlider.getBottom() + 20,m_gainSlider.getWidth(), m_gainSlider.getHeight());
    m_playButton.setBounds(m_speedSlider.getX(), m_speedSlider.getBottom() + m_speedSlider.getHeight() + 20,m_speedSlider.getWidth()/2-10, m_speedSlider.getHeight());
    m_stopButton.setBounds(m_playButton.getRight() + 10, m_playButton.getY(),m_playButton.getWidth(), m_speedSlider.getHeight());
    m_startTime.setBounds(m_playButton.getX(), m_playButton.getBottom() + 20, m_playButton.getWidth(), 30);
    m_endTime.setBounds(m_stopButton.getX(), m_stopButton.getBottom() + 20, m_playButton.getWidth(), 30);
}

void TrackComponent::filesDropped(const StringArray &files, int x, int y) {
    if (isInterestedInFileDrag(files)) {
        loadFileThumbnail(*files.begin());
        m_processorRef.loadFile(files.begin()->toStdString());
    }
    repaint();
}

bool TrackComponent::isInterestedInFileDrag(const StringArray &files) {
    if (files.begin()->contains(".wav")) {
        return true;
    } else return false;
}

void TrackComponent::loadFileThumbnail(const String &fileName) {
    const auto file = juce::File(fileName);
    auto* reader = m_formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
        //transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        m_thumbnail.setSource (new juce::FileInputSource (file));
        //readerSource.reset (newSource.release());
    }
}
