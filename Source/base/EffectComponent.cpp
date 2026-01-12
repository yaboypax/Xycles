#include "EffectComponent.h"

EffectComponent::EffectComponent() {
  setInterceptsMouseClicks(false, true);
  m_color = juce::Colours::black;
}
EffectComponent::~EffectComponent() = default;

void EffectComponent::setEngine(rust_part::Engine *engine) {
  m_engine = engine;
}
void EffectComponent::setColor(const juce::Colour color) {
  m_color = color;
  layoutSliders();
}
