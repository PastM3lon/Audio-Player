#define TAGLIB_STATIC
#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

PlayerGUI::~PlayerGUI() {}

PlayerGUI::PlayerGUI(PlayerAudio& audioRef) : audio(audioRef)
{
    for (auto* btn : { &loadButton, &stopButton, &loopButton, &addButton, &removeButton, &shuffleButton, &setAButton, &setBButton, &abLoopButton ,&mixButton })
    {
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xffa5978c));
        btn->setColour(juce::TextButton::textColourOffId, juce::Colour(0xff1c1816));
        addAndMakeVisible(btn);
    }

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xff1b1b1b));

    auto startButtonImage = juce::Drawable::createFromImageData(BinaryData::goStart_svg, BinaryData::goStart_svgSize);
    startButton.setImages(startButtonImage.get());
    startButton.addListener(this);
    addAndMakeVisible(startButton);

    auto endButtonImage = juce::Drawable::createFromImageData(BinaryData::goEnd_svg, BinaryData::goEnd_svgSize);
    endButton.setImages(endButtonImage.get());
    endButton.addListener(this);
    addAndMakeVisible(endButton);

    auto tenForwardButtonImage = juce::Drawable::createFromImageData(BinaryData::forwardTen_svg, BinaryData::forwardTen_svgSize);
    tenForwardButton.setImages(tenForwardButtonImage.get());
    tenForwardButton.addListener(this);
    addAndMakeVisible(tenForwardButton);

    auto tenBackwardButtonImage = juce::Drawable::createFromImageData(BinaryData::replayTen_svg, BinaryData::replayTen_svgSize);
    tenBackwardButton.setImages(tenBackwardButtonImage.get());
    tenBackwardButton.addListener(this);
    addAndMakeVisible(tenBackwardButton);

    songpause = juce::Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize);
    songplay = juce::Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize);
    muteVol = juce::Drawable::createFromImageData(BinaryData::mute_svg, BinaryData::mute_svgSize);
    lowVol = juce::Drawable::createFromImageData(BinaryData::volumeDown_svg, BinaryData::volumeDown_svgSize);
    highVol = juce::Drawable::createFromImageData(BinaryData::volumeUp_svg, BinaryData::volumeUp_svgSize);
    speedHalf = juce::Drawable::createFromImageData(BinaryData::speedHalf_svg, BinaryData::speedHalf_svgSize);
    speedOne = juce::Drawable::createFromImageData(BinaryData::speedOne_svg, BinaryData::speedOne_svgSize);
    speedOneHalf = juce::Drawable::createFromImageData(BinaryData::speedOneHalf_svg, BinaryData::speedOneHalf_svgSize);
    speedTwo = juce::Drawable::createFromImageData(BinaryData::speedTwo_svg, BinaryData::speedTwo_svgSize);

    pausePlay.setImages(songplay.get());
    pausePlay.addListener(this);
    addAndMakeVisible(pausePlay);

    volButton.setImages(highVol.get());
    volButton.addListener(this);
    addAndMakeVisible(volButton);

    speedButton.setImages(speedOne.get());
    speedButton.addListener(this);
    addAndMakeVisible(speedButton);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    progressbar.setRange(0.0, 1.0, 0.001);
    progressbar.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressbar.addListener(this);
    addAndMakeVisible(progressbar);

    currentTime.setText("00:00", juce::dontSendNotification);
    currentTime.setJustificationType(juce::Justification::centredLeft);
    currentTime.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(currentTime);

    totalTime.setText("00:00", juce::dontSendNotification);
    totalTime.setJustificationType(juce::Justification::centredRight);
    totalTime.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(totalTime);

    addAndMakeVisible(metaLabel);
    metaLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metaLabel.setJustificationType(juce::Justification::centred);
    metaLabel.setMinimumHorizontalScale(1.0f);
    metaLabel.setInterceptsMouseClicks(false, false);
    metaLabel.setJustificationType(juce::Justification::centred);
    metaLabel.setSize(getWidth() - 80, 80);
    metaLabel.setText("No track loaded", juce::dontSendNotification);

    shuffleButton.setButtonText("Shuffle: OFF");

    mixButton.setButtonText("Mix: OFF");

}

