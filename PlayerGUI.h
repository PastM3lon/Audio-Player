#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

// Inheritance: Class inherits from several juce classes to build its functionality.
class PlayerGUI : 
    public juce::Component, // Makes the class a visible part of the UI that can draw and resize itself
    public juce::Button::Listener, // Lets it respond to button presses
	public juce::Slider::Listener, // Lets it respond to slider movements
	public juce::Timer, // Allows it to perform actions at regular intervals
    public juce::ListBoxModel // Manages how songs appear in the playlist.
{
public:
    // Abstraction: This provides a simple way to create and use the class without needing to know the details of how the UI is managed.
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;

    // Polymorphism: These are virtual functions from base classes that PlayerGUI overrides to provide the specific function
    // The override keyword helps the class provide its own implementation for these functions.
    void paint(juce::Graphics& g) override; 
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
    void playSelectedFromPlaylist();
    void extractMetadata(const juce::File& file);

private:
    // Encapsulation: internal details (like buttons, sliders, and labels) are hidden inside the class.
    PlayerAudio& audio;

    juce::TextButton shuffleButton;
    juce::TextButton loadButton{ "Load" };
    juce::TextButton loopButton{ "Loop: OFF" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton abLoopButton{ "A -> B Loop: OFF" };
    juce::TextButton addButton{ "Add" };
    juce::TextButton removeButton{ "Remove" };
    juce::TextButton mixButton{ "Mix: OFF" };

    juce::DrawableButton pausePlay{ "Play", juce::DrawableButton::ImageFitted };
    juce::DrawableButton volButton{ "Volume", juce::DrawableButton::ImageFitted };
	juce::DrawableButton speedButton{ "Speed", juce::DrawableButton::ImageFitted };
    juce::DrawableButton startButton{ "Go to Start", juce::DrawableButton::ImageFitted };
    juce::DrawableButton endButton{ "Go to End", juce::DrawableButton::ImageFitted };
    juce::DrawableButton tenForwardButton{ "10s Forward", juce::DrawableButton::ImageFitted };
    juce::DrawableButton tenBackwardButton{ "10s Backward", juce::DrawableButton::ImageFitted };
    
    juce::Label currentTime;
    juce::Label totalTime;
    juce::Label metaLabel;

    juce::ListBox playlistBox;

    juce::Array<juce::File> playlistFiles;
    juce::Array<int> shuffledOrder;

    bool looping = false;
    bool muted = false;
    bool shuffling = false;
    bool abLooping = false;
    bool mixing = false;

    float volprev = 0.5f;
    double loopStart = 0.0;
    double loopEnd = 0.0;
    int currentTrackIndex = -1;
    
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
	std::unique_ptr<juce::Drawable> speedHalf;
	std::unique_ptr<juce::Drawable> speedOne;
	std::unique_ptr<juce::Drawable> speedOneHalf;
	std::unique_ptr<juce::Drawable> speedTwo;
};
