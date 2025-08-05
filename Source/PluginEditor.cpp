#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
#include "BinaryData.h"
namespace
{
    constexpr int kTopBarHeight = 50;
}
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
    : AudioProcessorEditor(&p), m_trackEditor(p), processorRef(p) {
    setSize(1200, 1000);
    setResizable(true, true);
    addAndMakeVisible(m_trackViewport);
    m_trackEditor.setSize(1200, 1000);
    m_trackViewport.setViewedComponent(&m_trackEditor, false);

    addAndMakeVisible(m_topBar);
    m_topBar.onGlobalPlay = [&] {
        p.playAll();
    };
    m_topBar.onGlobalStop = [&] {
        p.stopAll();
        m_recorder.setVisible(false);
        m_recorder.record();
    };

    addChildComponent(m_recorder);
    m_topBar.onGlobalRecord = [&] {
        m_recorder.setVisible(true);
        m_recorder.record();
    };

    m_backgroundImage = juce::Image(
        juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize));
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {
}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::white);
    g.drawImage(m_backgroundImage, getLocalBounds().toFloat() );
    }

void XyclesAudioProcessorEditor::resized() {

    m_topBar.setBounds(0, 0, getWidth(), kTopBarHeight);
    m_trackViewport.setBounds(getLocalBounds().withTrimmedTop(kTopBarHeight));
    m_trackEditor.setSize(getWidth(), m_trackEditor.getHeight());

    m_recorder.setBounds(m_trackViewport.getBounds());


}