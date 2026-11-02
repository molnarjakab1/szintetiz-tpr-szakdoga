#pragma once
#include <JuceHeader.h>
#include <functional>

class FxSlotComponent : public juce::Component
{
public:
    FxSlotComponent(juce::AudioProcessorValueTreeState& apvts, int slotIndex);
    void resized() override;

    std::function<void(int)> onDeleteRequested;
    int getSlotIndex() const { return slot; };

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    int slot = 1;

    juce::ComboBox typeBox;

    juce::Slider a, b, c; 
    juce::Label  aLbl, bLbl, cLbl;

    std::unique_ptr<ComboAttachment> typeAttach;
    std::unique_ptr<SliderAttachment> aAttach, bAttach, cAttach;

    juce::AudioProcessorValueTreeState& state;

    juce::TextButton removeButton{ "X" };

    void setControl(juce::Slider& s, juce::Label& l,
        const juce::String& id, const juce::String& text,
        std::unique_ptr<SliderAttachment>& att);

    void updateForType();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxSlotComponent)
};