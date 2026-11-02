

#pragma once

#include <JuceHeader.h>

class OscComponent  : public juce::Component
{
public:
    OscComponent (juce::AudioProcessorValueTreeState& apvts, juce::String waveSelectorId, juce::String fmFreqId, juce::String fmDepthId);
    ~OscComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::ComboBox oscWaveSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscWaveSelectorAttachment;
    
    juce::Slider fmFreqSlider;
    juce::Slider fmDepthSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment> fmFreqAttachment;
    std::unique_ptr<Attachment> fmDepthAttachment;
    
    juce::Label waveSelectorLabel { "Waveform", "Waveform" };
    juce::Label fmFreqLabel { "FM frequency", "FM frequency" };
    juce::Label fmDepthLabel { "FM depth", "FM depth" };
    juce::TextButton fmFreqMinus{ "-" }, fmFreqPlus{ "+" };
    juce::TextButton fmDepthMinus{ "-" }, fmDepthPlus{ "+" };

    
    void setSliderWithLabel (juce::Slider& slider, juce::Label& label, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment,juce::TextButton& minusButton, juce::TextButton& plusButton);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscComponent)
};