void PlayerGUI::paint(juce::Graphics& g)
{
    juce::Colour top = juce::Colour(0xffd9d4cf);
    juce::Colour bottom = juce::Colour(0xffbfbab5);
    g.setGradientFill(juce::ColourGradient(top, 0, 0, bottom, 0, (float)getHeight(), false));
    g.fillAll();

    auto displayArea = juce::Rectangle<float>(40, 30, getWidth() - 80, 150);
    g.setColour(juce::Colour(0xff1b1b1b));
    g.fillRoundedRectangle(displayArea, 12.0f);
    g.setColour(juce::Colour(0xfff2662f).withAlpha(0.8f));
    g.fillRoundedRectangle(displayArea.getX(), displayArea.getBottom() + 8, displayArea.getWidth(), 6, 3);

    int playlistY = 300;
    int playlistHeight = getHeight() - playlistY - 40;
    auto playlistArea = juce::Rectangle<float>(40, playlistY, getWidth() - 80, (float)playlistHeight);

    g.setColour(juce::Colour(0xff1b1b1b));
    g.fillRoundedRectangle(playlistArea, 12.0f);
    g.setColour(juce::Colour(0xfff2662f).withAlpha(0.3f));
    g.drawRoundedRectangle(playlistArea, 12.0f, 1.5f);
}

void PlayerGUI::resized()
{
    int buttonWidth = 83;
    int buttonHeight = 30;
    int spacing = 1;
    int y = 190;

    loadButton.setBounds(40, y + 5, buttonWidth, buttonHeight * 2);
    stopButton.setBounds(loadButton.getRight() + spacing, y + 5, buttonWidth, buttonHeight * 2);
    loopButton.setBounds(stopButton.getRight() + spacing, y + 5, buttonWidth, buttonHeight * 2);
    setAButton.setBounds(loopButton.getRight() + spacing, y + 5, buttonWidth, buttonHeight);
    setBButton.setBounds(setAButton.getRight() + spacing, y + 5, buttonWidth, buttonHeight);
    abLoopButton.setBounds(loopButton.getRight() + spacing, y + 5 + buttonHeight, buttonWidth * 2, buttonHeight);

    progressbar.setBounds(90, 150, 3 * getWidth() / 4 - 120, 30);
    tenBackwardButton.setBounds(110, (y / 2) + 25, buttonWidth / 2, buttonHeight);
    startButton.setBounds(tenBackwardButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    pausePlay.setBounds(startButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    endButton.setBounds(pausePlay.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    tenForwardButton.setBounds(endButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    currentTime.setBounds(50, progressbar.getY(), 45, progressbar.getHeight());
    totalTime.setBounds(progressbar.getRight(), progressbar.getY(), 45, progressbar.getHeight());

    volButton.setBounds(totalTime.getRight() + 25, 150, buttonWidth / 2, 25);
    volumeSlider.setBounds(volButton.getX() + 7, 30, 30, 120);

    speedSlider.setBounds(volumeSlider.getX() - 25, volumeSlider.getY(), 30, 120);
    speedButton.setBounds(speedSlider.getX(), volButton.getY(), buttonWidth / 3, 25);

    int playlistY = 300;
    int playlistHeight = getHeight() - playlistY - 40;

    playlistBox.setBounds(60, playlistY + 20, getWidth() - 120, playlistHeight - 80);
    addButton.setBounds(60, playlistY + playlistHeight - 50, 80, 30);
    removeButton.setBounds(addButton.getRight() + 10, playlistY + playlistHeight - 50, 80, 30);
    shuffleButton.setBounds(removeButton.getRight() + 10, playlistY + playlistHeight - 50, 120, 30);

    mixButton.setBounds(shuffleButton.getRight() + 10, playlistY + playlistHeight - 50, 100, 30);

    metaLabel.setBounds(30, 20, getWidth() - 80, 100);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour(0xfff2662f));
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(0xff3a3a3a));
}
int PlayerGUI::getNumRows()
{
    return playlistFiles.size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);

    g.setColour(juce::Colours::white);

    if (rowNumber >= 0 && rowNumber < playlistFiles.size())
        g.drawText(playlistFiles[rowNumber].getFileName(),
            5, 0, width - 10, height,
            juce::Justification::centredLeft);
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < playlistFiles.size())
    {
        currentTrackIndex = row;
        playSelectedFromPlaylist();
    }
}

