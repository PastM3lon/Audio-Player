#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerAudio& audio;

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton mutebutton{ "Mute" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton startButton{ "Go to Start" };
    juce::TextButton endButton{ "Go to End" };

    bool muted = false;
    float volprev = 0.5f;
    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    std::unique_ptr<juce::FileChooser> fileChooser;
};
