#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef) : audio(audioRef)
{
    for (auto* btn : { &loadButton, &playButton, &stopButton, &mutebutton, &pauseButton, &startButton, &endButton, &loopButton })

    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    const int buttonWidth = 100;
    const int buttonHeight = 40;
    const int buttonSpacing = 10;

    int startX = 20;
    int y = 20;


    loadButton.setBounds(startX, y, buttonWidth, buttonHeight);
    playButton.setBounds(loadButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(playButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
    stopButton.setBounds(pauseButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
    mutebutton.setBounds(stopButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
    startButton.setBounds(mutebutton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
    endButton.setBounds(startButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);
	loopButton.setBounds(endButton.getRight() + buttonSpacing, y, buttonWidth, buttonHeight);


    int sliderY = y + buttonHeight + 40;
    int sliderHeight = 30;

    volumeSlider.setBounds(20, sliderY, getWidth() - 40, sliderHeight);
    speedSlider.setBounds(20, sliderY + 50, getWidth() - 40, sliderHeight);
}


void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select Audio File", juce::File{}, "*.wav;*.mp3");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                audio.loadFile(file);
                audio.start();
            });
    }
    else if (button == &playButton)
    {
        audio.start();
    }
    else if (button == &stopButton)
    {
        audio.stop();
    }
    else if (button == &mutebutton)
    {
        if (volumeSlider.getValue() > 0.0)
        {
            volprev = (float)volumeSlider.getValue();
            volumeSlider.setValue(0.0);
            mutebutton.setButtonText("Unmute");
        }
        else
        {
            volumeSlider.setValue(volprev);
            mutebutton.setButtonText("Mute");
        }
    }
    else if (button == &pauseButton)
    {
        audio.pause();
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
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audio.setGain((float)slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        audio.setSpeed(slider->getValue());
    }
}
