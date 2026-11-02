#include "FxSlotComponent.h"

FxSlotComponent::FxSlotComponent(juce::AudioProcessorValueTreeState& apvts, int slotIndex)
    : slot(slotIndex), state(apvts)
{
  
    typeBox.addItemList({ "Empty", "Distortion", "Chorus", "Delay", "Reverb" }, 1);
    addAndMakeVisible(typeBox);

    typeAttach = std::make_unique<ComboAttachment>(state, "FX" + juce::String(slot) + "_TYPE", typeBox);

    addAndMakeVisible(removeButton);
    removeButton.onClick = [this]
    {
        if (onDeleteRequested)
            onDeleteRequested(slot);
    };

   
    for (auto* s : { &a, &b, &c })
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 46, 18);
        addAndMakeVisible(*s);
    }

    for (auto* l : { &aLbl, &bLbl, &cLbl })
    {
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(*l);
    }

    typeBox.onChange = [this] { updateForType(); };
    updateForType();
}

void FxSlotComponent::setControl(juce::Slider& s, juce::Label& l,
    const juce::String& id,
    const juce::String& text,
    std::unique_ptr<SliderAttachment>& att)
{
    l.setText(text, juce::dontSendNotification);
    att = std::make_unique<SliderAttachment>(state, id, s);
}

void FxSlotComponent::updateForType()
{
   
    aAttach.reset(); bAttach.reset(); cAttach.reset();

   
    a.setVisible(false); b.setVisible(false); c.setVisible(false);
    aLbl.setVisible(false); bLbl.setVisible(false); cLbl.setVisible(false);

    const int type = typeBox.getSelectedItemIndex();

    if (type == 1) 
    {
        setControl(a, aLbl, "FX" + juce::String(slot) + "_DISTDRIVE", "Drive", aAttach);
        a.setVisible(true); aLbl.setVisible(true);
    }
    else if (type == 2) 
    {
        setControl(a, aLbl, "FX" + juce::String(slot) + "_CHORUSDEPTH", "Depth", aAttach);
        setControl(b, bLbl, "FX" + juce::String(slot) + "_CHORUSRATE", "Rate", bAttach);
        a.setVisible(true); b.setVisible(true);
        aLbl.setVisible(true); bLbl.setVisible(true);
    }
    else if (type == 3) 
    {
        setControl(a, aLbl, "FX" + juce::String(slot) + "_DELAYTIME", "Time", aAttach);
        setControl(b, bLbl, "FX" + juce::String(slot) + "_DELAYFB", "FB", bAttach);
        setControl(c, cLbl, "FX" + juce::String(slot) + "_DELAYMIX", "Mix", cAttach);
        a.setVisible(true); b.setVisible(true); c.setVisible(true);
        aLbl.setVisible(true); bLbl.setVisible(true); cLbl.setVisible(true);
    }
    else if (type == 4) 
    {
        setControl(a, aLbl, "FX" + juce::String(slot) + "_REVROOM", "Room", aAttach);
        setControl(b, bLbl, "FX" + juce::String(slot) + "_REVMIX", "Mix", bAttach);
        a.setVisible(true); b.setVisible(true);
        aLbl.setVisible(true); bLbl.setVisible(true);
    }

    resized();
}

void FxSlotComponent::resized()
{
    auto r = getLocalBounds().reduced(8);

  
    auto topRow = r.removeFromTop(28);

    typeBox.setBounds(topRow.removeFromLeft(170));
    topRow.removeFromLeft(8);
    removeButton.setBounds(topRow.removeFromLeft(28));

    r.removeFromTop(8);

    const int visibleCount = (int)a.isVisible() + (int)b.isVisible() + (int)c.isVisible();
    if (visibleCount == 0)
        return;


    auto knobArea = r.reduced(6, 0);

   
    const int knobSize = 64;
    const int labelH = 18;
    const int gap = 18;

  
    const int totalWidth = visibleCount * knobSize + (visibleCount - 1) * gap;

    int startX = knobArea.getX() + (knobArea.getWidth() - totalWidth) / 2;
    int knobY = knobArea.getY();

    auto place = [&](juce::Slider& s, juce::Label& l, int index)
    {
        int x = startX + index * (knobSize + gap);
        s.setBounds(x, knobY, knobSize, knobSize);
        l.setBounds(x, knobY + knobSize + 2, knobSize, labelH);
    };

    int index = 0;
    if (a.isVisible()) place(a, aLbl, index++);
    if (b.isVisible()) place(b, bLbl, index++);
    if (c.isVisible()) place(c, cLbl, index++);
}

//void FxSlotComponent::resized()
//{
//    auto r = getLocalBounds().reduced(8);
//
//    // Left column for type selector
//    auto left = r.removeFromLeft(170);
//    typeBox.setBounds(left.removeFromTop(26));
//
//    // Right side for knobs
//    const int gap = 10;
//
//    int visibleCount = (int)a.isVisible() + (int)b.isVisible() + (int)c.isVisible();
//    if (visibleCount == 0)
//        return;
//
//    auto knobArea = r;                 // remaining area
//    knobArea.removeFromTop(6);         // small padding
//    knobArea.reduce(6, 4);
//
//    int colW = (knobArea.getWidth() - (visibleCount - 1) * gap) / visibleCount;
//    int knobSize = juce::jmin(colW, knobArea.getHeight() - 22);
//
//    auto place = [&](juce::Slider& s, juce::Label& l)
//    {
//        auto col = knobArea.removeFromLeft(colW);
//        if (knobArea.getWidth() > 0) knobArea.removeFromLeft(gap);
//
//        auto knob = col.removeFromTop(knobSize);
//        knob = knob.withSizeKeepingCentre(knobSize, knobSize);
//
//        s.setBounds(knob);
//        l.setBounds(col.removeFromTop(18));
//        l.setBounds(knob.getX(), knob.getBottom() + 2, knob.getWidth(), 18);
//    };
//
//    if (a.isVisible()) place(a, aLbl);
//    if (b.isVisible()) place(b, bLbl);
//    if (c.isVisible()) place(c, cLbl);
//}