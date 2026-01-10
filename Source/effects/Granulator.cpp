#include "Granulator.h"

Granulator::Granulator() {
  setInterceptsMouseClicks(false, true);
  m_color = juce::Colours::black;

  loadTheme();
  layoutSliders();
}

void Granulator::resized() {

  constexpr int sliderY = 20, sliderSize = 75, spacer = 25;
  constexpr int labelY = sliderSize + sliderY;

  m_grainSpeed.setBounds(0, sliderY, sliderSize, sliderSize);
  m_grainSpeedLabel.setBounds(m_grainSpeed.getX(), labelY, sliderSize, spacer);

  m_grainLength.setBounds(m_grainSpeed.getRight() + 20, sliderY, sliderSize,
                          sliderSize);
  m_grainLengthLabel.setBounds(m_grainLength.getX(), labelY, sliderSize,
                               spacer);

  m_grainOverlap.setBounds(m_grainLength.getRight() + 20, sliderY, sliderSize,
                           sliderSize);
  m_grainOverlapLabel.setBounds(m_grainOverlap.getX(), labelY, sliderSize,
                                spacer);

  m_grainsCount.setBounds(m_grainOverlap.getRight() + 20, sliderY, sliderSize,
                          sliderSize);
  m_grainsCountLabel.setBounds(m_grainsCount.getX(), labelY, sliderSize,
                               spacer);

  m_grainSpread.setBounds(m_grainsCount.getRight() + 20, sliderY, sliderSize,
                          sliderSize);
  m_grainSpreadLabel.setBounds(m_grainSpread.getX(), labelY, sliderSize,
                               spacer);

  m_granulatorLabel.setBounds(m_grainSpeed.getX(), 0,
                              m_grainSpread.getRight() - m_grainSpeed.getX(),
                              spacer);
}

void Granulator::setTheme(const Theme theme) {
  m_theme = theme;

  loadTheme();

  m_grainSpeed.setTheme(m_theme);
  m_grainLength.setTheme(m_theme);
  m_grainOverlap.setTheme(m_theme);
  m_grainsCount.setTheme(m_theme);
  m_grainSpread.setTheme(m_theme);
}

void Granulator::setEngine(rust_part::Engine *engine) { m_engine = engine; }
void Granulator::setColor(const juce::Colour color) { m_color = color; }

void Granulator::layoutSliders() {
  addAndMakeVisible(m_granulatorLabel);
  m_granulatorLabel.setText("Granulator", juce::dontSendNotification);
  m_granulatorLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_granulatorLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_grainSpeed);
  m_grainSpeed.setRange(-2.0, 2.0, 0.01);
  m_grainSpeed.setValue(1.0);
  m_grainSpeed.setTrackColor(m_color);
  m_grainSpeed.onValueChange = [&]() {
    m_engine->set_grain_speed(static_cast<float>(m_grainSpeed.getValue()));
  };

  addAndMakeVisible(m_grainSpeedLabel);
  m_grainSpeedLabel.attachToComponent(&m_grainSpeed, false);
  m_grainSpeedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_grainSpeedLabel.setText("Grain Speed", juce::dontSendNotification);
  m_grainSpeedLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_grainLength);
  m_grainLength.setRange(20, 10000, 1);
  m_grainLength.setTrackColor(m_color);
  m_grainLength.onValueChange = [&]() {
    m_engine->set_grain_length(static_cast<float>(m_grainLength.getValue()));
  };

  addAndMakeVisible(m_grainLengthLabel);
  m_grainLengthLabel.attachToComponent(&m_grainLength, false);
  m_grainLengthLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_grainLengthLabel.setText("Grain Length", juce::dontSendNotification);
  m_grainLengthLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_grainOverlap);
  m_grainOverlap.setRange(0.1, 10.0, 0.01);
  m_grainOverlap.setTrackColor(m_color);
  m_grainOverlap.onValueChange = [&]() {
    m_engine->set_grain_overlap(static_cast<float>(m_grainOverlap.getValue()));
  };

  addAndMakeVisible(m_grainOverlapLabel);
  m_grainOverlapLabel.attachToComponent(&m_grainOverlap, false);
  m_grainOverlapLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
  m_grainOverlapLabel.setText("Grain Overlap", juce::dontSendNotification);
  m_grainOverlapLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_grainSpread);
  m_grainSpread.setRange(0.0, 2.0, 0.01);
  m_grainSpread.setTrackColor(m_color);
  m_grainSpread.onValueChange = [&]() {
    m_engine->set_grain_spread(static_cast<float>(m_grainSpread.getValue()));
  };

  addAndMakeVisible(m_grainSpreadLabel);
  m_grainSpreadLabel.attachToComponent(&m_grainSpread, false);
  m_grainSpreadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_grainSpreadLabel.setText("Grain Spread", juce::dontSendNotification);
  m_grainSpreadLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_grainsCount);
  m_grainsCount.setRange(1, 12, 1);
  m_grainsCount.setTrackColor(m_color);
  m_grainsCount.onValueChange = [&]() {
    m_engine->set_grain_count(static_cast<float>(m_grainsCount.getValue()));
  };

  addAndMakeVisible(m_grainsCountLabel);
  m_grainsCountLabel.attachToComponent(&m_grainsCount, false);
  m_grainsCountLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_grainsCountLabel.setText("Grains", juce::dontSendNotification);
  m_grainsCountLabel.setJustificationType(juce::Justification::centredTop);
}

void Granulator::loadTheme() {
  switch (m_theme) {
  case Theme::Light: {
    m_grainSpeedLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
    m_granulatorLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
    m_grainSpreadLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::black);
    m_grainLengthLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::black);
    m_grainOverlapLabel.setColour(juce::Label::textColourId,
                                  juce::Colours::black);
    m_grainsCountLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::black);
    break;
  }
  case Theme::Dark: {
    m_grainSpeedLabel.setColour(juce::Label::textColourId,
                                juce::Colours::white);
    m_granulatorLabel.setColour(juce::Label::textColourId,
                                juce::Colours::white);
    m_grainSpreadLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::white);
    m_grainLengthLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::white);
    m_grainOverlapLabel.setColour(juce::Label::textColourId,
                                  juce::Colours::white);
    m_grainsCountLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::white);
    break;
  }
  }
  repaint();
}
