//
// Created by Audio on 6/25/2025.
//

#include "TrackComponent.h"

TrackComponent::TrackComponent(rust_part::Engine *engine)
    : m_engine(engine), m_thumbnailCache(5),
      m_thumbnail(512, m_formatManager, m_thumbnailCache) {
  setFramesPerSecond(30);
  setAlpha(0.8f);

  m_color = juce::Colours::black;
  setOpaque(true);

  loadTheme();
  layoutSliders();
  layoutButtons();

  m_effectRack.setEngine(m_engine);
  m_effectRack.playbackCallback = [&] { togglePlayMode(); };

  m_effectViewport.setViewedComponent(&m_effectRack, false);
  m_effectViewport.setScrollBarsShown(false, true);
  addAndMakeVisible(m_effectViewport);

  m_formatManager.registerBasicFormats();
}

void TrackComponent::layoutSliders() {

  addAndMakeVisible(m_playHeadLabel);
  m_playHeadLabel.setText("PlayHead", juce::dontSendNotification);
  m_playHeadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_playHeadLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_gainSlider);
  m_gainSlider.setRange(0.01, 2.00, 0.01);
  m_gainSlider.setValue(1.0);
  m_gainSlider.setTrackColor(m_color);
  m_gainSlider.onValueChange = [&]() {
    m_engine->set_gain(static_cast<float>(m_gainSlider.getValue()));
  };

  addAndMakeVisible(m_gainLabel);
  m_gainLabel.attachToComponent(&m_gainSlider, false);
  m_gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_gainLabel.setText("Gain", juce::dontSendNotification);
  m_gainLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_speedSlider);
  m_speedSlider.setRange(-2.0, 2.0, 0.01);
  m_speedSlider.setValue(1.0);
  m_speedSlider.setTrackColor(m_color);
  m_speedSlider.onValueChange = [&]() {
    m_engine->set_speed(static_cast<float>(m_speedSlider.getValue()));
  };

  addAndMakeVisible(m_speedLabel);
  m_speedLabel.attachToComponent(&m_speedSlider, false);
  m_speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  m_speedLabel.setText("Speed", juce::dontSendNotification);
  m_speedLabel.setJustificationType(juce::Justification::centredTop);

  addAndMakeVisible(m_startTime);
  m_endTime.setTrackDirection(Start);
  m_startTime.setSliderStyle(juce::Slider::LinearHorizontal);
  m_startTime.setRange(0.0, 1.0, 0.001);
  m_startTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_startTime.setTrackColor(m_color);
  m_startTime.onValueChange = [&]() {
    if (m_startTime.getValue() >= m_endTime.getValue()) {
      m_startTime.setValue(m_endTime.getValue() - 0.001);
    }
    m_endTime.setStartPosition(m_startTime.getValue());
    m_engine->set_start(static_cast<float>(m_startTime.getValue()));
    repaint();
  };

  addAndMakeVisible(m_endTime);
  m_endTime.setTrackDirection(End);
  m_endTime.setRange(0.0, 1.0, 0.001);
  m_endTime.setValue(1.0);
  m_endTime.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  m_endTime.setTrackColor(m_color);
  m_endTime.onValueChange = [&]() {
    if (m_endTime.getValue() <= m_startTime.getValue()) {
      m_endTime.setValue(m_startTime.getValue() + 0.001);
    }
    m_startTime.setEndPosition(m_endTime.getValue());
    m_engine->set_end(static_cast<float>(m_endTime.getValue()));
    repaint();
  };
}

