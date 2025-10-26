#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
}

PlayerAudio::~PlayerAudio() {}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingSource->getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resamplingSource->releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        }
    }
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setSpeed(double ratio)
{
    if (resamplingSource)
        resamplingSource->setResamplingRatio(ratio);
}
void PlayerAudio::pause()
{
    transportSource.stop();
}

void PlayerAudio::setPosition(double newPosition)
{
    transportSource.setPosition(newPosition);
}

void PlayerAudio::setPositionToEnd()
{
    auto length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    if (readerSource)
    {
        readerSource->setLooping(shouldLoop);
    }
    transportSource.setLooping(shouldLoop);
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLengthInSeconds() const
{
    if (readerSource && readerSource->getAudioFormatReader())
        return readerSource->getAudioFormatReader()->lengthInSamples /
        readerSource->getAudioFormatReader()->sampleRate;
    return 0.0;
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}

void PlayerAudio::skipForward(double seconds)
{
    double newPosition = transportSource.getCurrentPosition() + 10;
    if (newPosition > getLengthInSeconds())
        newPosition = getLengthInSeconds();
    transportSource.setPosition(newPosition);
}

void PlayerAudio::skipBackward(double seconds)
{
    double newPosition = transportSource.getCurrentPosition() - 10;
    if (newPosition < 0.0)
        newPosition = 0.0;
    transportSource.setPosition(newPosition);
}   
