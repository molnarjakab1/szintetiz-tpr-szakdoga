
#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"
#include "Data/AdsrData.h"
#include "PresetManager.h"
#include <juce_dsp/juce_dsp.h>
#include <array>
#include "FxSlot.h"

class TapSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    TapSynthAudioProcessor();
    ~TapSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void setFxOrder(const std::array<int, 4>& newOrder);

    bool savePreset(const juce::String& name) { return presets.save(name); }
    bool loadPresetByName(const juce::String& name) { return presets.loadByName(name); }
    juce::Array<PresetManager::PresetInfo> listPresets() { return presets.list(); }

    juce::File getPresetFolder() const { return presets.dir(); }



    juce::AudioProcessorValueTreeState apvts;

private:
    juce::Synthesiser synth;    
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();

    PresetManager presets{ apvts, "TapSynth" };

    juce::dsp::ProcessorChain<
        juce::dsp::Chorus<float>,
        juce::dsp::DelayLine<float>,
        juce::dsp::Reverb
    > fxChain;

    std::array<int, 4> fxOrder{ 0, 1, 2, 3 };


    enum FxIndex { fxChorus, fxDelay, fxReverb };
    std::array<FxSlot, 10> fxSlots;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapSynthAudioProcessor)
};