void TrackComponent::layoutButtons() {
  addAndMakeVisible(m_playButton);
  m_playButton.setColour(juce::ComboBox::outlineColourId, m_color);
  m_playButton.onClick = [&]() {
    switch (m_playMode) {
    case Regular:
      m_engine->play();
      break;
    case Granular:
      m_engine->grain_play();
      break;
    }
  };

  addAndMakeVisible(m_stopButton);
  m_stopButton.setColour(juce::ComboBox::outlineColourId, m_color);
  m_stopButton.onClick = [&]() { m_engine->stop(); };

  m_playButton.toFront(true);
  m_stopButton.toFront(true);

  addAndMakeVisible(m_loadButton);
  m_loadButton.setColour(juce::TextButton::buttonColourId,
                         juce::Colours::transparentWhite);
  m_loadButton.onClick = [&]() {
    m_fileChooser = std::make_unique<juce::FileChooser>(
        "Select a file", File::getSpecialLocation(File::userHomeDirectory),
        "*.wav;", true, false, nullptr);
    m_fileChooser->launchAsync(juce::FileBrowserComponent::openMode |
                                   juce::FileBrowserComponent::canSelectFiles,
                               [this](const juce::FileChooser &fc) {
                                 if (const auto file = fc.getResult();
                                     file.existsAsFile())
                                   filesDropped({file.getFullPathName()}, 0, 0);
                               });
  };

  addAndMakeVisible(m_loopMasterButton);
  m_loopMasterButton.setButtonText("LOOP");
  m_loopMasterButton.setColour(juce::ComboBox::outlineColourId, m_color);
  m_loopMasterButton.setColour(juce::TextButton::buttonColourId,
                               juce::Colours::transparentWhite);
  m_loopMasterButton.onClick = [&]() {
    if (loopMasterCallback) {
      const auto samples = m_engine->get_length();
      const auto length =
          m_endTime.getValue() * samples - m_startTime.getValue() * samples;
      loopMasterCallback(length);
    }
  };
}

void TrackComponent::togglePlayMode() {
  switch (m_playMode) {
  case Regular: {
    m_playMode = PlayMode::Granular;
    m_engine->grain_play();
    break;
  }
  case Granular: {
    m_playMode = PlayMode::Regular;
    m_engine->play();
    break;
  }
  }
  repaint();
}

void TrackComponent::update() { repaint(); }

void TrackComponent::paint(juce::Graphics &g) { drawTrack(g); }

void TrackComponent::drawTrack(juce::Graphics &g) {

  juce::Colour themeColor;
  switch (m_theme) {
  case Theme::Light: {
    themeColor = juce::Colours::black;
    break;
  }
  case Theme::Dark: {
    themeColor = juce::Colours::white;
    break;
  }
  }

  if (m_thumbnail.getNumChannels() != 0) {
    g.setColour(m_color);
    m_thumbnail.drawChannels(
        g, m_thumbnailBounds,
        m_thumbnail.getTotalLength() * m_startTime.getValue(),
        m_thumbnail.getTotalLength() * m_endTime.getValue(), 1.0f);

    g.setColour(themeColor);
    const auto playPosition =
        m_engine->get_playhead() * m_thumbnailBounds.toFloat().getWidth() +
        m_thumbnailBounds.toFloat().getX();
    g.drawLine(playPosition, m_thumbnailBounds.toFloat().getY(), playPosition,
               m_thumbnailBounds.toFloat().getHeight() * 1.25f);
  } else {
    animate(g);

    g.setColour(themeColor);
    g.setFont(32.0f);
    g.drawText("Click to Select or Drag and Drop .wav File", m_thumbnailBounds,
               juce::Justification::centred, false);
  }
}

void TrackComponent::animate(juce::Graphics &g) {
  switch (m_theme) {
  case Theme::Light: {
    g.setColour(juce::Colours::black);
    break;
  }
  case Theme::Dark: {
    g.setColour(juce::Colours::white);
    break;
  }
  }
  float animationProgress =
      static_cast<float>(getFrameCounter() % 100) / 100.0f;
  float maxRadius = static_cast<float>(m_thumbnailBounds.getWidth()) * 0.4f;
  float currentRadius = animationProgress * maxRadius;
  float alpha = 1.0f - animationProgress;
  g.setOpacity(alpha);

  juce::Point<int> center(m_thumbnailBounds.getCentreX(),
                          m_thumbnailBounds.getCentreY());
  g.drawEllipse(center.toFloat().x - currentRadius,
                center.toFloat().y - currentRadius, 2.0f * currentRadius,
                2.0f * currentRadius, 2.f);
}

void TrackComponent::setTheme(const Theme theme) {
  m_theme = theme;
  loadTheme();
}
void TrackComponent::loadTheme() {
  switch (m_theme) {
  case Theme::Light: {
    m_gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    m_playHeadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    break;
  }
  case Theme::Dark: {
    m_gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    m_speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    m_playHeadLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    break;
  }
  }

  m_playButton.setTheme(m_theme);
  m_stopButton.setTheme(m_theme);
  m_gainSlider.setTheme(m_theme);
  m_speedSlider.setTheme(m_theme);

  m_effectRack.setTheme(m_theme);

  m_startTime.setTheme(m_theme);
  m_endTime.setTheme(m_theme);
  repaint();
}

