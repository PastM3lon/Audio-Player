#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>



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
void readMetadata(PlayerAudio& player, const juce::File& file)
{
    TagLib::FileRef ref(file.getFullPathName().toRawUTF8());
    if (!ref.isNull() && ref.tag())
    {
        TagLib::Tag* tag = ref.tag();
        player.title  = tag->title().toCString(true);
        player.artist = tag->artist().toCString(true);
        player.album  = tag->album().toCString(true);
        player.genre  = tag->genre().toCString(true);
        player.year   = tag->year();
        player.track  = tag->track();
    }
    else
    {
        DBG(" Could not read metadata from: " << file.getFullPathName());
        player.title = "";
        player.artist = "";
        player.album = "";
        player.genre = "";
        player.year = 0;
        player.track = 0;
    }
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
            readMetadata(*this, file);
            if (reader != nullptr)
                duration = reader->lengthInSamples / reader->sampleRate;
            if (title.isNotEmpty())
            {
                DBG("Title: " << title);
                DBG("Artist: " << artist);
                DBG(" Album: " << album);
                DBG("Genre: " << genre);
                DBG("Year: " << year);
                DBG(" Track: " << track);
            }
            else
            {
                DBG(" File: " << file.getFileNameWithoutExtension());
            }
            DBG(" Duration: " << juce::String(duration, 2) + " sec");

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
