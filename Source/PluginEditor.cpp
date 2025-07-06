#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
: AudioProcessorEditor(&p), m_trackEditor(p), processorRef(p)
{
    setSize(1200, 1000);
    addAndMakeVisible(m_trackViewport);
    m_trackEditor.setSize(1200, 1000);
    m_trackViewport.setViewedComponent(&m_trackEditor, false);
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

void XyclesAudioProcessorEditor::resized() {
    m_trackViewport.setBounds(getLocalBounds());
}