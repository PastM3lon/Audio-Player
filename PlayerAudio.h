#pragma once
#include <JuceHeader.h>


class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void loadFile(const juce::File& file);
    void start();
    void stop();
    void setGain(float gain);
    void setSpeed(double ratio);
    void pause();
    void setPosition(double newPosition);
    void setPositionToEnd();
    void setLooping(bool shouldLoop);
    double getCurrentPosition() const;
    double getLengthInSeconds() const;
    bool isPlaying() const;
    void skipForward(double seconds);
    void skipBackward(double seconds);

    juce::String getTitle() const { return title; }
    juce::String getArtist() const { return artist; }
    juce::String getAlbum() const { return album; }
    juce::String getGenre() const { return genre; }
    double getDuration() const { return duration; }
    juce::String title, artist, album, genre;
    double duration = 0.0;
    int year = 0, track = 0;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;

};
