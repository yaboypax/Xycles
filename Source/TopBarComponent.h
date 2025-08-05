//
// Created by R&F Dev on 7/22/25.
//

#pragma once
#include <JuceHeader.h>
#include "subcomponents/PlayButton.h"
#include "subcomponents/RecordButton.h"
#include "subcomponents/StopButton.h"

class TopBarComponent : public juce::Component
{
public:
    TopBarComponent();
    void paint (juce::Graphics& g) override;
    void resized() override;

    std::function<void()> onGlobalPlay = nullptr;
    std::function<void()> onGlobalStop = nullptr;
    std::function<void()> onGlobalRecord = nullptr;


private:
    Xycles::StopButton m_globalStop;
    Xycles::PlayButton m_globalPlay;
    Xycles::RecordButton m_globalRecord;
};