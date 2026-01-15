#pragma once

#include "../static/Utilities.h"
#include "../subcomponents/PowerButton.h"
#include <JuceHeader.h>
#include <rust_part.h>

class EffectComponent : public juce::Component {
public:
  EffectComponent();
  virtual ~EffectComponent() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  virtual void setTheme(const Theme) {}
  void setEngine(rust_part::Engine *engine);
  void setColor(const juce::Colour color);

  void setWindowState(const EffectWindowState);
  EffectWindowState getWindowState();
  void cycleWindowState();
  std::function<void()> powerButtonCallback;

protected:
  virtual void layoutSliders() {}
  virtual void loadTheme() {}

  rust_part::Engine *m_engine;
  Theme m_theme = Theme::Light;
  EffectWindowState m_windowState = EffectWindowState::Full;
  juce::Colour m_color;
  PowerButton m_powerButton;
};
