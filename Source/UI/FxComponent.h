#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>

class FxComponent : public juce::Component
{
public:
    FxComponent(juce::AudioProcessorValueTreeState& apvts);
    ~FxComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(const std::array<int, 4>&)> onOrderChanged;


private:

    enum class EffectType { Dist = 0, Chorus = 1, Delay = 2, Reverb = 3 };

    struct Panel
    {
        EffectType type;
        juce::Rectangle<int> bounds;
    };

    std::array<Panel, 4> panels;
    int draggingIndex = -1;
    int dragOffsetX = 0;




    juce::Slider distDrive;
    juce::Slider chorusDepth;
    juce::Slider chorusRate;
    juce::Slider delayTime;
    juce::Slider reverbRoom;
    juce::Slider reverbMix;

 
    juce::Label distLabel{ "DistDriveLabel",     "Drive" };
    juce::Label chorusDepthLbl{ "ChorusDepthLabel",   "Depth" };
    juce::Label chorusRateLbl{ "ChorusRateLabel",    "Rate" };
    juce::Label delayTimeLbl{ "DelayTimeLabel",     "Time" };
    juce::Label reverbRoomLbl{ "ReverbRoomLabel",    "Room" };
    juce::Label reverbMixLbl{ "ReverbMixLabel",     "Mix" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> distDriveAttach;
    std::unique_ptr<SliderAttachment> depthAttach;
    std::unique_ptr<SliderAttachment> rateAttach;
    std::unique_ptr<SliderAttachment> timeAttach;
    std::unique_ptr<SliderAttachment> roomAttach;
    std::unique_ptr<SliderAttachment> mixAttach;


    juce::Rectangle<int> distArea;
    juce::Rectangle<int> chorusArea;
    juce::Rectangle<int> delayArea;
    juce::Rectangle<int> reverbArea;


    juce::TextButton distMinus{ "-" }, distPlus{ "+" };
    juce::TextButton chorusDepthMinus{ "-" }, chorusDepthPlus{ "+" };
    juce::TextButton chorusRateMinus{ "-" }, chorusRatePlus{ "+" };
    juce::TextButton delayMinus{ "-" }, delayPlus{ "+" };
    juce::TextButton reverbRoomMinus{ "-" }, reverbRoomPlus{ "+" };
    juce::TextButton reverbMixMinus{ "-" }, reverbMixPlus{ "+" };

    void setupSliderWithLabel(juce::Slider& slider,
        juce::Label& label,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::String& paramId,
        std::unique_ptr<SliderAttachment>& attach,
        juce::TextButton& minusButton,
        juce::TextButton& plusButton);

    void layoutPanel(const Panel& p); 

    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxComponent)
};
