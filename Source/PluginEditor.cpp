#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
XyclesAudioProcessorEditor::XyclesAudioProcessorEditor(
    XyclesAudioProcessor &p)
: AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 1000);
    addAndMakeVisible(&m_plusButton);
    m_plusButton.setButtonText("+");
    m_plusButton.onClick = [&] {
        processorRef.addTrack();
        std::unique_ptr<TrackComponent> track = std::make_unique<TrackComponent>(processorRef, m_trackCount);
        addAndMakeVisible(track.get());
        m_tracks.push_back(std::move(track));
        m_trackCount++;
        resized();
    };
}

XyclesAudioProcessorEditor::~XyclesAudioProcessorEditor() {}

//==============================================================================
void XyclesAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

void XyclesAudioProcessorEditor::resized() {
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexBox.alignContent = juce::FlexBox::AlignContent::stretch;
    flexBox.alignItems = juce::FlexBox::AlignItems::stretch;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

    flexBox.items.clear();

    for (auto& track : m_tracks) {
        flexBox.items.add(
            juce::FlexItem(*track)
                .withFlex(.2f)
                .withMargin(10.0f)
        );
    }

    flexBox.items.add(
        juce::FlexItem(m_plusButton)
            .withFlex(0.01f)
            .withMargin(2.0f)
    );


    flexBox.performLayout(getLocalBounds());
}