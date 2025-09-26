#pragma once
#include <JuceHeader.h>

class AudioRecorder final
{
public:
    explicit AudioRecorder();
    ~AudioRecorder();

    void startRecording (const File& file);
    void stop();

    void prepareToPlay(double sampleRate);
    void releaseResources();

    bool isRecording() const;
    void processBlock (const juce::AudioSourceChannelInfo& bufferToFill) const;


private:
    TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double m_sampleRate = 0.0;
    int64 nextSampleNum = 0;

    CriticalSection writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

