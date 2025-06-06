#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p),
      gainAttachment(p.state, "gain", gainSlider) {
  juce::ignoreUnused(processorRef);

  addAndMakeVisible(gainSlider);
  gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 300);
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
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
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
}
