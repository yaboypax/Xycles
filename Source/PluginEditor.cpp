#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)//,gainAttachment(p.state, "gain", gainSlider)
{

  addAndMakeVisible(gainSlider);
  gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  gainSlider.setRange(0.01, 1.00, 0.01);
  gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider.onValueChange = [&]() {
      processorRef.setGain(static_cast<float>(gainSlider.getValue()));
    };

    addAndMakeVisible(speedSlider);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setRange(0.0, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.onValueChange = [&]() {
        processorRef.setSpeed(static_cast<float>(speedSlider.getValue()));
    };


    addAndMakeVisible(textEditor);
    textEditor.onReturnKey = [&]() {
        processorRef.loadFile(textEditor.getText().toStdString());
    };

  setSize(400, 300);
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));



}

void XyclesAudioProcessorEditor::resized() {

    gainSlider.setBounds(50, 50, getWidth()-100, 30);
    speedSlider.setBounds(gainSlider.getX(), gainSlider.getBottom() + 20,gainSlider.getWidth(), gainSlider.getHeight());
    textEditor.setBounds(speedSlider.getX(), speedSlider.getBottom() + 20,speedSlider.getWidth(), speedSlider.getHeight());
}
