#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef) : audio(audioRef)
{
    for (auto* btn : { &loadButton, &playButton, &stopButton, &mutebutton })
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
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    playButton.setBounds(140, y, 60, 40);
    stopButton.setBounds(220, y, 60, 40);
    mutebutton.setBounds(300, y, 60, 40);
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
	speedSlider.setBounds(20, 150, getWidth() - 40, 30);
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
