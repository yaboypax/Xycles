//
// Created by Audio on 6/25/2025.
//

#pragma once
#include "../effects/Delay.h"
#include "../effects/Granulator.h"
#include "../effects/Reverb.h"
#include "../static/Utilities.h"
#include "../subcomponents/PlayButton.h"
#include "../subcomponents/StopButton.h"

#include "TrackKnob.h"
#include "TrackSlider.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "rust_part.h"
#include <JuceHeader.h>

class TrackComponent : public juce::AnimatedAppComponent,
                       public juce::FileDragAndDropTarget {
public:
  explicit TrackComponent(rust_part::Engine *);
  void paint(juce::Graphics &) override;
  void resized() override;

  void setTheme(Theme theme);
  void loadTheme();

  void filesDropped(const StringArray &files, int x, int y) override;
  bool isInterestedInFileDrag(const StringArray &files) override;
  void loadFileThumbnail(const String &fileName);
  void drawTrack(juce::Graphics &g);
  void animate(juce::Graphics &g);
  void update() override;

  void play();
  void stop();

  void setLength(double length);
  void mouseEnter(const MouseEvent &e) override;

  std::function<void()> minusCallback;
  std::function<void(double)> loopMasterCallback;
  std::function<void()> soloCallback;
  std::function<void()> muteCallback;

private:
  rust_part::Engine *m_engine;
  void layoutSliders();
  void layoutButtons();

  Theme m_theme = Theme::Light;
  juce::Colour m_color;

  PlayMode m_playMode = PlayMode::Regular;
  void togglePlayMode();

  juce::Label m_playHeadLabel;
  TrackKnob m_gainSlider, m_speedSlider;
  juce::Label m_gainLabel, m_speedLabel;

  Granulator m_granulator;
  juce::TextButton m_granulatorButton;

  Freeverb m_reverb;
  juce::TextButton m_reverbButton;

  TrackSlider m_startTime, m_endTime;
  Xycles::PlayButton m_playButton;
  Xycles::StopButton m_stopButton;

  juce::AudioFormatManager m_formatManager;
  juce::AudioThumbnailCache m_thumbnailCache;
  juce::AudioThumbnail m_thumbnail;
  juce::Rectangle<int> m_thumbnailBounds;
  juce::TextButton m_loadButton;
  std::unique_ptr<juce::FileChooser> m_fileChooser;

  juce::TextButton m_minusButton, m_loopMasterButton, m_soloButton,
      m_muteButton;
};
