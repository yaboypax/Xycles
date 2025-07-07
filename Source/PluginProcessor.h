#pragma once

#include <JuceHeader.h>
#include "rust_part.h"

//==============================================================================
class XyclesAudioProcessor final : public juce::AudioProcessor {
public:
  //==============================================================================
  XyclesAudioProcessor();
  ~XyclesAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  using AudioProcessor::processBlock;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  void addTrack();
  void removeTrack();

  void setGain(size_t index, float gain);
  void setSpeed(size_t index, float speed);
  void setStart(size_t index, float start);
  void setEnd(size_t index, float end);

  float getTrackPlayhead(size_t index);
  
  void loadFile(size_t index, const std::string& path);
  void play(size_t index);
  void stop(size_t index);

  juce::AudioProcessorValueTreeState state;
private:
  rust::Vec<float> m_interleavedBuffer;
  std::vector<rust::Box<rust_part::Engine>> m_trackEngines;
  //==============================================================================

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessor)
};
