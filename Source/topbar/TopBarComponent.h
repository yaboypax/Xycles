//
// Created by R&F Dev on 7/22/25.
//

#pragma once
#include <JuceHeader.h>

#include "../AudioRecorder.h"
#include "../subcomponents/PlayButton.h"
#include "../subcomponents/RecordButton.h"
#include "../subcomponents/StopButton.h"
#include "RecordingIndicator.h"

class TopBarComponent : public juce::Component {
public:
  TopBarComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;
  void setRecorder(std::shared_ptr<AudioRecorder> recorder);

  std::function<void()> onGlobalPlay = nullptr;
  std::function<void()> onGlobalStop = nullptr;
  std::function<void()> onLightDarkToggle = nullptr;

private:
  Xycles::StopButton m_globalStop;
  Xycles::PlayButton m_globalPlay;
  Xycles::RecordButton m_globalRecord;
  RecordingIndicator m_recorderComponent;
  Theme m_theme = Light;
  bool m_isPlaying = false;

  juce::TextButton m_lightDarkButton;
};
