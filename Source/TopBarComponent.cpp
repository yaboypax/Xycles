//
// Created by R&F Dev on 7/22/25.
//

#include "TopBarComponent.h"

TopBarComponent::TopBarComponent()
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

void TopBarComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
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
}