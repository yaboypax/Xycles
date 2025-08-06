#include "PluginProcessor.h"

#include <filesystem>

#include "PluginEditor.h"

//==============================================================================
XyclesAudioProcessor::XyclesAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      state(*this, nullptr, "state",
            {std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{"gain", 1}, "Gain",
                juce::NormalisableRange<float>(0.0f, 4.0f), 1.0f)})
{


}

XyclesAudioProcessor::~XyclesAudioProcessor() {

}
//==============================================================================
const juce::String XyclesAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool XyclesAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool XyclesAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool XyclesAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double XyclesAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int XyclesAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int XyclesAudioProcessor::getCurrentProgram() { return 0; }

void XyclesAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String XyclesAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void XyclesAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

//==============================================================================
void XyclesAudioProcessor::prepareToPlay(double sampleRate,
                                              const int samplesPerBlock)
{
  for (int i = 0; i < samplesPerBlock*2; i++) {
    m_interleavedBuffer.push_back(0.f);
  }
  m_recorder->prepareToPlay(sampleRate);
}

void XyclesAudioProcessor::releaseResources() {
  m_recorder->releaseResources();
}

bool XyclesAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void XyclesAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);
  juce::ScopedNoDenormals noDenormals;

  if (m_trackEngines.empty())
    return;
  buffer.clear();
  using Format = AudioData::Format<AudioData::Float32, AudioData::NativeEndian>;
  juce::AudioData::interleaveSamples (AudioData::NonInterleavedSource<Format> { buffer.getArrayOfReadPointers(), buffer.getNumChannels() },
                                      AudioData::InterleavedDest<Format>      { &m_interleavedBuffer[0],   buffer.getNumChannels() }, buffer.getNumSamples());

  for (auto& track: m_trackEngines) {
    track->process_block(m_interleavedBuffer);
  }


  juce::AudioData::deinterleaveSamples(AudioData::InterleavedSource<Format>{&m_interleavedBuffer[0], buffer.getNumChannels()},
                                        AudioData::NonInterleavedDest<Format>{buffer.getArrayOfWritePointers(), buffer.getNumChannels()}, buffer.getNumSamples());

  if (m_recorder->isRecording())
      m_recorder->processBlock(buffer);
}

//==============================================================================
bool XyclesAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *XyclesAudioProcessor::createEditor() {
  return new XyclesAudioProcessorEditor(*this);
}

//==============================================================================
void XyclesAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void XyclesAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new XyclesAudioProcessor();
}

void XyclesAudioProcessor::addTrack() {
  m_trackEngines.push_back(rust_part::new_engine());
}

void XyclesAudioProcessor::removeTrack() {
  m_trackEngines.pop_back();
}


void XyclesAudioProcessor::setGain(size_t index, float gain)
{
  m_trackEngines[index]->set_gain(gain);
}

void XyclesAudioProcessor::setSpeed(size_t index, float speed)
{
  m_trackEngines[index]->set_speed(speed);
}

void XyclesAudioProcessor::setStart(size_t index, float start) {
  m_trackEngines[index]->set_start(start);
}

void XyclesAudioProcessor::setEnd(size_t index, float end) {
  m_trackEngines[index]->set_end(end);
}

void XyclesAudioProcessor::loadFile(size_t index, const std::string& path) {
  rust::Str string = path;
  m_trackEngines[index]->load_audio(path);
}

void XyclesAudioProcessor::play(size_t index) {
  m_trackEngines[index]->play();
}

void XyclesAudioProcessor::stop(size_t index) {
  m_trackEngines[index]->stop();
}

void XyclesAudioProcessor::playAll()
{
  for (auto& track: m_trackEngines)
    track->play();
}

void XyclesAudioProcessor::stopAll()
{
  for (auto& track: m_trackEngines)
    track->stop();

}

float XyclesAudioProcessor::getTrackPlayhead(size_t index) {
  return m_trackEngines[index]->get_playhead();
}