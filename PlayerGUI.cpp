#include "PlayerGUI.h"

PlayerGUI::~PlayerGUI() {}

PlayerGUI::PlayerGUI(PlayerAudio& audioRef) : audio(audioRef)
{
    for (auto* btn : { &loadButton, &stopButton, &loopButton })
    {
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xffa5978c));
		btn->setColour(juce::TextButton::textColourOffId, juce::Colour(0xff1c1816));
        addAndMakeVisible(btn);
    }

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

	pausePlay.setImages(songplay.get());
    pausePlay.addListener(this);
    addAndMakeVisible(pausePlay);

    volButton.setImages(highVol.get());
    volButton.addListener(this);
    addAndMakeVisible(volButton);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	speedSlider.setSliderStyle(juce::Slider::Rotary);
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
}

void PlayerGUI::resized()
{
    int buttonWidth =90;
    int buttonHeight = 30;
    int spacing = 1;
    int y = 190;

    loadButton.setBounds(40, y+5, buttonWidth, buttonHeight*2);
    stopButton.setBounds(loadButton.getRight() + spacing, y+5, buttonWidth, buttonHeight*2);
    loopButton.setBounds(stopButton.getRight() + spacing, y+5, buttonWidth, buttonHeight*2);
    
    progressbar.setBounds(90, 150, 3*getWidth()/4 - 120, 30);
	tenBackwardButton.setBounds(280, (y / 2) + 25, buttonWidth / 2, buttonHeight);
    startButton.setBounds(tenBackwardButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    pausePlay.setBounds(startButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    endButton.setBounds(pausePlay.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
	tenForwardButton.setBounds(endButton.getRight(), (y / 2) + 25, buttonWidth / 2, buttonHeight);
    currentTime.setBounds(50, progressbar.getY(), 45, progressbar.getHeight());
    totalTime.setBounds(progressbar.getRight(), progressbar.getY(), 45, progressbar.getHeight());

	volButton.setBounds(totalTime.getRight(), 150, buttonWidth / 2, 25);
    volumeSlider.setBounds(volButton.getRight() - 15, 150, getWidth() / 6, 30);

    speedSlider.setBounds(60, y + 150, 100, 100);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour(0xfff2662f));
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(0xff3a3a3a));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select Audio File", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                audio.loadFile(file);
                if (looping)
                    audio.setLooping(true);
                audio.start();
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
    }
    else if (button == &loopButton)
    {
        looping = !looping;
        audio.setLooping(looping);
        loopButton.setButtonText(looping ? "Loop: ON" : "Loop: OFF");
    }
    else if (button == &tenForwardButton)
    {
        audio.skipForward(10.0);
    }
    else if (button == &tenBackwardButton)
    {
        audio.skipBackward(10.0);
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
    double position = audio.getCurrentPosition();
    double length = audio.getLengthInSeconds();

    if (length > 0.0 && !progressbar.isMouseButtonDown())
    {
        progressbar.setValue(position / length, juce::dontSendNotification);
    }

    if (position >= length) {
        stopTimer();
        pausePlay.setImages(songplay.get());
    }

    currentTime.setText(formatTime(position), juce::dontSendNotification);
    totalTime.setText(formatTime(length), juce::dontSendNotification);
}

