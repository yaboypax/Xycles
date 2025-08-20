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
    setAlpha(0.8f);

    m_color = juce::Colours::black;
    setOpaque (true);
    layoutSliders();
    layoutButtons();

    m_formatManager.registerBasicFormats();

}

void TrackComponent::layoutSliders()
{

    addAndMakeVisible(m_playHeadLabel);
    m_playHeadLabel.setText("PlayHead", juce::dontSendNotification);
    m_playHeadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_playHeadLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_granulatorLabel);
    m_granulatorLabel.setText("Granulator", juce::dontSendNotification);
    m_granulatorLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_granulatorLabel.setJustificationType(juce::Justification::centred);


    addAndMakeVisible(m_gainSlider);
    m_gainSlider.setRange(0.01, 1.00, 0.01);
    m_gainSlider.setTrackColor( m_color);
    m_gainSlider.onValueChange = [&]() {
        m_processorRef.setGain(m_id, static_cast<float>(m_gainSlider.getValue()));
    };

    addAndMakeVisible(m_gainLabel);
    m_gainLabel.attachToComponent(&m_gainSlider, false);
    m_gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_gainLabel.setText("Gain", juce::dontSendNotification);
    m_gainLabel.setJustificationType(juce::Justification::centredTop);



    addAndMakeVisible(m_speedSlider);
    m_speedSlider.setRange(-2.0, 2.0, 0.01);
    m_speedSlider.setValue(1.0);
    m_speedSlider.setTrackColor( m_color);
    m_speedSlider.onValueChange = [&]() {
        m_processorRef.setSpeed(m_id, static_cast<float>(m_speedSlider.getValue()));
    };

    addAndMakeVisible(m_speedLabel);
    m_speedLabel.attachToComponent(&m_speedSlider, false);
    m_speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_speedLabel.setText("Speed", juce::dontSendNotification);
    m_speedLabel.setJustificationType(juce::Justification::centredTop);


    addAndMakeVisible(m_grainSpeed);
    m_grainSpeed.setRange(-2.0, 2.0, 0.01);
    m_grainSpeed.setValue(1.0);
    m_grainSpeed.setTrackColor( m_color);
    m_grainSpeed.onValueChange = [&]() {
        m_processorRef.setGrainSpeed(m_id, static_cast<float>(m_grainSpeed.getValue()));
    };

    addAndMakeVisible(m_grainSpeedLabel);
    m_grainSpeedLabel.attachToComponent(&m_grainSpeed, false);
    m_grainSpeedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_grainSpeedLabel.setText("Grain Speed", juce::dontSendNotification);
    m_grainSpeedLabel.setJustificationType(juce::Justification::centredTop);

    addAndMakeVisible(m_grainLength);
    m_grainLength.setRange(20, 10000, 1);
    m_grainLength.setTrackColor( m_color);
    m_grainLength.onValueChange = [&]() {
        m_processorRef.setGrainLength(m_id, static_cast<float>(m_grainLength.getValue()));
    };

    addAndMakeVisible(m_grainLengthLabel);
    m_grainLengthLabel.attachToComponent(&m_grainLength, false);
    m_grainLengthLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_grainLengthLabel.setText("Grain Length", juce::dontSendNotification);
    m_grainLengthLabel.setJustificationType(juce::Justification::centredTop);


    addAndMakeVisible(m_grainOverlap);
    m_grainOverlap.setRange(0.1, 10.0, 0.01);
    m_grainOverlap.setTrackColor( m_color);
    m_grainOverlap.onValueChange = [&]() {
        m_processorRef.setGrainOverlap(m_id, static_cast<float>(m_grainOverlap.getValue()));
    };

    addAndMakeVisible(m_grainOverlapLabel);
    m_grainOverlapLabel.attachToComponent(&m_grainOverlap, false);
    m_grainOverlapLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_grainOverlapLabel.setText("Grain Overlap", juce::dontSendNotification);
    m_grainOverlapLabel.setJustificationType(juce::Justification::centredTop);

    addAndMakeVisible(m_grainSpread);
    m_grainSpread.setRange(0.0, 2.0, 0.01);
    m_grainSpread.setTrackColor( m_color);
    m_grainSpread.onValueChange = [&]() {
        m_processorRef.setGrainOverlap(m_id, static_cast<float>(m_grainOverlap.getValue()));
    };

    addAndMakeVisible(m_grainSpreadLabel);
    m_grainSpreadLabel.attachToComponent(&m_grainSpread, false);
    m_grainSpreadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_grainSpreadLabel.setText("Grain Spread", juce::dontSendNotification);
    m_grainSpreadLabel.setJustificationType(juce::Justification::centredTop);


    addAndMakeVisible(m_grainsCount);
    m_grainsCount.setRange(1, 12, 1);
    m_grainOverlap.setTrackColor( m_color);
    m_grainOverlap.onValueChange = [&]() {
        m_processorRef.setGrainOverlap(m_id, static_cast<float>(m_grainOverlap.getValue()));
    };

    addAndMakeVisible(m_grainsCountLabel);
    m_grainsCountLabel.attachToComponent(&m_grainsCount, false);
    m_grainsCountLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_grainsCountLabel.setText("Grains", juce::dontSendNotification);
    m_grainsCountLabel.setJustificationType(juce::Justification::centredTop);



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
    //
    // m_grainLength.setEnabled(false);
    // m_grainOverlap.setEnabled(false);
    // m_grainSpeed.setEnabled(false);
}

