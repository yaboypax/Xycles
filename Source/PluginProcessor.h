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

  void setGain(float gain);
  void setSpeed(float speed);
  void setStart(float start);
  void setEnd(float end);
  
  void loadFile(const std::string& path);
  void play();
  void stop();

  juce::AudioProcessorValueTreeState state;
private:
  rust::Vec<float> m_interleavedBuffer;
  rust::Box<rust_part::Engine> m_rustEngine = rust_part::new_engine();
  //==============================================================================

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XyclesAudioProcessor)
};
