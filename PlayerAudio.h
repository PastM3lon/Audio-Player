#pragma once
#include <JuceHeader.h>

// Inheritance: PlayerAudio class inherits from juce::AudioSource
class PlayerAudio : public juce::AudioSource 
{
public:
    // Abstraction: The interface provides a simplified way to interact with the audio player.
    PlayerAudio();
    ~PlayerAudio();

	// Polymorphism: These methods are virtual functions defined in the base class juce::AudioSource.
    // The override keyword helps the class provide its own implementation for these functions.
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

	// Abstract actions the user can perform on the audio player without knowing how they are implemented
    void loadFile(const juce::File& file);
    void start();
    void stop();
    void setVol(float volume);
    void setSpeed(double ratio);
    void pause();
    void setPosition(double newPosition);
    void setPositionToEnd();
    void setLooping(bool shouldLoop);
    void skipForward(double seconds);
    void skipBackward(double seconds);
    void setABLooping(bool shouldLoop, double start, double end);
    void startNextForCrossfade(const juce::File& file);
    void setCrossfadeProgress(float progress);
    void finishCrossfade();
    void setMixMode(bool shouldMix);

    // Concrete/Normal functions (not abstract)
    double getCurrentPosition() const;
    double getLengthInSeconds() const;
    bool isPlaying() const;
    double getDuration() const { return duration; };
    bool getMixMode() const { return isMixModeOn; }
    bool getIsCrossfading() const { return isCrossfading; }
    double getCrossfadeDuration() const { return crossfadeDuration; }
    void setIsCrossfading(bool value) { isCrossfading = value; }
    
    juce::String getTitle() const { return title; }
    juce::String getArtist() const { return artist; }
    juce::String getAlbum() const { return album; }
    void setMetadata(const juce::String& newTitle, const juce::String& newArtist, const juce::String& newAlbum)
    {
        title = newTitle;
        artist = newArtist;
        album = newAlbum;
    }

private: 
    // Encapsulation: Internal members of the class
    juce::Label metaLabel;
    juce::String title, artist, album;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSourceNext;
    juce::AudioTransportSource transportSourceNext;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSourceNext;

	// Direct data needs to be encapsulated
    bool abLooping = false;
    double loopStart = 0.0;
    double loopEnd = 0.0;
    double duration = 0.0;
    bool isCrossfading = false;
    double crossfadeDuration = 15.0;
    bool isMixModeOn = false;
    bool nextLoaded = false;
    float currentGain = 1.0f;
    float nextGain = 0.0f;
};