void PlayerGUI::playSelectedFromPlaylist()
{
    if (currentTrackIndex >= 0 && currentTrackIndex < playlistFiles.size())
    {
        auto& file = playlistFiles[currentTrackIndex];
        extractMetadata(file);
        audio.loadFile(file);
        if (looping)
            audio.setLooping(true);
        audio.start();
        pausePlay.setImages(songpause.get());
        startTimerHz(30);
    }
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &addButton)
    {
        fileChooser = std::unique_ptr<juce::FileChooser>(
            new juce::FileChooser("Select Audio Files", juce::File{}, "*.mp3;*.wav"));


        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                for (auto& f : files)
                    playlistFiles.add(f);
                playlistBox.updateContent();
                if (shuffling)
                {
                    shuffledOrder.clear();
                    for (int i = 0; i < playlistFiles.size(); ++i)
                        shuffledOrder.add(i);
                    juce::Random rng;
                    for (int i = shuffledOrder.size() - 1; i > 0; --i)
                        shuffledOrder.swap(i, rng.nextInt(i + 1));

                }

            });
    }
    else if (button == &removeButton)
    {
        if (currentTrackIndex >= 0 && currentTrackIndex < playlistFiles.size())
        {
            playlistFiles.remove(currentTrackIndex);
            playlistBox.updateContent();

            if (!playlistFiles.isEmpty())
                currentTrackIndex = juce::jlimit(0, playlistFiles.size() - 1, currentTrackIndex);
            else
                currentTrackIndex = -1;
        }
    }
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select Audio File", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                audio.loadFile(file);
                extractMetadata(file);
                if (looping)
                    audio.setLooping(true);
            });
    }
    else if (button == &pausePlay)
    {
        if (audio.isPlaying())
        {
            audio.pause();
            pausePlay.setImages(songplay.get());
            stopTimer();
        }
        else
        {
            audio.start();
            pausePlay.setImages(songpause.get());
            startTimerHz(30);
        }
    }
    else if (button == &stopButton)
    {
        audio.stop();
        stopTimer();
        progressbar.setValue(0.0);
    }
    else if (button == &volButton)
    {
        if (volumeSlider.getValue() > 0.0f)
        {
            volprev = (float)volumeSlider.getValue();
            volumeSlider.setValue(0.0);
            volButton.setImages(muteVol.get());
        }
        else
        {
            volumeSlider.setValue(volprev > 0.0f ? volprev : 0.5f);
            float vol = (float)volumeSlider.getValue();

            if (vol <= 0.5f)
                volButton.setImages(lowVol.get());
            else
                volButton.setImages(highVol.get());
        }
    }
    else if (button == &startButton)
    {
        audio.setPosition(0.0);
    }
    else if (button == &endButton)
    {
        audio.setPositionToEnd();
        pausePlay.setImages(songplay.get());
    }
    else if (button == &loopButton)
    {
        looping = !looping;
        audio.setLooping(looping);
        loopButton.setButtonText(looping ? "Loop: ON" : "Loop: OFF");
    }
    else if (button == &setAButton)
    {
        loopStart = audio.getCurrentPosition();
    }

    else if (button == &setBButton)
    {
        loopEnd = audio.getCurrentPosition();
    }

    else if (button == &abLoopButton)
    {
        abLooping = !abLooping;
        abLoopButton.setButtonText(abLooping ? "A -> B Loop: ON" : "A -> B Loop: OFF");
        audio.setABLooping(abLooping, loopStart, loopEnd);
    }
    else if (button == &tenForwardButton)
    {
        audio.skipForward(10.0);
    }
    else if (button == &tenBackwardButton)
    {
        audio.skipBackward(10.0);
    }
    else if (button == &shuffleButton)
    {
        shuffling = !shuffling;
        shuffleButton.setButtonText(shuffling ? "Shuffle: ON" : "Shuffle: OFF");

        if (shuffling)
        {
            shuffledOrder.clear();
            for (int i = 0; i < playlistFiles.size(); ++i)
                shuffledOrder.add(i);
            juce::Random rng;
            for (int i = shuffledOrder.size() - 1; i > 0; --i)
                shuffledOrder.swap(i, rng.nextInt(i + 1));

            currentTrackIndex = shuffledOrder[0];
        }
        else
        {
            shuffledOrder.clear();
        }
    }
    else if (button == &mixButton)
    {
        mixing = !mixing; 
        mixButton.setButtonText(mixing ? "Mix: ON" : "Mix: OFF");
        audio.setMixMode(mixing);
        }

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        float vol = (float)slider->getValue();
        audio.setGain(vol);

        if (vol == 0.0f)
            volButton.setImages(muteVol.get());
        else if (vol <= 0.5f)
            volButton.setImages(lowVol.get());
        else
            volButton.setImages(highVol.get());
    }
    else if (slider == &speedSlider)
    {
        audio.setSpeed(slider->getValue());
        double speed = slider->getValue();

        if (speed < 0.75)
            speedButton.setImages(speedHalf.get());
        else if (speed < 1.25)
            speedButton.setImages(speedOne.get());
        else if (speed < 1.75)
            speedButton.setImages(speedOneHalf.get());
        else
            speedButton.setImages(speedTwo.get());
    }
    else if (slider == &progressbar)
    {
        double length = audio.getLengthInSeconds();
        double newPos = progressbar.getValue() * length;
        audio.setPosition(newPos);
    }
}

