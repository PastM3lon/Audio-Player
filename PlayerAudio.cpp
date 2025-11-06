#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <cstdint>


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
    if (abLooping && loopEnd > loopStart) {
    double current = transportSource.getCurrentPosition();
    if (current >= loopEnd)
        transportSource.setPosition(loopStart);
    }
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

        player.title = juce::String(juce::CharPointer_UTF8(tag->title().toCString(true)));
        player.artist = juce::String(juce::CharPointer_UTF8(tag->artist().toCString(true)));
        player.album = juce::String(juce::CharPointer_UTF8(tag->album().toCString(true)));
        player.genre = juce::String(juce::CharPointer_UTF8(tag->genre().toCString(true)));
        player.year = tag->year();
        player.track = tag->track();
    }
    else
    {
        DBG("Could not read metadata from: " << file.getFullPathName());
        player.title = {};
        player.artist = {};
        player.album = {};
        player.genre = {};
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
void PlayerAudio::mixTracks()
{
    auto chooser1 = std::make_shared<juce::FileChooser>(
        "Select first audio file", juce::File{}, "*.wav;*.mp3");

    chooser1->launchAsync(juce::FileBrowserComponent::canSelectFiles,
        [this, chooser1](const juce::FileChooser& fc1)
        {
            auto file1 = fc1.getResult();
            if (!file1.existsAsFile()) return;

            auto chooser2 = std::make_shared<juce::FileChooser>(
                "Select second audio file", juce::File{}, "*.wav;*.mp3");

            chooser2->launchAsync(juce::FileBrowserComponent::canSelectFiles,
                [this, file1, chooser2](const juce::FileChooser& fc2)
                {
                    auto file2 = fc2.getResult();
                    if (!file2.existsAsFile()) return;

                    runMix(file1, file2);
                });
        });
}


void PlayerAudio::runMix(const juce::File& file1, const juce::File& file2)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    auto reader1 = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file1));
    auto reader2 = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file2));

    if (!reader1 || !reader2)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Mix Error", "Could not read one or both of the selected files.");
        return;
    }

    using int64_t = long long;

    int64_t numSamples1 = reader1->lengthInSamples;
    int64_t numSamples2 = reader2->lengthInSamples;
    int64_t sampleRate = (int64_t)reader1->sampleRate;
    int64_t crossfadeLength = (int64_t)(10.0 * sampleRate);

    juce::AudioBuffer<float> buffer1((int)reader1->numChannels, (int)numSamples1);
    juce::AudioBuffer<float> buffer2((int)reader2->numChannels, (int)numSamples2);
    reader1->read(&buffer1, 0, (int)numSamples1, 0, true, true);
    reader2->read(&buffer2, 0, (int)numSamples2, 0, true, true);

    int64_t totalLength = numSamples1 + numSamples2 - crossfadeLength;
    juce::AudioBuffer<float> mixBuffer(2, (int)totalLength);
    mixBuffer.clear();

    for (int c = 0; c < 2; ++c)
    {
        auto* dest = mixBuffer.getWritePointer(c);
        auto* src1 = buffer1.getReadPointer(juce::jmin(c, (int)buffer1.getNumChannels() - 1));
        auto* src2 = buffer2.getReadPointer(juce::jmin(c, (int)buffer2.getNumChannels() - 1));

        for (int i = 0; i < numSamples1 - crossfadeLength; ++i)
            dest[i] = src1[i];

        for (int i = 0; i < crossfadeLength; ++i)
        {
            float fadeOut = 1.0f - (float)i / (float)crossfadeLength;
            float fadeIn = (float)i / (float)crossfadeLength;
            dest[i + numSamples1 - crossfadeLength] =
                src1[i + numSamples1 - crossfadeLength] * fadeOut +
                src2[i] * fadeIn;
        }

        for (int i = crossfadeLength; i < numSamples2; ++i)
            dest[i + numSamples1 - crossfadeLength] = src2[i];
    }

    auto outputFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
        .getChildFile("MixedOutput.wav");

    std::unique_ptr<juce::FileOutputStream> output(outputFile.createOutputStream());
    if (output)
    {
        juce::WavAudioFormat wavFormat;
        if (auto* writer = wavFormat.createWriterFor(output.get(), sampleRate, 2, 16, {}, 0))
        {
            output.release();
            std::unique_ptr<juce::AudioFormatWriter> writerPtr(writer);
            writerPtr->writeFromAudioSampleBuffer(mixBuffer, 0, mixBuffer.getNumSamples());
        }
    }

    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Mix Complete",
        "MixedOutput.wav has been saved to your Desktop!");
}

double PlayerAudio::getCurrentPosition()
{
    return transportSource.getCurrentPosition();
}

void PlayerAudio::setABLooping(bool shouldLoop, double start, double end)
{
    abLooping = shouldLoop;
    loopStart = start;
    loopEnd = end;
}






