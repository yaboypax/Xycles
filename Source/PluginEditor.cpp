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
    speedSlider.setRange(-2.0, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.onValueChange = [&]() {
        processorRef.setSpeed(static_cast<float>(speedSlider.getValue()));
    };

    addAndMakeVisible(startTime);
    startTime.setSliderStyle(juce::Slider::LinearHorizontal);
    startTime.setRange(0.0, 1.0, 0.01);
    startTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    startTime.onValueChange = [&]() {
        processorRef.setStart(static_cast<float>(startTime.getValue()));
    };

    addAndMakeVisible(endTime);
    endTime.setSliderStyle(juce::Slider::LinearHorizontal);
    endTime.setRange(0.0, 1.0, 0.01);
    endTime.setValue(1.0);
    endTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    endTime.onValueChange = [&]() {
        processorRef.setEnd(static_cast<float>(endTime.getValue()));
    };


    addAndMakeVisible(textEditor);
    textEditor.onReturnKey = [&]() {
        processorRef.loadFile(textEditor.getText().toStdString());
    };

    addAndMakeVisible(playButton);
    playButton.onClick = [&]() {
        processorRef.play();
    };

    addAndMakeVisible(stopButton);
    stopButton.onClick = [&]() {
        processorRef.stop();
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
    playButton.setBounds(textEditor.getX(), textEditor.getBottom() + 20,textEditor.getWidth()/2-10, textEditor.getHeight());
    stopButton.setBounds(playButton.getRight() + 10, textEditor.getBottom() + 20,playButton.getWidth(), textEditor.getHeight());
    startTime.setBounds(playButton.getX(), playButton.getBottom() + 20, playButton.getWidth(), 30);
    endTime.setBounds(stopButton.getX(), stopButton.getBottom() + 20, playButton.getWidth(), 30);
}

void XyclesAudioProcessorEditor::filesDropped(	const StringArray &	files, int	x, int	y ) {
    if (isInterestedInFileDrag(files))
        processorRef.loadFile(files.begin()->toStdString());
}

bool XyclesAudioProcessorEditor::isInterestedInFileDrag(const StringArray &files) {
    if (files.begin()->contains(".wav")) {
        return true;
    } else return false;
}
