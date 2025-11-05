#include "MainComponent.h"

MainComponent::MainComponent()
    : playerGUI1(playerAudio1), playerGUI2(playerAudio2)
{
    setSize(1000, 600);
    setAudioChannels(0, 2);

    mixer.addInputSource(&playerAudio1, false);
    mixer.addInputSource(&playerAudio2, false);

    addAndMakeVisible(playerGUI1);
    addAndMakeVisible(playerGUI2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    mixer.removeAllInputs();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerAudio1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixer.releaseResources();
    playerAudio1.releaseResources();
    playerAudio2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    auto halfWidth = area.getWidth() / 2;

    playerGUI1.setBounds(area.removeFromLeft(halfWidth));
    playerGUI2.setBounds(area);
}
