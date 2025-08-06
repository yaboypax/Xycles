//
// Created by Audio on 8/6/2025.
//

#include "RecordingIndicator.h"

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

RecordingIndicator::RecordingIndicator()
{
    setOpaque (false);
    addAndMakeVisible (m_recordingLabel);
    m_recordingLabel.setFont (FontOptions (32.0f, Font::plain));
    m_recordingLabel.setJustificationType (Justification::centredTop);
    m_recordingLabel.setEditable (false, false, false);
    m_recordingLabel.setColour (Label::textColourId, Colours::red);
}

void RecordingIndicator::paint (Graphics& g)
{
    g.fillAll (Colours::transparentWhite);
}

void RecordingIndicator::resized()
{
    m_recordingLabel.setBounds (getLocalBounds().reduced (8));
}

void RecordingIndicator::startRecording()
{
    if (! RuntimePermissions::isGranted (RuntimePermissions::writeExternalStorage))
    {
        SafePointer<RecordingIndicator> safeThis (this);

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


void RecordingIndicator::stopRecording()
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

void RecordingIndicator::setRecorder(std::shared_ptr<AudioRecorder> recorder)
{
    m_recorder = std::move(recorder);
}