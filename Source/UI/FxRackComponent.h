#pragma once
#include <JuceHeader.h>
#include "FxSlotComponent.h"

class FxRackComponent : public juce::Component
{
public:
    FxRackComponent(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Label title{ "fxTitle", "Effektek" };

    juce::Viewport viewport;
    juce::Component slotContainer;

    juce::OwnedArray<FxSlotComponent> slots;

    juce::TextButton addEffectButton{ "+ Add Effect" };

    juce::AudioProcessorValueTreeState& state;

    int getSlotType(int slotIndex) const;
    void setSlotType(int slotIndex, int typeIndex);
    void clearSlot(int slotIndex);
    void copySlotState(int fromSlot, int toSlot);
    void compactSlots();
    void refreshVisibleSlots();
    void addEffect();
    void removeEffect(int slotIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxRackComponent)
};