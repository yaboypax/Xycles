#pragma once
#include <JuceHeader.h>
//#include "../Assets/DemoUtilities.h"
//#include "../Assets/AudioLiveScrollingDisplay.h"

//==============================================================================
/** A simple class that acts as an AudioIODeviceCallback and writes the
    incoming audio data to a WAV file.
*/
class AudioRecorder final
{
public:
    explicit AudioRecorder ()
    {
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
                    //thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
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
    //AudioThumbnail& thumbnail;
    TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double m_sampleRate = 0.0;
    int64 nextSampleNum = 0;

    CriticalSection writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

//==============================================================================
class RecordingThumbnail final : public Component,
                                 private ChangeListener
{
public:
    RecordingThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener (this);
    }

    ~RecordingThumbnail() override
    {
        thumbnail.removeChangeListener (this);
    }

    AudioThumbnail& getAudioThumbnail()     { return thumbnail; }

    void setDisplayFullThumbnail (bool displayFull)
    {
        displayFullThumb = displayFull;
        repaint();
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
        g.setColour (Colours::lightgrey);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto endTime = displayFullThumb ? thumbnail.getTotalLength()
                                            : jmax (30.0, thumbnail.getTotalLength());

            auto thumbArea = getLocalBounds();
            thumbnail.drawChannels (g, thumbArea.reduced (2), 0.0, endTime, 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No file recorded)", getLocalBounds(), Justification::centred, 2);
        }
    }

private:
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache  { 10 };
    AudioThumbnail thumbnail            { 512, formatManager, thumbnailCache };

    bool displayFullThumb = false;

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordingThumbnail)
};

class LiveScrollingAudioDisplay final : public AudioVisualiserComponent,
                                        public AudioIODeviceCallback
{
public:
    LiveScrollingAudioDisplay()  : AudioVisualiserComponent (1)
    {
        setSamplesPerBlock (256);
        setBufferSize (1024);
    }

    //==============================================================================
    void audioDeviceAboutToStart (AudioIODevice*) override
    {
        clear();
    }

    void audioDeviceStopped() override
    {
        clear();
    }

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData, int numInputChannels,
                                           float* const* outputChannelData, int numOutputChannels,
                                           int numberOfSamples, const AudioIODeviceCallbackContext& context) override
    {
        ignoreUnused (context);

        for (int i = 0; i < numberOfSamples; ++i)
        {
            float inputSample = 0;

            for (int chan = 0; chan < numInputChannels; ++chan)
                if (const float* inputChannel = inputChannelData[chan])
                    inputSample += inputChannel[i];  // find the sum of all the channels

            inputSample *= 10.0f; // boost the level to make it more easily visible.

            pushSample (&inputSample, 1);
        }

        // We need to clear the output buffers before returning, in case they're full of junk..
        for (int j = 0; j < numOutputChannels; ++j)
            if (float* outputChannel = outputChannelData[j])
                zeromem (outputChannel, (size_t) numberOfSamples * sizeof (float));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveScrollingAudioDisplay)
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
        addAndMakeVisible (liveAudioScroller);

        addAndMakeVisible (explanationLabel);
        explanationLabel.setFont (FontOptions (15.0f, Font::plain));
        explanationLabel.setJustificationType (Justification::topLeft);
        explanationLabel.setEditable (false, false, false);
        explanationLabel.setColour (TextEditor::textColourId, Colours::black);
        explanationLabel.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

        addAndMakeVisible (recordButton);
        recordButton.setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
        recordButton.setColour (TextButton::textColourOnId, Colours::black);


        addAndMakeVisible (recordingThumbnail);

        setSize (500, 500);
    }

    ~AudioRecordingDemo() override
    {
    }

    void paint (Graphics& g) override
    {
    }

    void resized() override
    {
        auto area = getLocalBounds();

        liveAudioScroller .setBounds (area.removeFromTop (80).reduced (8));
        recordingThumbnail.setBounds (area.removeFromTop (80).reduced (8));
        recordButton      .setBounds (area.removeFromTop (36).removeFromLeft (140).reduced (8));
        explanationLabel  .setBounds (area.reduced (8));
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

        lastRecording = parentDir.getNonexistentChildFile ("JUCE Demo Audio Recording", ".wav");

        m_recorder->startRecording (lastRecording);

        recordButton.setButtonText ("Stop");
        recordingThumbnail.setDisplayFullThumbnail (false);
    }

    void stopRecording()
    {
        m_recorder->stop();

        chooser.launchAsync (  FileBrowserComponent::saveMode
                             | FileBrowserComponent::canSelectFiles
                             | FileBrowserComponent::warnAboutOverwriting,
                             [this] (const FileChooser& c)
                             {
                                 if (FileInputStream inputStream (lastRecording); inputStream.openedOk())
                                     if (const auto outputStream = makeOutputStream (c.getURLResult()))
                                         outputStream->writeFromInputStream (inputStream, -1);

                                  recordButton.setButtonText ("Record");
                                  recordingThumbnail.setDisplayFullThumbnail (true);
                              });
    }

    void setRecorder(std::shared_ptr<AudioRecorder> recorder) {
        m_recorder = std::move(recorder);
    }


private:
    // if this PIP is running inside the demo runner, we'll use the shared device manager instead
   #ifndef JUCE_DEMO_RUNNER
    AudioDeviceManager audioDeviceManager;
   #else
    AudioDeviceManager& audioDeviceManager { getSharedAudioDeviceManager (1, 0) };
   #endif

    std::shared_ptr<AudioRecorder> m_recorder;
    LiveScrollingAudioDisplay liveAudioScroller;
    RecordingThumbnail recordingThumbnail;

    Label explanationLabel { {},
                             "This page demonstrates how to record a wave file from the live audio input.\n\n"
                             "After you are done with your recording you can choose where to save it." };
    TextButton recordButton { "Record" };
    File lastRecording;
    FileChooser chooser { "Output file...", File::getCurrentWorkingDirectory().getChildFile ("recording.wav"), "*.wav" };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioRecordingDemo)
};
