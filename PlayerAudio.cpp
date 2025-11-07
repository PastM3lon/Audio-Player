#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

// Constructor: Initializes the object's internal state
// ENCAPSULATION: Sets up the private members which are hidden
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
    resamplingSourceNext = std::make_unique<juce::ResamplingAudioSource>(&transportSourceNext, false);
}
// Destructor: Cleans up resources when the object is destroyed
PlayerAudio::~PlayerAudio() {}

// Polymorphism: Overrides the virtual prepareToPlay function from juce::AudioSource
// Sets up the audio source with the correct sample rate and block size before playback
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    transportSourceNext.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resamplingSourceNext->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Polymorphism: Overrides the virtual getNextAudioBlock function from juce::AudioSource
// Provides the actual audio data every time the system asks for it
// It also contains the custom logic for A -> B looping
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    resamplingSource->getNextAudioBlock(bufferToFill);
    if (currentGain != 1.0f)
    {
        auto* out = bufferToFill.buffer;
        const int numCh = out->getNumChannels();
        const int numSamps = bufferToFill.numSamples;
        for (int ch = 0; ch < numCh; ++ch)
            out->applyGainRamp(ch, bufferToFill.startSample, numSamps, currentGain, currentGain);
    }


    if (nextLoaded)
    {
        juce::AudioBuffer<float> nextBuf(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        nextBuf.clear();
        juce::AudioSourceChannelInfo nextInfo(&nextBuf, 0, bufferToFill.numSamples);

        resamplingSourceNext->getNextAudioBlock(nextInfo);

        auto* out = bufferToFill.buffer;
        const int numCh = out->getNumChannels();
        const int numSamps = bufferToFill.numSamples;
        for (int ch = 0; ch < numCh; ++ch)
        {
            float* dst = out->getWritePointer(ch, bufferToFill.startSample);
            const float* src = nextBuf.getReadPointer(ch);
            for (int i = 0; i < numSamps; ++i)
                dst[i] += src[i] * nextGain;
        }
    }
    if (abLooping && loopEnd > loopStart) {
        double current = transportSource.getCurrentPosition();
        if (current >= loopEnd)
            transportSource.setPosition(loopStart);
    }
}

// Polymorphism: Overrides the virtual releaseResources function from juce::AudioSource
// Releases any resources when playback stops
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resamplingSource->releaseResources();
}

// Abstraction: Interacts with the PlayerAudio object through setMetadata
// Reads metadata from audio file using TagLib
void readMetadata(PlayerAudio& player, const juce::File& file) 
{
    TagLib::FileRef ref(file.getFullPathName().toRawUTF8());
    if (!ref.isNull() && ref.tag())
    {
        TagLib::Tag* tag = ref.tag();

        juce::String title = juce::String(juce::CharPointer_UTF8(tag->title().toCString(true)));
        juce::String artist = juce::String(juce::CharPointer_UTF8(tag->artist().toCString(true)));
        juce::String album = juce::String(juce::CharPointer_UTF8(tag->album().toCString(true)));

		// Encapsulation: The metadata is updated via a public method, keeping the internal members private
        player.setMetadata(title, artist, album);
    }
    else
    {
        DBG("Could not read metadata from: " << file.getFullPathName());
        player.setMetadata({}, {}, {});
    }
}


// Abstraction: Provides a simple loadFile method
// Encapsulation: Handles internal members like transportSource, and readerSource
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
            }
            else
            {
                DBG(" File: " << file.getFileNameWithoutExtension());
            }
            DBG(" Duration: " << juce::String(duration, 2) + " sec");
        }
    }
}

// Abstraction: A simple method to start audio playback
void PlayerAudio::start()
{
    transportSource.start();
}

// Abstraction: A simple method to stop audio playback and reset the position
void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

// Encapsulation: Handles playback audio via transportSource using a public method
void PlayerAudio::setVol(float volume)
{
    transportSource.setGain(volume);
}

// Encapsulation: Handles playback speed via resamplingSource using a public method
void PlayerAudio::setSpeed(double ratio) 
{
    if (resamplingSource)
        resamplingSource->setResamplingRatio(ratio);
}

// Abstraction: A simple method to pause audio playback
void PlayerAudio::pause()
{
    transportSource.stop();
}

