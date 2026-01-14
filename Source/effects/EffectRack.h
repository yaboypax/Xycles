#pragma once
#include <JuceHeader.h>

class EffectRack : public juce::Component {
public:
  EffectRack() {}
  void paint(juce::Graphics &g) override { g.fillAll(juce::Colours::grey); }
  void resized() override {}

private:
};
