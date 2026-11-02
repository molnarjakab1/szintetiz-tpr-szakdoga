#pragma once

#include <JuceHeader.h>

class AdsrComponent : public juce::Component
{
public:
    AdsrComponent(juce::String name,
        juce::AudioProcessorValueTreeState& apvts,
        juce::String attackId,
        juce::String decayId,
        juce::String sustainId,
        juce::String releaseId);
    ~AdsrComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    void setSliderWithLabel(juce::Slider& slider,
        juce::Label& label,
        juce::AudioProcessorValueTreeState& apvts,
        juce::String paramId,
        std::unique_ptr<Attachment>& attachment,
        juce::TextButton& minusButton,
        juce::TextButton& plusButton);

    // Sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    // Attachments
    std::unique_ptr<Attachment> attackAttachment;
    std::unique_ptr<Attachment> decayAttachment;
    std::unique_ptr<Attachment> sustainAttachment;
    std::unique_ptr<Attachment> releaseAttachment;

    // Labels
    juce::Label attackLabel{ "Attack", "A" };
    juce::Label decayLabel{ "Decay",  "D" };
    juce::Label sustainLabel{ "Sustain","S" };
    juce::Label releaseLabel{ "Release","R" };

    // +/- buttons for fine control
    juce::TextButton attackMinus{ "-" }, attackPlus{ "+" };
    juce::TextButton decayMinus{ "-" }, decayPlus{ "+" };
    juce::TextButton sustainMinus{ "-" }, sustainPlus{ "+" };
    juce::TextButton releaseMinus{ "-" }, releasePlus{ "+" };

    juce::String componentName{ "" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrComponent)
};