juce::String formatTime(double seconds)
{
    int totalSeconds = (int)std::round(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", minutes, secs);
}

void PlayerGUI::timerCallback()
{
    if (audio.getMixMode() && audio.getLengthInSeconds() > 0.0)
    {
        double position = audio.getCurrentPosition();
        double length = audio.getLengthInSeconds();
        double timeLeft = length - position;

        const double fadeDur = audio.crossfadeDuration;

        if (!audio.isCrossfading && timeLeft <= fadeDur && currentTrackIndex + 1 < playlistFiles.size())
        {
           
            int nextIndex = currentTrackIndex + 1;
            DBG("Starting pre-load for crossfade into track " << nextIndex);
            audio.isCrossfading = true;

  
            audio.startNextForCrossfade(playlistFiles[nextIndex]);
        }

        if (audio.isCrossfading)
        {
            float progress = (float)((fadeDur - timeLeft) / fadeDur);
            progress = juce::jlimit(0.0f, 1.0f, progress);
            audio.setCrossfadeProgress(progress);

            if (progress >= 1.0f)
            {
               
                audio.finishCrossfade();

           
                currentTrackIndex++;
                auto& file = playlistFiles[currentTrackIndex];
                extractMetadata(file);

                DBG("Crossfade finished. Now playing track " << currentTrackIndex);
            }
        }

    
    }

    double position = audio.getCurrentPosition();
    double length = audio.getLengthInSeconds();

    if (length > 0.0 && !progressbar.isMouseButtonDown())
    {
        progressbar.setValue(position / length, juce::dontSendNotification);
    }

    currentTime.setText(formatTime(position), juce::dontSendNotification);
    totalTime.setText(formatTime(length), juce::dontSendNotification);

    if (position >= length)
    {
        if (looping)
        {
            audio.setPosition(0.0);
            audio.start();
            progressbar.setValue(0.0);
            currentTime.setText("00:00", juce::dontSendNotification);
        }
        else
        {
            if (shuffling && shuffledOrder.size() > 0)
            {
                int currentPos = shuffledOrder.indexOf(currentTrackIndex);
                if (currentPos + 1 < shuffledOrder.size())
                {
                    currentTrackIndex = shuffledOrder[currentPos + 1];
                    playSelectedFromPlaylist();
                }
                else
                {
                    stopTimer();
                    pausePlay.setImages(songplay.get());
                }
            }
            else
            {
                if (currentTrackIndex + 1 < playlistFiles.size())
                {
                    currentTrackIndex++;
                    playSelectedFromPlaylist();
                }
                else
                {
                    stopTimer();
                    pausePlay.setImages(songplay.get());
                }
            }
        }
    }
}

void PlayerGUI::extractMetadata(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        metaLabel.setText(file.getFileName(), juce::dontSendNotification);
        return;
    }

    try
    {
        TagLib::FileRef f(file.getFullPathName().toRawUTF8());
        if (!f.isNull() && f.tag())
        {
            auto* tag = f.tag();

            juce::String title = juce::String(juce::CharPointer_UTF8(tag->title().toCString(true)));
            juce::String artist = juce::String(juce::CharPointer_UTF8(tag->artist().toCString(true)));
            juce::String album = juce::String(juce::CharPointer_UTF8(tag->album().toCString(true)));

            if (title.isEmpty())
                title = file.getFileNameWithoutExtension();
            if (artist.isEmpty())
                artist = "Unknown Artist";
            if (album.isEmpty())
                album = "Unknown Album";

            int duration = f.audioProperties() ? f.audioProperties()->length() : 0;

            metaLabel.setText(
                "Title: " + title + "\n"
                "Artist: " + artist + "\n"
                "Album: " + album + "\n"
                "Duration: " + juce::String(duration) + "s",
                juce::dontSendNotification);
        }
        else
        {
            metaLabel.setText(file.getFileName(), juce::dontSendNotification);
        }
    }
    catch (...)
    {
        metaLabel.setText("Unknown track", juce::dontSendNotification);
    }
}
