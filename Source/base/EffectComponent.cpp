#include "EffectComponent.h"

EffectComponent::EffectComponent() {
  setInterceptsMouseClicks(false, true);
  m_color = juce::Colours::black;
}
EffectComponent::~EffectComponent() = default;

void EffectComponent::paint(juce::Graphics &g) {
  if (getWindowState() == Minimized) {
    g.setColour(m_color);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 6.f, 2.f);
  }
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
    break;
  case Small:
    setWindowState(Full);
    layoutSliders();
    break;
  case Full:
    setWindowState(Minimized);
    removeAllChildren();
    break;
  }

  resized();
  repaint();
}

EffectWindowState EffectComponent::getWindowState() { return m_windowState; }
