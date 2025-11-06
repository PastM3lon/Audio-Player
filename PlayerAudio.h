#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio();
    void mixTracks();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void runMix(const juce::File& folder1, const juce::File& folder2);
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
    double getCurrentPosition();
    void setABLooping(bool shouldLoop, double start, double end);
    juce::String getTitle() const { return title; }
    juce::String getArtist() const { return artist; }
    juce::String getAlbum() const { return album; }
    juce::String getGenre() const { return genre; }
    juce::String title, artist, album, genre;
    double getDuration() const { return duration; }
    double duration = 0.0;
    int year = 0, track = 0;

private:
    juce::Label metaLabel;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    bool abLooping = false;
    double loopStart = 0.0;
    double loopEnd = 0.0;
};