void TrackComponent::resized() {
  constexpr auto sliderSize = 75;
  constexpr auto buttonSize = 30;
  constexpr auto margin = 5;
  constexpr auto spacer = 25;

  m_thumbnailBounds = juce::Rectangle<int>(50, 5, getWidth() - 100, 200);
  const auto sliderY = m_thumbnailBounds.getBottom() + 20;
  const auto buttonY = m_thumbnailBounds.getBottom() - buttonSize - margin;

  // Playhead

  m_playHeadLabel.setBounds(m_thumbnailBounds.getX(),
                            m_thumbnailBounds.getBottom(), sliderSize + 98,
                            spacer);
  m_gainSlider.setBounds(50, sliderY, sliderSize, sliderSize);
  m_speedSlider.setBounds(m_gainSlider.getRight() + 20, sliderY, sliderSize,
                          sliderSize);
  const int labelY = m_gainSlider.getBottom();
  const auto effectHeight = (labelY + spacer * 2) - sliderY;

  m_gainLabel.setBounds(50, labelY, sliderSize, spacer);
  m_speedLabel.setBounds(m_speedSlider.getX(), labelY, sliderSize, spacer);

  // FX

  m_effectRack.setSize(2000, effectHeight);
  const int effectStart = m_speedLabel.getRight() + 40;
  m_effectViewport.setBounds(effectStart, m_thumbnailBounds.getBottom(),
                             m_thumbnailBounds.getRight() - effectStart,
                             effectHeight);

  m_startTime.setBounds(m_thumbnailBounds.getX(),
                        m_gainSlider.getBottom() + spacer * 2,
                        m_thumbnailBounds.getWidth(), 20);
  m_endTime.setBounds(m_thumbnailBounds.getX(), m_startTime.getBottom() + 5,
                      m_thumbnailBounds.getWidth(), 20);
  m_playButton.setBounds(m_thumbnailBounds.getRight() - buttonSize * 2 -
                             margin * 2,
                         buttonY, buttonSize, buttonSize);
  m_stopButton.setBounds(m_thumbnailBounds.getRight() - buttonSize - margin,
                         buttonY, buttonSize, buttonSize);
  m_loadButton.setBounds(m_thumbnailBounds);
  // DBG("RESIZED");

  m_loopMasterButton.setBounds(m_thumbnailBounds.getRight() + margin,
                               getHeight() / 4, buttonSize / 2, buttonSize / 2);
}

void TrackComponent::filesDropped(const StringArray &files, int, int) {
  if (isInterestedInFileDrag(files)) {
    loadFileThumbnail(*files.begin());
    const std::string path = files.begin()->toStdString();
    const rust::Str rustPath = path;
    m_engine->load_audio(rustPath);
  }
  auto &random = juce::Random::getSystemRandom();
  const juce::Colour color(random.nextInt(256), random.nextInt(256),
                           random.nextInt(256));
  m_color = color;
  m_effectRack.setColor(m_color);

  layoutSliders();
  layoutButtons();
  loadTheme();
  repaint();
  resized();
}

bool TrackComponent::isInterestedInFileDrag(const StringArray &files) {
  if (files.begin()->contains(".wav")) {
    return true;
  } else
    return false;
}

void TrackComponent::loadFileThumbnail(const String &fileName) {
  const auto file = juce::File(fileName);
  if (auto *reader = m_formatManager.createReaderFor(file); reader != nullptr) {
    auto newSource =
        std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    // transportSource.setSource (newSource.get(), 0, nullptr,
    // reader->sampleRate);
    m_thumbnail.setSource(new juce::FileInputSource(file));
    // readerSource.reset (newSource.release());
  }
}

void TrackComponent::mouseEnter(const MouseEvent &) {}

void TrackComponent::play() {
  if (m_playMode == Regular) {
    m_engine->play();
  } else if (m_playMode == Granular) {
    m_engine->grain_play();
  }
}

void TrackComponent::stop() { m_engine->stop(); }

void TrackComponent::setLength(const double length) {
  const auto trackLength = m_engine->get_length();
  const auto end =
      m_startTime.getValue() +
      static_cast<double>(length) / static_cast<double>(trackLength);
  m_endTime.setValue(end);
}
