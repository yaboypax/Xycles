//
// Created by Audio on 8/5/2025.
//

#include "AudioRecorder.h"

#include <memory>


AudioRecorder::AudioRecorder ()
{
    backgroundThread.startThread();
}

AudioRecorder::~AudioRecorder()
{
    stop();
}

void AudioRecorder::startRecording (const File& file)
{
    stop();

    if (m_sampleRate > 0 && file.deleteFile())
    {
        if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream()))
        {
            WavAudioFormat wavFormat;
            if (const auto writer = wavFormat.createWriterFor (fileStream.release(), m_sampleRate, 2, 16, {}, 0))
            {
                threadedWriter = std::make_unique<AudioFormatWriter::ThreadedWriter> (writer, backgroundThread, 32768);
                nextSampleNum = 0;

                const ScopedLock sl (writerLock);
                activeWriter = threadedWriter.get();
            }
        }
    }
}

void AudioRecorder::stop() {
    {
        const ScopedLock sl (writerLock);
        activeWriter = nullptr;
    }
    threadedWriter.reset();
}


void AudioRecorder::prepareToPlay(const double sampleRate)
{
    m_sampleRate = sampleRate;
}

void AudioRecorder::releaseResources()
{
    m_sampleRate = 0;
}

bool AudioRecorder::isRecording() const
{
    return activeWriter.load() != nullptr;
}

void AudioRecorder::processBlock (AudioSampleBuffer& buffer)
{
    const ScopedLock sl (writerLock);
    if (activeWriter.load() != nullptr)
        activeWriter.load()->write (buffer.getArrayOfReadPointers(), buffer.getNumSamples());
}