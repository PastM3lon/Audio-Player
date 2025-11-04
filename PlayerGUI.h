#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
    void playSelectedFromPlaylist();
private:
    PlayerAudio& audio;

    juce::Label metaLabel;
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
    juce::TextButton addButton{ "Add" };
    juce::TextButton removeButton{ "Remove" };
    juce::ListBox playlistBox;
    juce::Array<juce::File> playlistFiles;
    int currentTrackIndex = -1;
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
