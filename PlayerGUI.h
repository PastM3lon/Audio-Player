#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,  
	public juce::Timer
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;  

private:
    PlayerAudio& audio;

    juce::TextButton loadButton{ "Load" }; 
    juce::TextButton loopButton{ "Loop: OFF" };
    juce::TextButton stopButton{ "Stop" };
	juce::DrawableButton pausePlay{ "Play", juce::DrawableButton::ImageFitted };   
	juce::DrawableButton volButton{ "Volume", juce::DrawableButton::ImageFitted };
	juce::DrawableButton startButton{ "Go to Start", juce::DrawableButton::ImageFitted };   
	juce::DrawableButton endButton{ "Go to End", juce::DrawableButton::ImageFitted };
	juce::DrawableButton speedButton{ "Speed", juce::DrawableButton::ImageFitted };
	juce::DrawableButton tenForwardButton{ "10s Forward", juce::DrawableButton::ImageFitted };
	juce::DrawableButton tenBackwardButton{ "10s Backward", juce::DrawableButton::ImageFitted };
    juce::Label currentTime;
    juce::Label totalTime;
    
    bool looping = false;
    bool muted = false;
    float volprev = 0.5f;
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider progressbar;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::Drawable> muteVol;
    std::unique_ptr<juce::Drawable> lowVol;
    std::unique_ptr<juce::Drawable> highVol;
    std::unique_ptr<juce::Drawable> songplay;
    std::unique_ptr<juce::Drawable> songpause;
	std::unique_ptr<juce::Drawable> forwardTen;
	std::unique_ptr<juce::Drawable> backwardTen;
};
