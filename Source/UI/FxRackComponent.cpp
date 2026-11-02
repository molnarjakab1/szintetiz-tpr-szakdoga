#include "FxRackComponent.h"

FxRackComponent::FxRackComponent(juce::AudioProcessorValueTreeState& apvts) :state(apvts) {
    title.setJustificationType(juce::Justification::centred);
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    title.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(title);

    viewport.setViewedComponent(&slotContainer, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);

    addAndMakeVisible(addEffectButton);
    addEffectButton.onClick = [this] { addEffect(); };

    for (int i = 1; i <= 10; ++i)
    {
        auto* s = slots.add(new FxSlotComponent(apvts, i));
        s->onDeleteRequested = [this](int slotIndex)
        {
            removeEffect(slotIndex);
        };

        slotContainer.addAndMakeVisible(s);
    }

    refreshVisibleSlots();


}
int FxRackComponent::getSlotType(int slotIndex) const
{
    auto id = "FX" + juce::String(slotIndex) + "_TYPE";
    return (int)state.getRawParameterValue(id)->load();
}

void FxRackComponent::setSlotType(int slotIndex, int typeIndex)
{
    auto id = "FX" + juce::String(slotIndex) + "_TYPE";

    if (auto* p = state.getParameter(id))
    {
        auto* choice = dynamic_cast<juce::AudioParameterChoice*>(p);
        if (choice != nullptr)
        {
            const int numChoices = choice->choices.size();
            float norm = (numChoices > 1) ? (float)typeIndex / (float)(numChoices - 1) : 0.0f;
            p->setValueNotifyingHost(norm);
        }
    }
}
void FxRackComponent::clearSlot(int slotIndex)
{
    setSlotType(slotIndex, 0);

    auto prefix = "FX" + juce::String(slotIndex) + "_";

    auto resetFloat = [this](const juce::String& id)
    {
        if (auto* p = state.getParameter(id))
            p->setValueNotifyingHost(p->getDefaultValue());
    };

    resetFloat(prefix + "DISTDRIVE");
    resetFloat(prefix + "CHORUSDEPTH");
    resetFloat(prefix + "CHORUSRATE");
    resetFloat(prefix + "DELAYTIME");
    resetFloat(prefix + "DELAYFB");
    resetFloat(prefix + "DELAYMIX");
    resetFloat(prefix + "REVROOM");
    resetFloat(prefix + "REVMIX");
}
void FxRackComponent::copySlotState(int fromSlot, int toSlot)
{
    const juce::String fromPrefix = "FX" + juce::String(fromSlot) + "_";
    const juce::String toPrefix = "FX" + juce::String(toSlot) + "_";

    auto copyParam = [this](const juce::String& fromId, const juce::String& toId)
    {
        auto* fromP = state.getParameter(fromId);
        auto* toP = state.getParameter(toId);

        if (fromP != nullptr && toP != nullptr)
            toP->setValueNotifyingHost(fromP->getValue());
    };

    copyParam(fromPrefix + "TYPE", toPrefix + "TYPE");
    copyParam(fromPrefix + "DISTDRIVE", toPrefix + "DISTDRIVE");
    copyParam(fromPrefix + "CHORUSDEPTH", toPrefix + "CHORUSDEPTH");
    copyParam(fromPrefix + "CHORUSRATE", toPrefix + "CHORUSRATE");
    copyParam(fromPrefix + "DELAYTIME", toPrefix + "DELAYTIME");
    copyParam(fromPrefix + "DELAYFB", toPrefix + "DELAYFB");
    copyParam(fromPrefix + "DELAYMIX", toPrefix + "DELAYMIX");
    copyParam(fromPrefix + "REVROOM", toPrefix + "REVROOM");
    copyParam(fromPrefix + "REVMIX", toPrefix + "REVMIX");
}
void FxRackComponent::compactSlots()
{
    int writeSlot = 1;

    for (int readSlot = 1; readSlot <= 10; ++readSlot)
    {
        if (getSlotType(readSlot) != 0) 
        {
            if (readSlot != writeSlot)
            {
                copySlotState(readSlot, writeSlot);
                clearSlot(readSlot);
            }
            ++writeSlot;
        }
    }

    for (int i = writeSlot; i <= 10; ++i)
        clearSlot(i);
}

void FxRackComponent::addEffect()
{
    for (int i = 1; i <= 10; ++i)
    {
        if (getSlotType(i) == 0)
        {
            setSlotType(i, 1); 
            refreshVisibleSlots();
            resized();
            repaint();
            return;
        }
    }
}

void FxRackComponent::removeEffect(int slotIndex)
{
    clearSlot(slotIndex);
    compactSlots();
    refreshVisibleSlots();
    resized();
    repaint();
}

void FxRackComponent::refreshVisibleSlots()
{
    int usedCount = 0;
    for (int i = 1; i <= 10; ++i)
        if (getSlotType(i) != 0)
            ++usedCount;

    for (int i = 0; i < slots.size(); ++i)
        slots[i]->setVisible(i < usedCount);

    addEffectButton.setEnabled(usedCount < 10);
}



void FxRackComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour::fromRGB(15, 15, 20));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);
}

void FxRackComponent::resized()
{
    auto r = getLocalBounds().reduced(8);

    title.setBounds(r.removeFromTop(24));
    addEffectButton.setBounds(r.removeFromTop(28).removeFromLeft(140));
    r.removeFromTop(6);

    viewport.setBounds(r);

    const int rowH = 130;
    const int gap = 8;

    int visibleCount = 0;
    for (auto* s : slots)
        if (s->isVisible())
            ++visibleCount;

    int y = 0;
    for (auto* s : slots)
    {
        if (!s->isVisible())
            continue;

        s->setBounds(0, y, viewport.getWidth() - 14, rowH);
        y += rowH + gap;
    }

    slotContainer.setSize(viewport.getWidth(), juce::jmax(y, viewport.getHeight()));
}