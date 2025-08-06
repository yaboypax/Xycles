//
// Created by R&F Dev on 7/22/25.
//

#pragma once
#include <JuceHeader.h>

#include "AudioRecorder.h"
#include "subcomponents/PlayButton.h"
#include "subcomponents/RecordButton.h"
#include "subcomponents/StopButton.h"

class TopBarComponent : public juce::Component
{
public:
    TopBarComponent();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void setRecorder(std::shared_ptr<AudioRecorder> recorder);

    std::function<void()> onGlobalPlay = nullptr;
    std::function<void()> onGlobalStop = nullptr;

private:
    Xycles::StopButton m_globalStop;
    Xycles::PlayButton m_globalPlay;
    Xycles::RecordButton m_globalRecord;
    AudioRecordingDemo m_recorderComponent;
    bool m_isPlaying = false;
};