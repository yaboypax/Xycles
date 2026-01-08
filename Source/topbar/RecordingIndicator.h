//
// Created by Audio on 8/6/2025.
//
#pragma once

#include "../AudioRecorder.h"
#include <JuceHeader.h>

class RecordingIndicator final : public juce::Component {
public:
  RecordingIndicator();

  void paint(Graphics &g) override;
  void resized() override;

  void startRecording();
  void stopRecording();
  void setRecorder(std::shared_ptr<AudioRecorder> recorder);
  bool isRecording() const;

private:
  AudioDeviceManager m_audioDeviceManager;
  std::shared_ptr<AudioRecorder> m_recorder;

  Label m_recordingLabel{{}, "Recording..."};
  File m_lastRecording;
  FileChooser m_chooser{"Output file...",
                        File::getSpecialLocation(File::userMusicDirectory)
                            .getChildFile("recording.wav"),
                        "*.wav"};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingIndicator)
};