void TrackComponent::layoutButtons()
{
    addAndMakeVisible(m_playButton);
    m_playButton.setColour(juce::ComboBox::outlineColourId, m_color);
    m_playButton.onClick = [&]() {
        m_processorRef.play(m_id);
    };

    addAndMakeVisible(m_stopButton);
    m_stopButton.setColour(juce::ComboBox::outlineColourId, m_color);
    m_stopButton.onClick = [&]() {
        m_processorRef.stop(m_id);
    };

    //addAndMakeVisible(m_granulatorButton);
    m_granulatorButton.setColour(juce::ComboBox::outlineColourId, m_color);
    m_granulatorButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    m_granulatorButton.onClick = [&]() {
        togglePlayMode();
    };
}

void TrackComponent::togglePlayMode() {
    if (m_playMode == PlayMode::Regular) {
        m_playMode = PlayMode::Granular;

        m_grainLength.setEnabled(true);
        m_grainOverlap.setEnabled(true);
        m_grainSpeed.setEnabled(true);
    } else if (m_playMode == PlayMode::Granular) {
        m_playMode = PlayMode::Regular;

        m_grainLength.setEnabled(false);
        m_grainOverlap.setEnabled(false);
        m_grainSpeed.setEnabled(false);
    }
}

void TrackComponent::update()
{
    repaint();
}


void TrackComponent::paint(juce::Graphics &g) {
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

    m_thumbnailBounds = juce::Rectangle<int>(50, 5, getWidth()- 100, 200);
    const auto sliderY = m_thumbnailBounds.getBottom() + 20;
    const auto buttonY = sliderY + 10;
    constexpr auto sliderSize = 75;
    constexpr auto buttonSize = 50;
    constexpr auto margin = 5;
    constexpr auto spacer = 25;


    // Playhead

    m_playHeadLabel.setBounds(m_thumbnailBounds.getX(), m_thumbnailBounds.getBottom() , sliderSize + 98, spacer);
    m_gainSlider.setBounds(50, sliderY, sliderSize, sliderSize);
    m_speedSlider.setBounds(m_gainSlider.getRight() + 20, sliderY, sliderSize, sliderSize);
    const int labelY = m_gainSlider.getBottom();
    m_gainLabel.setBounds(50,labelY , sliderSize, spacer);
    m_speedLabel.setBounds(m_speedSlider.getX(), labelY, sliderSize, spacer);

    // Grain
    m_grainSpeed.setBounds(m_speedSlider.getRight()+75, sliderY, sliderSize, sliderSize);
    m_grainSpeedLabel.setBounds(m_grainSpeed.getX(), labelY, sliderSize, spacer);

    m_grainLength.setBounds(m_grainSpeed.getRight()+20, sliderY, sliderSize, sliderSize);
    m_grainLengthLabel.setBounds(m_grainLength.getX(), labelY, sliderSize, spacer);

    m_grainOverlap.setBounds(m_grainLength.getRight()+20, sliderY, sliderSize, sliderSize);
    m_grainOverlapLabel.setBounds(m_grainOverlap.getX(), labelY, sliderSize, spacer);

    m_grainsCount.setBounds(m_grainOverlap.getRight()+20, sliderY, sliderSize, sliderSize);
    m_grainsCountLabel.setBounds(m_grainsCount.getX(), labelY, sliderSize, spacer);

    m_grainSpread.setBounds(m_grainsCount.getRight()+20, sliderY, sliderSize, sliderSize);
    m_grainSpreadLabel.setBounds(m_grainSpread.getX(), labelY, sliderSize, spacer);

    m_granulatorLabel.setBounds(m_grainSpeed.getX(), m_thumbnailBounds.getBottom(), m_grainSpread.getRight() - m_grainSpeed.getX(), spacer);
    m_granulatorButton.setBounds(m_grainSpeed.getX() - spacer, m_thumbnailBounds.getBottom() + 2, (m_grainSpread.getRight() + spacer) - (m_grainSpeed.getX() - spacer), (m_grainOverlapLabel.getBottom() + 2) - (m_thumbnailBounds.getBottom() + 2) );


    m_startTime.setBounds(m_thumbnailBounds.getX(), m_gainSlider.getBottom() + spacer, m_thumbnailBounds.getWidth(), 20);
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
    resized();
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

void TrackComponent::mouseEnter(const MouseEvent &e)
{

}
