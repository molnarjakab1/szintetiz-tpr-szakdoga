
#include "PluginProcessor.h"
#include "PluginEditor.h"

TapSynthAudioProcessor::TapSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound (new SynthSound());
    synth.addVoice (new SynthVoice());
}

TapSynthAudioProcessor::~TapSynthAudioProcessor()
{
    
}

const juce::String TapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapSynthAudioProcessor::getNumPrograms()
{
    return 1;   
}

int TapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


void TapSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
   
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
    for (auto& s : fxSlots)
        s.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

   
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    fxChain.prepare(spec);
    fxChain.reset();
  
    fxChain.get<fxDelay>().setMaximumDelayInSamples((size_t)(sampleRate * 2.0));
}


void TapSynthAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//void TapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
//    auto totalNumOutputChannels = getTotalNumOutputChannels();
//
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        buffer.clear (i, 0, buffer.getNumSamples());
//    
//    {
//        for (int i = 0; i < synth.getNumVoices(); ++i)
//        {
//            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
//            {
//                
//                auto& oscWaveChoice = *apvts.getRawParameterValue ("OSC1WAVETYPE");
//                
//               
//                auto& fmFreq = *apvts.getRawParameterValue ("OSC1FMFREQ");
//                auto& fmDepth = *apvts.getRawParameterValue ("OSC1FMDEPTH");
//                
//               
//                auto& attack = *apvts.getRawParameterValue ("ATTACK");
//                auto& decay = *apvts.getRawParameterValue ("DECAY");
//                auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
//                auto& release = *apvts.getRawParameterValue ("RELEASE");
//                
//              
//                auto& fAttack = *apvts.getRawParameterValue ("FILTERATTACK");
//                auto& fDecay = *apvts.getRawParameterValue ("FILTERDECAY");
//                auto& fSustain = *apvts.getRawParameterValue ("FILTERSUSTAIN");
//                auto& fRelease = *apvts.getRawParameterValue ("FILTERRELEASE");
//                
//                
//                auto& filterType = *apvts.getRawParameterValue ("FILTERTYPE");
//                auto& cutoff = *apvts.getRawParameterValue ("FILTERFREQ");
//                auto& resonance = *apvts.getRawParameterValue ("FILTERRES");
//                
//                
//                voice->getOscillator().setWaveType (oscWaveChoice);
//                voice->getOscillator().updateFm (fmFreq, fmDepth);
//                voice->getAdsr().update (attack.load(), decay.load(), sustain.load(), release.load());
//                voice->getFilterAdsr().update (fAttack.load(), fDecay.load(), fSustain.load(), fRelease.load());
//                voice->updateFilter (filterType, cutoff, resonance);
//            }
//        }
//    }
//    
//    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
//}
void TapSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

   
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto& oscWave = *apvts.getRawParameterValue("OSC1WAVETYPE");
            auto& fmFreq = *apvts.getRawParameterValue("OSC1FMFREQ");
            auto& fmDepth = *apvts.getRawParameterValue("OSC1FMDEPTH");

            auto& attack = *apvts.getRawParameterValue("ATTACK");
            auto& decay = *apvts.getRawParameterValue("DECAY");
            auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
            auto& release = *apvts.getRawParameterValue("RELEASE");

            auto& fAttack = *apvts.getRawParameterValue("FILTERATTACK");
            auto& fDecay = *apvts.getRawParameterValue("FILTERDECAY");
            auto& fSustain = *apvts.getRawParameterValue("FILTERSUSTAIN");
            auto& fRelease = *apvts.getRawParameterValue("FILTERRELEASE");

            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERFREQ");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");

            voice->getOscillator().setWaveType(oscWave);
            voice->getOscillator().updateFm(fmFreq, fmDepth);
            voice->getAdsr().update(attack.load(), decay.load(), sustain.load(), release.load());
            voice->getFilterAdsr().update(fAttack.load(), fDecay.load(), fSustain.load(), fRelease.load());
            voice->updateFilter(filterType, cutoff, resonance);
        }
    }

  
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
 
    for (int i = 1; i <= 10; ++i)
    {
        auto idx = i - 1;

        const int type = (int)apvts.getRawParameterValue("FX" + juce::String(i) + "_TYPE")->load();

        const float distDrive = apvts.getRawParameterValue("FX" + juce::String(i) + "_DISTDRIVE")->load();

        const float cDepth = apvts.getRawParameterValue("FX" + juce::String(i) + "_CHORUSDEPTH")->load();
        const float cRate = apvts.getRawParameterValue("FX" + juce::String(i) + "_CHORUSRATE")->load();

        const float dTime = apvts.getRawParameterValue("FX" + juce::String(i) + "_DELAYTIME")->load();
        const float dFb = apvts.getRawParameterValue("FX" + juce::String(i) + "_DELAYFB")->load();
        const float dMix = apvts.getRawParameterValue("FX" + juce::String(i) + "_DELAYMIX")->load();

        const float rRoom = apvts.getRawParameterValue("FX" + juce::String(i) + "_REVROOM")->load();
        const float rMix = apvts.getRawParameterValue("FX" + juce::String(i) + "_REVMIX")->load();

        fxSlots[idx].process(buffer, type,
            distDrive,
            cDepth, cRate,
            dTime, dFb, dMix,
            rRoom, rMix);
    }

 
    auto& chorus = fxChain.get<fxChorus>();
    auto& delay = fxChain.get<fxDelay>();
    auto& reverb = fxChain.get<fxReverb>();

    chorus.setDepth(*apvts.getRawParameterValue("CHORUSDEPTH"));
    chorus.setRate(*apvts.getRawParameterValue("CHORUSRATE"));

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = *apvts.getRawParameterValue("REVERBROOM");
    rp.wetLevel = *apvts.getRawParameterValue("REVERBMIX");
    reverb.setParameters(rp);

    juce::dsp::AudioBlock<float> block(buffer);
    auto ctx = juce::dsp::ProcessContextReplacing<float>(block);

    auto applyDistortion = [this, &buffer]()
    {
        const float driveDb = *apvts.getRawParameterValue("DISTDRIVE");
        const float drive = juce::Decibels::decibelsToGain(driveDb);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            float* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float x = data[i];
                data[i] = std::tanh(x * drive);
            }
        }
    };


    for (int fx : fxOrder)
    {
        switch (fx)
        {
        case 0: 
            applyDistortion();
            break;
        case 1: 
            chorus.process(ctx);
            break;
        case 2: 
            delay.process(ctx);
            break;
        case 3: 
            reverb.process(ctx);
            break;
        }
    }

}


