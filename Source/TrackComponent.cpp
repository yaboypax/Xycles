//
// Created by Audio on 6/25/2025.
//

#include "TrackComponent.h"

TrackComponent::TrackComponent(XyclesAudioProcessor &p, const size_t id) :
                                                    m_id(id), m_processorRef(p),
                                                    m_thumbnailCache (5),
                                                    m_thumbnail (512, m_formatManager, m_thumbnailCache)
{
    setFramesPerSecond (30);
    m_color = juce::Colours::black;

    layoutSliders();
    layoutButtons();

    m_formatManager.registerBasicFormats();

}

void TrackComponent::layoutSliders()
{


    addAndMakeVisible(m_gainSlider);
    m_gainSlider.setSliderStyle(juce::Slider::Rotary);
    m_gainSlider.setRange(0.01, 1.00, 0.01);
    m_gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_gainSlider.setTrackColor( m_color);
    m_gainSlider.onValueChange = [&]() {
        m_processorRef.setGain(m_id, static_cast<float>(m_gainSlider.getValue()));
    };

    addAndMakeVisible(m_speedSlider);
    m_speedSlider.setSliderStyle(juce::Slider::Rotary);
    m_speedSlider.setRange(-2.0, 2.0, 0.01);
    m_speedSlider.setValue(1.0);
    m_speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_speedSlider.setTrackColor( m_color);
    m_speedSlider.onValueChange = [&]() {
        m_processorRef.setSpeed(m_id, static_cast<float>(m_speedSlider.getValue()));
    };

    addAndMakeVisible(m_startTime);
    m_endTime.setTrackDirection(Start);
    m_startTime.setSliderStyle(juce::Slider::LinearHorizontal);
    m_startTime.setRange(0.0, 1.0, 0.001);
    m_startTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_startTime.setTrackColor(m_color);
    m_startTime.onValueChange = [&]() {
        if (m_startTime.getValue() >= m_endTime.getValue()) {
            m_startTime.setValue(m_endTime.getValue()-0.001);
        }
        m_endTime.setStartPosition(m_startTime.getValue());
        m_processorRef.setStart(m_id, static_cast<float>(m_startTime.getValue()));
        repaint();
    };

    addAndMakeVisible(m_endTime);
    m_endTime.setTrackDirection(End);
    m_endTime.setRange(0.0, 1.0, 0.001);
    m_endTime.setValue(1.0);
    m_endTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_endTime.setTrackColor(m_color);
    m_endTime.onValueChange = [&]() {
        if (m_endTime.getValue() <= m_startTime.getValue()) {
            m_endTime.setValue(m_startTime.getValue()+0.001);
        }
        m_startTime.setEndPosition(m_endTime.getValue());
        m_processorRef.setEnd(m_id, static_cast<float>(m_endTime.getValue()));
        repaint();
    };
}

void TrackComponent::layoutButtons()
{
    addAndMakeVisible(m_playButton);
    m_playButton.setColour(juce::ComboBox::outlineColourId, m_color);
    m_playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    m_playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    m_playButton.onClick = [&]() {
        m_processorRef.play(m_id);
    };

    addAndMakeVisible(m_stopButton);
    m_stopButton.setColour(juce::ComboBox::outlineColourId, m_color);
    m_stopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    m_stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    m_stopButton.onClick = [&]() {
        m_processorRef.stop(m_id);
    };
}

void TrackComponent::update()
{
    repaint();
}


void TrackComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::white);
    drawTrack(g);
}

void TrackComponent::drawTrack(juce::Graphics &g) {

    if (m_thumbnail.getNumChannels() != 0) {
        g.setColour(m_color);
        m_thumbnail.drawChannels (g, m_thumbnailBounds,m_thumbnail.getTotalLength() * m_startTime.getValue(), m_thumbnail.getTotalLength() * m_endTime.getValue(), 1.0f);

        g.setColour(juce::Colours::black);
        const auto playPosition =  m_processorRef.getTrackPlayhead(m_id) * m_thumbnailBounds.toFloat().getWidth() + m_thumbnailBounds.toFloat().getX();
        g.drawLine( playPosition, m_thumbnailBounds.toFloat().getY(),playPosition, m_thumbnailBounds.toFloat().getHeight()*1.25f);
    } else {
        g.setColour (juce::Colours::darkgrey);
        g.fillRect (m_thumbnailBounds);

        //animate(g);

        g.setColour(juce::Colours::black);
        g.setFont (32.0f);
        g.drawText("Drag and Drop .wav File", m_thumbnailBounds, juce::Justification::centred, false);


    }
}

void TrackComponent::animate(juce::Graphics &g)
{
    g.setColour(juce::Colours::white);
    float animationProgress = static_cast<float>(getFrameCounter() % 100) / 100.0f;
    float maxRadius = static_cast<float>(m_thumbnailBounds.getWidth()) * 0.4f;
    float currentRadius = animationProgress * maxRadius;
    float alpha = 1.0f - animationProgress;
    g.setOpacity(alpha);

    juce::Point<int> center(m_thumbnailBounds.getCentreX(), m_thumbnailBounds.getCentreY());
    g.fillEllipse(center.toFloat().x - currentRadius, center.toFloat().y - currentRadius, 2.0f * currentRadius, 2.0f * currentRadius);
}


void TrackComponent::resized() {

    m_thumbnailBounds = juce::Rectangle<int>(50, 50, getWidth()- 100, 200);
    const auto sliderY = m_thumbnailBounds.getBottom() + 10;
    const auto buttonY = sliderY + 10;
    constexpr auto sliderSize = 75;
    constexpr auto buttonSize = 50;
    constexpr auto margin = 5;
    m_gainSlider.setBounds(50, sliderY, sliderSize, sliderSize);
    m_speedSlider.setBounds(m_gainSlider.getRight() + 20, sliderY, sliderSize, sliderSize);
    m_startTime.setBounds(m_thumbnailBounds.getX(), m_gainSlider.getBottom() + 5, m_thumbnailBounds.getWidth(), 20);
    m_endTime.setBounds(m_thumbnailBounds.getX(), m_startTime.getBottom() + 5, m_thumbnailBounds.getWidth(), 20);
    m_playButton.setBounds(m_thumbnailBounds.getRight() - buttonSize*2 - margin*2, buttonY, buttonSize, buttonSize);
    m_stopButton.setBounds(m_thumbnailBounds.getRight() - buttonSize - margin, buttonY, buttonSize, buttonSize);
    //DBG("RESIZED");
}

void TrackComponent::filesDropped(const StringArray &files, int x, int y) {
    if (isInterestedInFileDrag(files)) {
        loadFileThumbnail(*files.begin());
        m_processorRef.loadFile(m_id, files.begin()->toStdString());
    }
    auto& random = juce::Random::getSystemRandom();
    const juce::Colour color (random.nextInt (256),
            random.nextInt (256),
            random.nextInt (256));
    m_color = color;
    layoutSliders();
    layoutButtons();
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
