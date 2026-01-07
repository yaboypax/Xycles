//
// Created by R&F Dev on 7/22/25.
//

#include "TopBarComponent.h"
#include "juce_graphics/juce_graphics.h"

TopBarComponent::TopBarComponent() {
  addAndMakeVisible(m_globalPlay);
  m_globalPlay.onClick = [&] {
    if (onGlobalPlay) {
      onGlobalPlay();
      m_isPlaying = true;
    }
  };

  addAndMakeVisible(m_globalStop);
  m_globalStop.onClick = [&] {
    if (onGlobalStop) {
      onGlobalStop();
      m_isPlaying = false;
    }

    m_recorderComponent.setVisible(false);
    m_recorderComponent.stopRecording();
  };

  addAndMakeVisible(m_globalRecord);
  m_globalRecord.onClick = [&] {
    if (onGlobalPlay && !m_isPlaying) {
      onGlobalPlay();
      m_isPlaying = true;
    }

    if (onGlobalStop && m_recorderComponent.isRecording()) {
      onGlobalStop();
      m_isPlaying = false;

      m_recorderComponent.setVisible(false);
      m_recorderComponent.stopRecording();
      return;
    }

    m_recorderComponent.setVisible(true);
    m_recorderComponent.startRecording();
  };

  addChildComponent(m_recorderComponent);
  setOpaque(false);
  setAlpha(0.8f);

  addAndMakeVisible(m_lightDarkButton);
  m_lightDarkButton.setButtonText("LIGHT");
  m_lightDarkButton.onClick = [&] {
    if (onLightDarkToggle)
      onLightDarkToggle();

    if (m_lightDarkButton.getButtonText() == "LIGHT") {
      m_theme = Theme::Dark;
      m_lightDarkButton.setButtonText("DARK");
    } else if (m_lightDarkButton.getButtonText() == "DARK") {
      m_lightDarkButton.setButtonText("LIGHT");
      m_theme = Theme::Light;
    }

    m_globalPlay.setTheme(m_theme);
    m_globalStop.setTheme(m_theme);
    m_globalRecord.setTheme(m_theme);

    repaint();
  };

  addAndMakeVisible(m_audioButton);
  m_audioButton.setButtonText("AUDIO");
  m_audioButton.onClick = [&] {
    if (onAudioSettings)
      onAudioSettings();
  };
}

void TopBarComponent::paint(juce::Graphics &g) {

  if (m_lightDarkButton.getButtonText() == "LIGHT") {
    g.setColour(juce::Colours::black);
  } else if (m_lightDarkButton.getButtonText() == "DARK") {
    g.setColour(juce::Colours::white);
  }

  g.setFont(25.f);
  g.drawText("Xycles", getLocalBounds(), juce::Justification::centred, true);

  g.setColour(juce::Colours::black);
  g.drawLine(0.f, static_cast<float>(getHeight()),
             static_cast<float>(getWidth()), static_cast<float>(getHeight()),
             2.f);
}
void TopBarComponent::resized() {
  constexpr int margin = 5;
  const int buttonSize = getHeight() - margin * 2;

  m_globalPlay.setBounds(getWidth() - buttonSize * 2 - margin * 2, margin,
                         buttonSize, buttonSize);
  m_globalStop.setBounds(getWidth() - buttonSize - margin, margin, buttonSize,
                         buttonSize);
  m_globalRecord.setBounds(getWidth() - buttonSize * 3 - margin * 3, margin,
                           buttonSize, buttonSize);

  m_lightDarkButton.setBounds(margin, margin, buttonSize * 2, buttonSize);
  m_audioButton.setBounds(m_lightDarkButton.getRight() + margin, margin,
                          buttonSize * 2, buttonSize);
  constexpr int recordingSize = 200;
  m_recorderComponent.setBounds(m_globalRecord.getX() - recordingSize, 0,
                                recordingSize, getHeight());
}

void TopBarComponent::setRecorder(std::shared_ptr<AudioRecorder> recorder) {
  m_recorderComponent.setRecorder(std::move(recorder));
}
