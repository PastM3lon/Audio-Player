#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void loadFile(const juce::File& file);
    void start();
    void stop();
    void setGain(float gain);
    void setSpeed(double ratio);
    void startNextForCrossfade(const juce::File& file);   
    void setCrossfadeProgress(float progress);           
    void finishCrossfade();                              
    void pause();
    void setMixMode(bool shouldMix);
    bool getMixMode() const { return isMixModeOn; }
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
    juce::String title, artist, album;
    bool isMixModeOn = false;     
    bool isCrossfading = false;    
    double crossfadeDuration = 15.0;
    double getDuration() const { return duration; }
    double duration = 0.0;
    int year = 0, track = 0;
    bool nextLoaded = false;
    float currentGain = 1.0f;
    float nextGain = 0.0f;
    double getCurrentPosition();
    void setABLooping(bool shouldLoop, double start, double end);


private:
    std::unique_ptr<juce::AudioFormatReaderSource> readerSourceNext;
    juce::AudioTransportSource transportSourceNext;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSourceNext;
    juce::Label metaLabel;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    bool abLooping = false;
    double loopStart = 0.0;
    double loopEnd = 0.0;
};
