#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
: AudioProcessorEditor(&p), processorRef(p), m_trackComponent(p)
{
    setSize(1200, 800);
    addAndMakeVisible(&m_trackComponent);
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

void XyclesAudioProcessorEditor::resized() {
    m_trackComponent.setBounds(getLocalBounds());
}