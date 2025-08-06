//
// Created by R&F Dev on 7/22/25.
//

#include "TopBarComponent.h"

TopBarComponent::TopBarComponent()
{
    addAndMakeVisible(m_globalPlay);
    m_globalPlay.onClick = [&]{
        if (onGlobalPlay) {
            onGlobalPlay();
            m_isPlaying = true;
        }
    };

    addAndMakeVisible(m_globalStop);
    m_globalStop.onClick = [&]{
        if (onGlobalStop) {
            onGlobalStop();
            m_isPlaying = false;
        }

        m_recorderComponent.setVisible(false);
        m_recorderComponent.stopRecording();
    };

    addAndMakeVisible(m_globalRecord);
    m_globalRecord.onClick = [&]{
        if (onGlobalPlay && !m_isPlaying)
            onGlobalPlay();

        m_recorderComponent.setVisible(true);
        m_recorderComponent.startRecording();
    };

    addChildComponent(m_recorderComponent);
    setOpaque(false);
    setAlpha(0.8f);
}

void TopBarComponent::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::black);
    g.setFont (25.f);
    g.drawText("Xycles", getLocalBounds(), juce::Justification::centred, true);

    g.setColour (juce::Colours::black);
    g.drawLine(0.f,static_cast<float>(getHeight()), static_cast<float>(getWidth()), static_cast<float>(getHeight()), 2.f);
}
void TopBarComponent::resized()
{
    constexpr int margin = 5;
    const int buttonSize = getHeight() - margin*2;

    m_globalPlay.setBounds(getWidth() - buttonSize*2 - margin*2, margin, buttonSize, buttonSize);
    m_globalStop.setBounds(getWidth() - buttonSize - margin, margin, buttonSize, buttonSize);
    m_globalRecord.setBounds(getWidth() - buttonSize*3 - margin*3, margin, buttonSize, buttonSize);

    constexpr int recordingSize = 200;
    m_recorderComponent.setBounds(m_globalRecord.getX() - recordingSize, 0, recordingSize, getHeight());
}

void TopBarComponent::setRecorder(std::shared_ptr<AudioRecorder> recorder) {
    m_recorderComponent.setRecorder(std::move(recorder));
}
