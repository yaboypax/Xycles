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
    m_gainSlider.setSliderStyle(juce::Slider::Rotary);
    m_gainSlider.setRange(0.01, 1.00, 0.01);
    m_gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_gainSlider.onValueChange = [&]() {
        m_processorRef.setGain(static_cast<float>(m_gainSlider.getValue()));
    };

    addAndMakeVisible(m_speedSlider);
    m_speedSlider.setSliderStyle(juce::Slider::Rotary);
    m_speedSlider.setRange(-2.0, 2.0, 0.01);
    m_speedSlider.setValue(1.0);
    m_speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_speedSlider.onValueChange = [&]() {
        m_processorRef.setSpeed(static_cast<float>(m_speedSlider.getValue()));
    };

    addAndMakeVisible(m_startTime);
    m_startTime.setSliderStyle(juce::Slider::Rotary);
    m_startTime.setRange(0.0, 1.0, 0.001);
    m_startTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_startTime.onValueChange = [&]() {
        if (m_startTime.getValue() >= m_endTime.getValue()) {
            m_startTime.setValue(m_endTime.getValue()-0.01);
        }
        m_processorRef.setStart(static_cast<float>(m_startTime.getValue()));
        repaint();
    };

    addAndMakeVisible(m_endTime);
    m_endTime.setSliderStyle(juce::Slider::Rotary);
    m_endTime.setRange(0.0, 1.0, 0.001);
    m_endTime.setValue(1.0);
    m_endTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_endTime.onValueChange = [&]() {
        if (m_endTime.getValue() <= m_startTime.getValue()) {
            m_endTime.setValue(m_startTime.getValue()+0.01);
        }
        m_processorRef.setEnd(static_cast<float>(m_endTime.getValue()));
        repaint();
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
    drawTrack(g);
}

void TrackComponent::drawTrack(juce::Graphics &g) {

    if (m_thumbnail.getNumChannels() != 0) {
        g.setColour(juce::Colours::white);
        m_thumbnail.drawChannels (g, m_thumbnailBounds,m_thumbnail.getTotalLength() * m_startTime.getValue(), m_thumbnail.getTotalLength() * m_endTime.getValue(), 1.0f);
    } else {
        g.setColour (juce::Colours::darkgrey);
        g.fillRect (m_thumbnailBounds);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("No File Loaded", m_thumbnailBounds, juce::Justification::centred, 1);

    }
}


void TrackComponent::resized() {

    m_thumbnailBounds = juce::Rectangle<int>(50, 50, getWidth()- 100, 200);
    const auto sliderY = m_thumbnailBounds.getBottom() + 20;
    const auto sliderSize = 100;
    m_gainSlider.setBounds(50, sliderY, sliderSize, sliderSize);
    m_speedSlider.setBounds(m_gainSlider.getRight() + 20, sliderY, sliderSize, sliderSize);
    m_startTime.setBounds(m_speedSlider.getRight() + 20, sliderY, sliderSize, sliderSize);
    m_endTime.setBounds(m_startTime.getRight() + 20, sliderY, sliderSize, sliderSize);
    m_playButton.setBounds(m_gainSlider.getX(), m_speedSlider.getBottom() + 20,m_thumbnailBounds.getWidth()/2-10, m_speedSlider.getHeight());
    m_stopButton.setBounds(m_playButton.getRight() + 10, m_playButton.getY(),m_playButton.getWidth(), m_speedSlider.getHeight());

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
    if (auto* reader = m_formatManager.createReaderFor(file); reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
        //transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        m_thumbnail.setSource (new juce::FileInputSource (file));
        //readerSource.reset (newSource.release());
    }
}