// Encapsulation: Handles playack position via transportSource using a public method
void PlayerAudio::setPosition(double newPosition)
{
    transportSource.setPosition(newPosition);
}

// Abstraction: A method to jump to the end of the audio track
void PlayerAudio::setPositionToEnd()
{
    auto length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
}

// Encapsulation: Enables or disables looping via a public method
void PlayerAudio::setLooping(bool shouldLoop)
{
    if (readerSource)
    {
        readerSource->setLooping(shouldLoop);
    }
    transportSource.setLooping(shouldLoop);
}

// Encapsulation: Gets the current playback position without exposing transportSource
double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

// Encapsulation: Gets the total length of the audio in seconds without exposing readerSource
double PlayerAudio::getLengthInSeconds() const
{
    if (readerSource && readerSource->getAudioFormatReader())
        return readerSource->getAudioFormatReader()->lengthInSamples /
        readerSource->getAudioFormatReader()->sampleRate;
    return 0.0;
}

// Encapsulation: Checks if audio is currently playing without exposing transportSource
bool PlayerAudio::isPlaying() const 
{
    return transportSource.isPlaying();
}

// Abstraction: Easy method to skip forward in the audio by a specified number of seconds
void PlayerAudio::skipForward(double seconds) 
{
    double newPosition = transportSource.getCurrentPosition() + 10;
    if (newPosition > getLengthInSeconds())
        newPosition = getLengthInSeconds();
    transportSource.setPosition(newPosition);
}

// Abstraction: Easy method to skip backward in the audio by a specified number of seconds
void PlayerAudio::skipBackward(double seconds) 
{
    double newPosition = transportSource.getCurrentPosition() - 10;
    if (newPosition < 0.0)
        newPosition = 0.0;
    transportSource.setPosition(newPosition);
}

// Encapsulation: Handles A -> B looping logic via internal members using a public method
void PlayerAudio::setABLooping(bool shouldLoop, double start, double end)
{
    abLooping = shouldLoop;
    loopStart = start;
    loopEnd = end;
}

// Abstraction: Enables or disables Mix Mode via a public method
// Encapsulation: Manages the internal state isMixModeOn
void PlayerAudio::setMixMode(bool shouldMix)
{
    isMixModeOn = shouldMix;

    if (isMixModeOn)
    {
        DBG("Mix Mode: ON");
    }
    else
    {
        DBG("Mix Mode: OFF");
        isCrossfading = false;
    }
}

// Abstraction: Prepares the next audio file for crossfading
// Encapsulation: Manages internal members like transportSourceNext and readerSourceNext
void PlayerAudio::startNextForCrossfade(const juce::File& file)
{

    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {

            transportSourceNext.stop();
            transportSourceNext.setSource(nullptr);

            readerSourceNext.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSourceNext.setSource(readerSourceNext.get(), 0, nullptr, reader->sampleRate);


            transportSourceNext.setGain(0.0f);
            transportSourceNext.start();
            nextLoaded = true;
            nextGain = 0.0f;

        }
    }
}

// Abstraction: Sets the progress of the crossfade between current and next audio
// Encapsulation: Manages internal gain variables without exposing them
void PlayerAudio::setCrossfadeProgress(float progress)
{
    progress = juce::jlimit(0.0f, 1.0f, progress);
    nextGain = progress;
    currentGain = 1.0f - progress;
    transportSource.setGain(currentGain);
    transportSourceNext.setGain(nextGain);

}

// Abstraction: Completes the crossfade process, switching to the next audio source
// Encapsulation: Manages internal state and resources without exposing them
void PlayerAudio::finishCrossfade()
{
    if (!nextLoaded)
        return;

    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    if (readerSourceNext)
    {
        auto* r = readerSourceNext->getAudioFormatReader();

        readerSource = std::move(readerSourceNext);

        double currentPos = transportSourceNext.getCurrentPosition();

        transportSource.setSource(readerSource.get(), 0, nullptr, r->sampleRate);
        transportSource.setPosition(currentPos);
        transportSource.setGain(1.0f);
        transportSource.start();
        transportSourceNext.stop();
        transportSourceNext.setSource(nullptr);
        nextLoaded = false;

        currentGain = 1.0f;
        nextGain = 0.0f;
        isCrossfading = false;
    }
}
