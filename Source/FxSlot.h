#pragma once
#include <JuceHeader.h>

struct FxSlot
{
    void prepare(double sampleRate, int maxBlock, int numChannels)
    {
        sr = sampleRate;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = (juce::uint32)maxBlock;
        spec.numChannels = (juce::uint32)numChannels;

        chorus.prepare(spec);
        chorus.setCentreDelay(7.0f);
        chorus.setFeedback(0.0f);
        chorus.setMix(1.0f);

       
        const int maxDelaySamples = (int)(2.5 * sampleRate); 
        delay = juce::dsp::DelayLine<float>(maxDelaySamples);
        delay.prepare(spec);

        reverb.prepare(spec);
    }


    void process(juce::AudioBuffer<float>& buffer,
        int type,
        float distDriveDb,
        float chorusDepth, float chorusRate,
        float delayTimeMs, float delayFeedback, float delayMix,
        float revRoom, float revMix)
    {
        if (type == 0)
            return;

        juce::dsp::AudioBlock<float> block(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);

        switch (type)
        {
        case 1: 
        {
            const float drive = juce::Decibels::decibelsToGain(distDriveDb);
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                    data[i] = std::tanh(data[i] * drive);
            }
            break;
        }

        case 2: 
            chorus.setDepth(chorusDepth);
            chorus.setRate(chorusRate);
            chorus.process(ctx);
            break;

        case 3: 
        {
           
            const float delaySamples = (delayTimeMs * 0.001f) * (float)sr;
            delay.setDelay((size_t)juce::jlimit(1.0f, (float)delay.getMaximumDelayInSamples(), delaySamples));

          
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int n = 0; n < buffer.getNumSamples(); ++n)
                {
                    float in = data[n];
                    float d = delay.popSample((size_t)ch);
                    delay.pushSample((size_t)ch, in + d * delayFeedback);

                    float wet = d;
                    float out = in * (1.0f - delayMix) + wet * delayMix;
                    data[n] = out;
                }
            }
            break;
        }

        case 4: 
        {
            juce::dsp::Reverb::Parameters p;
            p.roomSize = revRoom;
            p.wetLevel = revMix;
            p.dryLevel = 1.0f - revMix;
            reverb.setParameters(p);
            reverb.process(ctx);
            break;
        }
        }
    }

    double sr = 44100.0;

    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float> delay{ 1 };
    juce::dsp::Reverb reverb;
};