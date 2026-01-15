#include "EffectRack.h"

EffectRack::EffectRack() {
  addAndMakeVisible(m_granulator);
  addAndMakeVisible(m_reverb);
  addAndMakeVisible(m_delay);

  addAndMakeVisible(m_granulatorButton);
  m_granulatorButton.setColour(juce::TextButton::buttonColourId,
                               juce::Colours::transparentWhite);
  m_granulatorButton.setColour(juce::ComboBox::outlineColourId,
                               juce::Colours::transparentWhite);
  m_granulatorButton.toBack();
  m_granulatorButton.onClick = [&] {
    m_granulator.cycleWindowState();
    resized();
    repaint();
  };

  m_granulator.powerButtonCallback = [&] {
    if (playbackCallback)
      playbackCallback();
  };

  addAndMakeVisible(m_reverbButton);
  m_reverbButton.setColour(juce::TextButton::buttonColourId,
                           juce::Colours::transparentWhite);
  m_reverbButton.setColour(juce::ComboBox::outlineColourId,
                           juce::Colours::transparentWhite);
  m_reverbButton.toBack();
  m_reverbButton.onClick = [&] {
    m_reverb.cycleWindowState();
    resized();
    repaint();
  };

  addAndMakeVisible(m_delayButton);
  m_delayButton.setColour(juce::TextButton::buttonColourId,
                          juce::Colours::transparentWhite);
  m_delayButton.setColour(juce::ComboBox::outlineColourId,
                          juce::Colours::transparentWhite);
  m_delayButton.toBack();
  m_delayButton.onClick = [&] {
    m_delay.cycleWindowState();
    resized();
    repaint();
  };
}

void EffectRack::resized() {
  int granulatorWidth = 20;
  if (m_granulator.getWindowState() == EffectWindowState::Full) {
    granulatorWidth = 460;
  }
  m_granulator.setBounds(0, 0, granulatorWidth, getHeight());
  m_granulatorButton.setBounds(m_granulator.getBounds());

  int reverbWidth = 20;
  if (m_reverb.getWindowState() == EffectWindowState::Full) {
    reverbWidth = 260;
  }
  m_reverb.setBounds(m_granulator.getRight() + 40, 0, reverbWidth, getHeight());
  m_reverbButton.setBounds(m_reverb.getBounds());

  int delayWidth = 20;
  if (m_delay.getWindowState() == EffectWindowState::Full) {
    delayWidth = 260;
  }

  m_delay.setBounds(m_reverb.getRight() + 40, 0, delayWidth, getHeight());
  m_delayButton.setBounds(m_delay.getBounds());
}

void EffectRack::setEngine(rust_part::Engine *engine) {
  m_granulator.setEngine(engine);
  m_reverb.setEngine(engine);
  m_delay.setEngine(engine);
}

void EffectRack::setTheme(const Theme theme) {
  m_granulator.setTheme(theme);
  m_reverb.setTheme(theme);
  m_delay.setTheme(theme);
}

void EffectRack::setColor(const juce::Colour color) {
  m_granulator.setColor(color);
  m_reverb.setColor(color);
  m_delay.setColor(color);
}
