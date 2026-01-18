#include "Compressor.h"

Compressor::Compressor() : EffectComponent() {
  loadTheme();
  layoutSliders();
  m_powerButton.setToggleState(true,
                               juce::NotificationType::dontSendNotification);
}

void Compressor::resized() {

  EffectComponent::resized();

  constexpr int sliderY = 20, sliderSize = 75, spacer = 25;
  constexpr int labelY = sliderSize + sliderY;

  if (getWindowState() != Full) {
    return;
  }

  m_compressorThreshold.setBounds(0, sliderY, sliderSize, sliderSize);
  m_compressorThresholdLabel.setBounds(m_compressorThreshold.getX(), labelY,
                                       sliderSize, spacer);

  m_compressorAttack.setBounds(m_compressorThreshold.getRight() + 20, sliderY,
                               sliderSize, sliderSize);
  m_compressorAttackLabel.setBounds(m_compressorAttack.getX(), labelY,
                                    sliderSize, spacer);

  m_compressorRelease.setBounds(m_compressorAttack.getRight() + 20, sliderY,
                                sliderSize, sliderSize);
  m_compressorReleaseLabel.setBounds(m_compressorRelease.getX(), labelY,
                                     sliderSize, spacer);

  m_compressorLabel.setBounds(
      m_compressorThreshold.getX(), 0,
      m_compressorRelease.getRight() - m_compressorThreshold.getX(), spacer);
}

void Compressor::setTheme(const Theme theme) {
  m_theme = theme;

  loadTheme();

  m_compressorThreshold.setTheme(m_theme);
  m_compressorRelease.setTheme(m_theme);
  m_compressorAttack.setTheme(m_theme);
}
void Compressor::layoutSliders() {

  addAndMakeVisible(m_compressorLabel);
  m_compressorLabel.setText("Compressor", juce::dontSendNotification);
  m_compressorLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_compressorLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_compressorThreshold);
  m_compressorThreshold.setRange(0.0, 1.0, 0.01);
  m_compressorThreshold.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_compressorThreshold.setTrackColor(m_color);
  m_compressorThreshold.onValueChange = [&]() {
    m_engine->set_delay_wet(
        static_cast<float>(m_compressorThreshold.getValue()));
  };
  m_compressorThreshold.setValue(0.0);

  addAndMakeVisible(m_compressorThresholdLabel);
  m_compressorThresholdLabel.attachToComponent(&m_compressorThreshold, false);
  m_compressorThresholdLabel.setColour(juce::Label::textColourId,
                                       juce::Colours::black);
  m_compressorThresholdLabel.setText("Threshold", juce::dontSendNotification);
  m_compressorThresholdLabel.setJustificationType(
      juce::Justification::centredTop);

  addAndMakeVisible(m_compressorAttack);
  m_compressorAttack.setRange(50.0, 2000.0, 10.0);
  m_compressorAttack.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_compressorAttack.setTrackColor(m_color);
  m_compressorAttack.onValueChange = [&]() {
    m_engine->set_delay_time(static_cast<float>(m_compressorAttack.getValue()));
  };
  m_compressorAttack.setValue(100.0);

  addAndMakeVisible(m_compressorAttackLabel);
  m_compressorAttackLabel.attachToComponent(&m_compressorAttack, false);
  m_compressorAttackLabel.setColour(juce::Label::textColourId,
                                    juce::Colours::black);
  m_compressorAttackLabel.setText("Attack", juce::dontSendNotification);
  m_compressorAttackLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_compressorRelease);
  m_compressorRelease.setRange(0.0, 1.0, 0.01);
  m_compressorRelease.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_compressorRelease.setTrackColor(m_color);
  m_compressorRelease.onValueChange = [&]() {
    m_engine->set_delay_feedback(
        static_cast<float>(m_compressorRelease.getValue()));
  };
  m_compressorRelease.setValue(0.0);

  addAndMakeVisible(m_compressorReleaseLabel);
  m_compressorReleaseLabel.attachToComponent(&m_compressorRelease, false);
  m_compressorReleaseLabel.setColour(juce::Label::textColourId,
                                     juce::Colours::black);
  m_compressorReleaseLabel.setText("Release", juce::dontSendNotification);
  m_compressorReleaseLabel.setJustificationType(
      juce::Justification::centredTop);

  m_compressorLabel.toBack();
}

void Compressor::loadTheme() {
  switch (m_theme) {
  case Theme::Light: {

    m_compressorLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
    m_compressorReleaseLabel.setColour(juce::Label::textColourId,
                                       juce::Colours::black);
    m_compressorAttackLabel.setColour(juce::Label::textColourId,
                                      juce::Colours::black);
    m_compressorThresholdLabel.setColour(juce::Label::textColourId,
                                         juce::Colours::black);

    break;
  }
  case Theme::Dark: {

    m_compressorLabel.setColour(juce::Label::textColourId,
                                juce::Colours::white);

    m_compressorReleaseLabel.setColour(juce::Label::textColourId,
                                       juce::Colours::white);
    m_compressorAttackLabel.setColour(juce::Label::textColourId,
                                      juce::Colours::white);
    m_compressorThresholdLabel.setColour(juce::Label::textColourId,
                                         juce::Colours::white);
    break;
  }
  }
  repaint();
}
