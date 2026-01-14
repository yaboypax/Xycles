#include "Delay.h"

HDelay::HDelay() : EffectComponent() {
  loadTheme();
  layoutSliders();
}

void HDelay::resized() {

  constexpr int sliderY = 20, sliderSize = 75, spacer = 25;
  constexpr int labelY = sliderSize + sliderY;

  m_delayAmount.setBounds(0, sliderY, sliderSize, sliderSize);
  m_delayAmountLabel.setBounds(m_delayAmount.getX(), labelY, sliderSize,
                               spacer);

  m_delayTime.setBounds(m_delayAmount.getRight() + 20, sliderY, sliderSize,
                        sliderSize);
  m_delayTimeLabel.setBounds(m_delayTime.getX(), labelY, sliderSize, spacer);

  m_delayFeedback.setBounds(m_delayTime.getRight() + 20, sliderY, sliderSize,
                            sliderSize);
  m_delayFeedbackLabel.setBounds(m_delayFeedback.getX(), labelY, sliderSize,
                                 spacer);

  m_delayLabel.setBounds(m_delayAmount.getX(), 0,
                         m_delayFeedback.getRight() - m_delayAmount.getX(),
                         spacer);
}

void HDelay::setTheme(const Theme theme) {
  m_theme = theme;

  loadTheme();

  m_delayAmount.setTheme(m_theme);
  m_delayFeedback.setTheme(m_theme);
  m_delayTime.setTheme(m_theme);
}
void HDelay::layoutSliders() {

  addAndMakeVisible(m_delayLabel);
  m_delayLabel.setText("Delay", juce::dontSendNotification);
  m_delayLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_delayLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_delayAmount);
  m_delayAmount.setRange(0.0, 1.0, 0.01);
  m_delayAmount.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_delayAmount.setTrackColor(m_color);
  m_delayAmount.onValueChange = [&]() {
    m_engine->set_delay_wet(static_cast<float>(m_delayAmount.getValue()));
  };
  m_delayAmount.setValue(0.0);

  addAndMakeVisible(m_delayAmountLabel);
  m_delayAmountLabel.attachToComponent(&m_delayAmount, false);
  m_delayAmountLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_delayAmountLabel.setText("Dry/Wet", juce::dontSendNotification);
  m_delayAmountLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_delayTime);
  m_delayTime.setRange(50.0, 2000.0, 10.0);
  m_delayTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_delayTime.setTrackColor(m_color);
  m_delayTime.onValueChange = [&]() {
    m_engine->set_delay_time(static_cast<float>(m_delayTime.getValue()));
  };
  m_delayTime.setValue(100.0);

  addAndMakeVisible(m_delayTimeLabel);
  m_delayTimeLabel.attachToComponent(&m_delayTime, false);
  m_delayTimeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_delayTimeLabel.setText("Time", juce::dontSendNotification);
  m_delayTimeLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_delayFeedback);
  m_delayFeedback.setRange(0.0, 1.0, 0.01);
  m_delayFeedback.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_delayFeedback.setTrackColor(m_color);
  m_delayFeedback.onValueChange = [&]() {
    m_engine->set_delay_feedback(
        static_cast<float>(m_delayFeedback.getValue()));
  };
  m_delayFeedback.setValue(0.0);

  addAndMakeVisible(m_delayFeedbackLabel);
  m_delayFeedbackLabel.attachToComponent(&m_delayFeedback, false);
  m_delayFeedbackLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::black);
  m_delayFeedbackLabel.setText("Feedback", juce::dontSendNotification);
  m_delayFeedbackLabel.setJustificationType(juce::Justification::centredTop);
}

void HDelay::loadTheme() {
  switch (m_theme) {
  case Theme::Light: {

    m_delayLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_delayFeedbackLabel.setColour(juce::Label::textColourId,
                                   juce::Colours::black);
    m_delayTimeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_delayAmountLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::black);

    break;
  }
  case Theme::Dark: {

    m_delayLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    m_delayFeedbackLabel.setColour(juce::Label::textColourId,
                                   juce::Colours::white);
    m_delayTimeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    m_delayAmountLabel.setColour(juce::Label::textColourId,
                                 juce::Colours::white);
    break;
  }
  }
  repaint();
}
