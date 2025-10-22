#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // Add buttons
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &loopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(500, 250);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!transportSource.isPlaying() && isLooping && transportSource.getLengthInSeconds() > 0.0)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    int y = 20;
    int x = 20;
    int w = 100;
    int h = 40;
    int spacing = 10;

    loadButton.setBounds(x, y, w, h);
    restartButton.setBounds(loadButton.getRight() + spacing, y, w, h);
    stopButton.setBounds(restartButton.getRight() + spacing, y, w, h);
    loopButton.setBounds(stopButton.getRight() + spacing, y, w, h); 

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    if (auto* reader = formatManager.createReaderFor(file))
                    {
                        transportSource.stop();
                        transportSource.setSource(nullptr);
                        readerSource.reset();

                        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

                        transportSource.setSource(readerSource.get(),
                            0,
                            nullptr,
                            reader->sampleRate);

                        transportSource.start();
                    }
                }
            });
    }

    else if (button == &restartButton)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }

    else if (button == &stopButton)
    {
        transportSource.stop();
        transportSource.setPosition(0.0);
    }

    else if (button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setButtonText(isLooping ? "Loop: ON" : "Loop: OFF");
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        transportSource.setGain((float)slider->getValue());
}
