#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p),
      gainAttachment(p.state, "gain", gainSlider) {

  addAndMakeVisible(gainSlider);
  gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  gainSlider.setRange(0.01, 1.00, 0.01);
  gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

  setSize(400, 300);
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  auto r = getLocalBounds().reduced(10);
  r.removeFromTop(20);
  auto sliderArea = r.removeFromTop(120);
  sliderArea.removeFromLeft(50);
  sliderArea.removeFromRight(50);

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Gain", sliderArea.removeFromTop(20),
                   juce::Justification::centred, 1);
  gainSlider.setBounds(sliderArea.removeFromTop(30));
}

void XyclesAudioProcessorEditor::resized() {
}