bool TapSynthAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* TapSynthAudioProcessor::createEditor()
{
    return new TapSynthAudioProcessorEditor (*this);
}


void TapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto tree = apvts.copyState();
    if (auto xml = tree.createXml())
        copyXmlToBinary(*xml, destData);
}

void TapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes)) 
    {
        juce::ValueTree tree = juce::ValueTree::fromXml(*xml);
        if (tree.isValid())
            apvts.replaceState(tree);
    }
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout TapSynthAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    
    params.push_back (std::make_unique<juce::AudioParameterChoice>("OSC1WAVETYPE", "Osc 1 Wave Type", juce::StringArray { "Sine", "Saw", "Square" }, 0));
    
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMFREQ", "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMDEPTH", "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    
   
    params.push_back (std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.4f));
    
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERATTACK", "Filter Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERDECAY", "Filter Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERSUSTAIN", "Filter Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRELEASE", "Filter Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.4f));
    
   
    params.push_back (std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray { "Low-Pass", "Band-Pass", "High-Pass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERFREQ", "Filter Freq", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRES", "Filter Resonance", juce::NormalisableRange<float> { 1.0f, 10.0f, 0.1f }, 1.0f));
    
 
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CHORUSDEPTH", "Chorus Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f
        ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CHORUSRATE", "Chorus Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 0.1f
        ));

    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DELAYTIME", "Delay Time (ms)",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f), 1.0f
        ));

   
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "REVERBROOM", "Reverb Room Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f
        ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "REVERBMIX", "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f
        ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("DISTDRIVE", "Distortion Drive", juce::NormalisableRange<float>(0.0f, 40.0f, 0.1f), 10.0f));
    


    auto makeFxTypeParam = [](int i)
    {
        return std::make_unique<juce::AudioParameterChoice>(
            "FX" + juce::String(i) + "_TYPE",
            "FX" + juce::String(i) + " Type",
            juce::StringArray{ "Empty", "Distortion", "Chorus", "Delay", "Reverb" },
            0
            );
    };

    auto makeFloat = [](juce::String id, juce::String name,
        float minV, float maxV, float step, float defV,
        float skew = 1.0f)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            id, name,
            juce::NormalisableRange<float>(minV, maxV, step, skew),
            defV
            );
    };

    for (int i = 1; i <= 10; ++i)
    {
        params.push_back(makeFxTypeParam(i));

       
        params.push_back(makeFloat("FX" + juce::String(i) + "_DISTDRIVE",
            "FX" + juce::String(i) + " Dist Drive (dB)",
            0.0f, 40.0f, 0.1f, 10.0f));

        params.push_back(makeFloat("FX" + juce::String(i) + "_CHORUSDEPTH",
            "FX" + juce::String(i) + " Chorus Depth",
            0.0f, 1.0f, 0.001f, 0.25f));
        params.push_back(makeFloat("FX" + juce::String(i) + "_CHORUSRATE",
            "FX" + juce::String(i) + " Chorus Rate",
            0.01f, 10.0f, 0.001f, 0.8f, 0.6f));

      
        params.push_back(makeFloat("FX" + juce::String(i) + "_DELAYTIME",
            "FX" + juce::String(i) + " Delay Time (ms)",
            1.0f, 2000.0f, 1.0f, 350.0f, 0.5f));
        params.push_back(makeFloat("FX" + juce::String(i) + "_DELAYFB",
            "FX" + juce::String(i) + " Delay Feedback",
            0.0f, 0.95f, 0.001f, 0.35f));
        params.push_back(makeFloat("FX" + juce::String(i) + "_DELAYMIX",
            "FX" + juce::String(i) + " Delay Mix",
            0.0f, 1.0f, 0.001f, 0.25f));

       
        params.push_back(makeFloat("FX" + juce::String(i) + "_REVROOM",
            "FX" + juce::String(i) + " Reverb Room",
            0.0f, 1.0f, 0.001f, 0.3f));
        params.push_back(makeFloat("FX" + juce::String(i) + "_REVMIX",
            "FX" + juce::String(i) + " Reverb Mix",
            0.0f, 1.0f, 0.001f, 0.2f));
    }
    return { params.begin(), params.end() };

}
void TapSynthAudioProcessor::setFxOrder(const std::array<int, 4>& newOrder)
{
    fxOrder = newOrder;
}
