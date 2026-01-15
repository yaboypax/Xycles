#include "EffectComponent.h"

EffectComponent::EffectComponent() {
  setInterceptsMouseClicks(false, true);
  m_color = juce::Colours::black;

  addAndMakeVisible(m_powerButton);
  m_powerButton.setToggleable(true);
  m_powerButton.setClickingTogglesState(true);
  m_powerButton.onClick = [&]() {
    auto enabled = m_powerButton.getToggleState();
    for (auto child : getChildren()) {
      child->setEnabled(enabled);
      if (!enabled)
        m_powerButton.setEnabled(true);
    }

    if (powerButtonCallback) {
      powerButtonCallback();
    }
  };
}
EffectComponent::~EffectComponent() = default;

void EffectComponent::paint(juce::Graphics &g) {
  if (getWindowState() == Minimized) {
    g.fillCheckerBoard(getLocalBounds().toFloat(), 4.f, 4.f,
                       m_color.withAlpha(0.3f), juce::Colours::white);
  }
}

void EffectComponent::resized() {
  constexpr int buttonSize = 20;
  m_powerButton.setBounds(0, 0, buttonSize, buttonSize);
}

void EffectComponent::setEngine(rust_part::Engine *engine) {
  m_engine = engine;
}
void EffectComponent::setColor(const juce::Colour color) {
  m_color = color;
  layoutSliders();
  resized();
}

void EffectComponent::setWindowState(const EffectWindowState windowState) {
  m_windowState = windowState;
}

void EffectComponent::cycleWindowState() {
  switch (m_windowState) {
  case Minimized:
    setWindowState(Full);
    layoutSliders();
    addAndMakeVisible(m_powerButton);
    break;
  case Small:
    setWindowState(Full);
    layoutSliders();
    addAndMakeVisible(m_powerButton);
    break;
  case Full:
    setWindowState(Minimized);
    removeAllChildren();
    addAndMakeVisible(m_powerButton);
    break;
  }

  resized();
  repaint();
}

EffectWindowState EffectComponent::getWindowState() { return m_windowState; }
