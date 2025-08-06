#pragma once
#include <JuceHeader.h>
//#include "../Assets/DemoUtilities.h"
//#include "../Assets/AudioLiveScrollingDisplay.h"

//==============================================================================

class AudioRecorder final
{
public:
    explicit AudioRecorder () {
        backgroundThread.startThread();
    }

    ~AudioRecorder()
    {
        stop();
    }

    //==============================================================================
    void startRecording (const File& file)
    {
        stop();

        if (m_sampleRate > 0 && file.deleteFile())
        {
            if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream()))
            {
                WavAudioFormat wavFormat;
                if (const auto writer = wavFormat.createWriterFor (fileStream.get(), m_sampleRate, 2, 16, {}, 0))
                {
                    fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

                    // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                    // write the data to disk on our background thread.
                    threadedWriter.reset (new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768));

                    // Reset our recording thumbnail
                    //m_thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    // And now, swap over our active writer pointer so that the audio callback will start using it..
                    const ScopedLock sl (writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        // First, clear this pointer to stop the audio callback from using our writer object..
        {
            const ScopedLock sl (writerLock);
            activeWriter = nullptr;
        }

        // Now we can delete the writer object. It's done in this order because the deletion could
        // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
        // the audio callback while this happens.
        threadedWriter.reset();
    }


    void prepareToPlay(const double sampleRate) {
        m_sampleRate = sampleRate;
    }

    void releaseResources()
    {
        m_sampleRate = 0;
    }

    bool isRecording() const {
        return activeWriter.load() != nullptr;
    }

    void processBlock (AudioSampleBuffer& buffer)
    {
        const ScopedLock sl (writerLock);
        if (activeWriter.load() != nullptr)
            activeWriter.load()->write ((const float**)buffer.getArrayOfReadPointers(), buffer.getNumSamples());

    }


private:
    TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double m_sampleRate = 0.0;
    int64 nextSampleNum = 0;

    CriticalSection writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

static std::unique_ptr<InputSource> makeInputSource (const URL& url)
{
        if (const auto doc = AndroidDocument::fromDocument (url))
            return std::make_unique<AndroidDocumentInputSource> (doc);

    #if ! JUCE_IOS
        if (url.isLocalFile())
            return std::make_unique<FileInputSource> (url.getLocalFile());
    #endif

        return std::make_unique<URLInputSource> (url);
}

static std::unique_ptr<OutputStream> makeOutputStream (const URL& url)
{
        if (const auto doc = AndroidDocument::fromDocument (url))
            return doc.createOutputStream();

    #if ! JUCE_IOS
        if (url.isLocalFile())
            return url.getLocalFile().createOutputStream();
    #endif

        return url.createOutputStream();
}

class AudioRecordingDemo final : public Component
{
public:
    AudioRecordingDemo()
    {
        setOpaque (false);
        addAndMakeVisible (m_recordingLabel);
        m_recordingLabel.setFont (FontOptions (32.0f, Font::plain));
        m_recordingLabel.setJustificationType (Justification::centredTop);
        m_recordingLabel.setEditable (false, false, false);
        m_recordingLabel.setColour (Label::textColourId, Colours::red);
    }

    ~AudioRecordingDemo() override
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::transparentWhite);
    }

    void resized() override
    {
        m_recordingLabel.setBounds (getLocalBounds().reduced (8));
    }

    void startRecording()
    {
        if (! RuntimePermissions::isGranted (RuntimePermissions::writeExternalStorage))
        {
            SafePointer<AudioRecordingDemo> safeThis (this);

            RuntimePermissions::request (RuntimePermissions::writeExternalStorage,
                                         [safeThis] (bool granted) mutable
                                         {
                                             if (granted)
                                                 safeThis->startRecording();
                                         });
            return;
        }

        #if (JUCE_ANDROID || JUCE_IOS)
                auto parentDir = File::getSpecialLocation (File::tempDirectory);
        #else
                auto parentDir = File::getSpecialLocation (File::userDocumentsDirectory);
        #endif

        m_lastRecording = parentDir.getNonexistentChildFile ("JUCE Demo Audio Recording", ".wav");
        m_recorder->startRecording (m_lastRecording);
    }

    void stopRecording()
    {
        m_recorder->stop();

        m_chooser.launchAsync (  FileBrowserComponent::saveMode
                             | FileBrowserComponent::canSelectFiles
                             | FileBrowserComponent::warnAboutOverwriting,
                             [this] (const FileChooser& c)
                             {
                                 if (FileInputStream inputStream (m_lastRecording); inputStream.openedOk())
                                     if (const auto outputStream = makeOutputStream (c.getURLResult()))
                                         outputStream->writeFromInputStream (inputStream, -1);
                              });
    }

    void setRecorder(std::shared_ptr<AudioRecorder> recorder) {
        m_recorder = std::move(recorder);
    }


private:
    AudioDeviceManager m_audioDeviceManager;
    std::shared_ptr<AudioRecorder> m_recorder;

    Label m_recordingLabel { {},"Recording..." };
    File m_lastRecording;
    FileChooser m_chooser { "Output file...", File::getCurrentWorkingDirectory().getChildFile ("recording.wav"), "*.wav" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioRecordingDemo)
};
