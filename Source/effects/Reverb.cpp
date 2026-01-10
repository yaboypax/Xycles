#include "Reverb.h"

Freeverb::Freeverb() {
  setInterceptsMouseClicks(false, true);
  m_color = juce::Colours::black;

  loadTheme();
  layoutSliders();
}

void Freeverb::resized() {

  constexpr int sliderY = 20, sliderSize = 75, spacer = 25;
  constexpr int labelY = sliderSize + sliderY;

  m_reverbAmount.setBounds(0, sliderY, sliderSize, sliderSize);
  m_reverbAmountLabel.setBounds(m_reverbAmount.getX(), labelY, sliderSize,
                                spacer);

  m_reverbSize.setBounds(m_reverbAmount.getRight() + 20, sliderY, sliderSize,
                         sliderSize);
  m_reverbSizeLabel.setBounds(m_reverbSize.getX(), labelY, sliderSize, spacer);

  m_reverbDamp.setBounds(m_reverbSize.getRight() + 20, sliderY, sliderSize,
                         sliderSize);
  m_reverbDampLabel.setBounds(m_reverbDamp.getX(), labelY, sliderSize, spacer);

  m_reverbLabel.setBounds(m_reverbAmount.getX(), 0,
                          m_reverbDamp.getRight() - m_reverbAmount.getX(),
                          spacer);
}

void Freeverb::setTheme(const Theme theme) {
  m_theme = theme;

  loadTheme();

  m_reverbAmount.setTheme(m_theme);
  m_reverbDamp.setTheme(m_theme);
  m_reverbSize.setTheme(m_theme);
}

void Freeverb::setEngine(rust_part::Engine *engine) { m_engine = engine; }
void Freeverb::setColor(const juce::Colour color) {
  m_color = color;
  layoutSliders();
}

void Freeverb::layoutSliders() {

  addAndMakeVisible(m_reverbLabel);
  m_reverbLabel.setText("Reverb", juce::dontSendNotification);
  m_reverbLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_reverbLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_reverbAmount);
  m_reverbAmount.setRange(0.0, 1.0, 0.01);
  m_reverbAmount.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_reverbAmount.setTrackColor(m_color);
  m_reverbAmount.onValueChange = [&]() {
    m_engine->set_reverb_wet(static_cast<float>(m_reverbAmount.getValue()));
  };
  m_reverbAmount.setValue(0.0);

  addAndMakeVisible(m_reverbAmountLabel);
  m_reverbAmountLabel.attachToComponent(&m_reverbAmount, false);
  m_reverbAmountLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
  m_reverbAmountLabel.setText("Dry/Wet", juce::dontSendNotification);
  m_reverbAmountLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_reverbSize);
  m_reverbSize.setRange(0.0, 1.0, 0.01);
  m_reverbSize.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_reverbSize.setTrackColor(m_color);
  m_reverbSize.onValueChange = [&]() {
    m_engine->set_reverb_size(static_cast<float>(m_reverbSize.getValue()));
  };
  m_reverbSize.setValue(0.5);

  addAndMakeVisible(m_reverbSizeLabel);
  m_reverbSizeLabel.attachToComponent(&m_reverbSize, false);
  m_reverbSizeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_reverbSizeLabel.setText("Room Size", juce::dontSendNotification);
  m_reverbSizeLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_reverbDamp);
  m_reverbDamp.setRange(0.0, 1.0, 0.01);
  m_reverbDamp.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_reverbDamp.setTrackColor(m_color);
  m_reverbDamp.onValueChange = [&]() {
    m_engine->set_reverb_damp(static_cast<float>(m_reverbDamp.getValue()));
  };
  m_reverbDamp.setValue(0.0);

  addAndMakeVisible(m_reverbDampLabel);
  m_reverbDampLabel.attachToComponent(&m_reverbDamp, false);
  m_reverbDampLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_reverbDampLabel.setText("Dampening", juce::dontSendNotification);
  m_reverbDampLabel.setJustificationType(juce::Justification::centredTop);
}

void Freeverb::loadTheme() {
  switch (m_theme) {
  case Theme::Light: {

    m_reverbLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_reverbDampLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
    m_reverbSizeLabel.setColour(juce::Label::textColourId,
                                juce::Colours::black);
    m_reverbAmountLabel.setColour(juce::Label::textColourId,
                                  juce::Colours::black);

    break;
  }
  case Theme::Dark: {

    m_reverbLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    m_reverbDampLabel.setColour(juce::Label::textColourId,
                                juce::Colours::white);
    m_reverbSizeLabel.setColour(juce::Label::textColourId,
                                juce::Colours::white);
    m_reverbAmountLabel.setColour(juce::Label::textColourId,
                                  juce::Colours::white);
    break;
  }
  }
  repaint();
}
