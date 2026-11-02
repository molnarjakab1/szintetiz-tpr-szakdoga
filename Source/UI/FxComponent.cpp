#include "FxComponent.h"

FxComponent::FxComponent(juce::AudioProcessorValueTreeState& apvts)
{
    panels[0].type = EffectType::Dist;
    panels[1].type = EffectType::Chorus;
    panels[2].type = EffectType::Delay;
    panels[3].type = EffectType::Reverb;

    setupSliderWithLabel(distDrive, distLabel, apvts, "DISTDRIVE", distDriveAttach, distMinus, distPlus);
    setupSliderWithLabel(chorusDepth, chorusDepthLbl, apvts, "CHORUSDEPTH", depthAttach, chorusDepthMinus, chorusDepthPlus);
    setupSliderWithLabel(chorusRate, chorusRateLbl, apvts, "CHORUSRATE", rateAttach, chorusRateMinus, chorusRatePlus);
    setupSliderWithLabel(delayTime, delayTimeLbl, apvts, "DELAYTIME", timeAttach, delayMinus, delayPlus);
    setupSliderWithLabel(reverbRoom, reverbRoomLbl, apvts, "REVERBROOM", roomAttach, reverbRoomMinus, reverbRoomPlus);
    setupSliderWithLabel(reverbMix, reverbMixLbl, apvts, "REVERBMIX", mixAttach, reverbMixMinus, reverbMixPlus);
}

void FxComponent::setupSliderWithLabel(juce::Slider& slider,
    juce::Label& label,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::String& paramId,
    std::unique_ptr<SliderAttachment>& attach,
    juce::TextButton& minusButton,
    juce::TextButton& plusButton)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 46, 18);
    slider.setNumDecimalPlacesToDisplay(3);
    slider.setVelocityBasedMode(true);
    slider.setVelocityModeParameters(0.1, 1.0, 0.1);

    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB(45, 50, 60));
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(0, 200, 220));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 220, 240));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(10, 10, 15));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(slider);

    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::Font(13.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    attach = std::make_unique<SliderAttachment>(apvts, paramId, slider);


}


void FxComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);

    g.setColour(juce::Colour::fromRGB(15, 15, 20));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);

    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Effektek", bounds.removeFromTop(22), juce::Justification::centred);

    for (auto& p : panels)
    {
        auto panelBounds = p.bounds.reduced(2);
        g.setColour(juce::Colour::fromRGB(28, 30, 40));      
        g.fillRoundedRectangle(panelBounds.toFloat(), 6.0f);

        g.setColour(juce::Colour::fromRGB(0, 200, 220));     
        g.drawRoundedRectangle(panelBounds.toFloat(), 6.0f, 1.8f);

        auto titleArea = panelBounds.removeFromTop(22);
        g.setColour(juce::Colour::fromRGB(40, 45, 60));
        g.fillRect(titleArea);

        juce::String title;
        switch (p.type)
        {
        case EffectType::Dist:   title = "Distortion"; break;
        case EffectType::Chorus: title = "Chorus";     break;
        case EffectType::Delay:  title = "Delay";      break;
        case EffectType::Reverb: title = "Reverb";     break;
        }

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(title, titleArea.reduced(4, 0), juce::Justification::centredLeft);
    }
}



//void FxComponent::paint(juce::Graphics& g)
//{
//    auto bounds = getLocalBounds().reduced(5);
//
//    g.setColour(juce::Colour::fromRGB(20, 20, 20));
//    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
//
//    g.setColour(juce::Colours::white);
//    g.setFont(16.0f);
//    g.drawText("Effektek", bounds.removeFromTop(20), juce::Justification::centred);
//
//    g.setColour(juce::Colours::darkgrey);
//    g.drawRoundedRectangle(distArea.toFloat(), 6.0f, 1.5f);
//    g.drawRoundedRectangle(chorusArea.toFloat(), 6.0f, 1.5f);
//    g.drawRoundedRectangle(delayArea.toFloat(), 6.0f, 1.5f);
//    g.drawRoundedRectangle(reverbArea.toFloat(), 6.0f, 1.5f);
//
//    g.setFont(14.0f);
//    g.setColour(juce::Colours::white);
//    g.drawText("Distortion", distArea.reduced(4).removeFromTop(20), juce::Justification::centredLeft);
//    g.drawText("Chorus", chorusArea.reduced(4).removeFromTop(20), juce::Justification::centredLeft);
//    g.drawText("Delay", delayArea.reduced(4).removeFromTop(20), juce::Justification::centredLeft);
//    g.drawText("Reverb", reverbArea.reduced(4).removeFromTop(20), juce::Justification::centredLeft);
//}
void FxComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    area.removeFromTop(24); 

    const int gap = 8;

    const int minPanelWidth = 110; 
    int totalWidth = area.getWidth();

    int colW = (totalWidth - 3 * gap) / 4;
    if (colW < minPanelWidth)
        colW = minPanelWidth;

    int usedWidth = colW * 4 + gap * 3;
    int startX = area.getX() + (area.getWidth() - usedWidth) / 2;

    const int h = area.getHeight();

    int x = startX;
    for (auto& p : panels)
    {
        p.bounds = { x, area.getY(), colW, h };
        x += colW + gap;
    }


    for (auto& p : panels)
        layoutPanel(p);
}




//void FxComponent::resized()
//{
//    auto area = getLocalBounds().reduced(10);
//    area.removeFromTop(24);
//
//    const int gap = 8;
//    const int colW = (area.getWidth() - 3 * gap) / 4;
//
//    distArea = { area.getX(),                     area.getY(), colW, area.getHeight() };
//    chorusArea = { distArea.getRight() + gap,       area.getY(), colW, area.getHeight() };
//    delayArea = { chorusArea.getRight() + gap,     area.getY(), colW, area.getHeight() };
//    reverbArea = { delayArea.getRight() + gap,      area.getY(), colW, area.getHeight() };
//
//    auto place = [](juce::Rectangle<int> box,
//        juce::Slider& slider,
//        juce::Label& label,
//        juce::TextButton& minusButton,
//        juce::TextButton& plusButton)
//    {
//        auto inner = box.reduced(10, 28);
//        int knobSize = juce::jmin(inner.getWidth(), inner.getHeight() - 36); // leave room for buttons
//
//        slider.setBounds(inner.withSizeKeepingCentre(knobSize, knobSize));
//        label.setBounds(slider.getX(), slider.getBottom() + 2, slider.getWidth(), 16);
//
//        const int btnH = 16;
//        const int btnW = slider.getWidth() / 2 - 2;
//        int btnY = label.getBottom() + 2;
//
//        minusButton.setBounds(slider.getX(), btnY, btnW, btnH);
//        plusButton.setBounds(slider.getX() + btnW + 4, btnY, btnW, btnH);
//    };
//
//    place(distArea, distDrive, distLabel, distMinus, distPlus);
//    place(delayArea, delayTime, delayTimeLbl, delayMinus, delayPlus);
//
//    place(chorusArea, chorusDepth, chorusDepthLbl, chorusDepthMinus, chorusDepthPlus);
//    // manual placement for chorusRate below chorusDepth:
//    {
//        auto box = chorusArea.reduced(10, 28);
//        int knobSize = 60;
//        int y = chorusDepth.getBottom() + 24;
//
//        chorusRate.setBounds(box.withSizeKeepingCentre(knobSize, knobSize).withY(y));
//        chorusRateLbl.setBounds(chorusRate.getX(), chorusRate.getBottom() + 2, chorusRate.getWidth(), 16);
//
//        int btnH = 16;
//        int btnW = chorusRate.getWidth() / 2 - 2;
//        int btnY = chorusRateLbl.getBottom() + 2;
//
//        chorusRateMinus.setBounds(chorusRate.getX(), btnY, btnW, btnH);
//        chorusRatePlus.setBounds(chorusRate.getX() + btnW + 4, btnY, btnW, btnH);
//    }
//
//    place(reverbArea, reverbRoom, reverbRoomLbl, reverbRoomMinus, reverbRoomPlus);
//    {
//        auto box = reverbArea.reduced(10, 28);
//        int knobSize = 60;
//        int y = reverbRoom.getBottom() + 24;
//
//        reverbMix.setBounds(box.withSizeKeepingCentre(knobSize, knobSize).withY(y));
//        reverbMixLbl.setBounds(reverbMix.getX(), reverbMix.getBottom() + 2, reverbMix.getWidth(), 16);
//
//        int btnH = 16;
//        int btnW = reverbMix.getWidth() / 2 - 2;
//        int btnY = reverbMixLbl.getBottom() + 2;
//
//        reverbMixMinus.setBounds(reverbMix.getX(), btnY, btnW, btnH);
//        reverbMixPlus.setBounds(reverbMix.getX() + btnW + 4, btnY, btnW, btnH);
//    }
//}
void FxComponent::layoutPanel(const Panel& p)
{
   
    auto panel = p.bounds;


    const int border = 6;
    const int titleSpace = 26;  

    auto content = panel.reduced(border);
    content.removeFromTop(titleSpace);      


    auto inner = content.reduced(6, 6);

 
    auto placeOne = [](juce::Rectangle<int> area,
        juce::Slider& slider,
        juce::Label& label,
        juce::TextButton* minusBtn = nullptr,
        juce::TextButton* plusBtn = nullptr)
    {
        const int knobSize = juce::jmin(area.getWidth() - 20, area.getHeight() - 40);
        auto knobArea = juce::Rectangle<int>(
            area.getCentreX() - knobSize / 2,
            area.getY() + 6,
            knobSize,
            knobSize
            );

        slider.setBounds(knobArea);

        auto labelArea = juce::Rectangle<int>(
            knobArea.getX(),
            knobArea.getBottom() + 2,
            knobArea.getWidth(),
            16
            );
        label.setBounds(labelArea);

        if (minusBtn != nullptr && plusBtn != nullptr)
        {
            const int btnH = 16;
            const int btnW = (labelArea.getWidth() / 2) - 2;
            int btnY = labelArea.getBottom() + 2;

            minusBtn->setBounds(labelArea.getX(), btnY, btnW, btnH);
            plusBtn->setBounds(labelArea.getX() + btnW + 4, btnY, btnW, btnH);
        }
    };

    switch (p.type)
    {
    case EffectType::Dist:
    {
        
        placeOne(inner, distDrive, distLabel, &distMinus, &distPlus);
        break;
    }
    case EffectType::Delay:
    {
        placeOne(inner, delayTime, delayTimeLbl, &delayMinus, &delayPlus);
        break;
    }
    case EffectType::Chorus:
    {
     
        auto topHalf = inner.removeFromTop(inner.getHeight() / 2);
        auto bottomHalf = inner;

        placeOne(topHalf, chorusDepth, chorusDepthLbl,
            &chorusDepthMinus, &chorusDepthPlus);

        placeOne(bottomHalf, chorusRate, chorusRateLbl,
            &chorusRateMinus, &chorusRatePlus);
        break;
    }
    case EffectType::Reverb:
    {
     
        auto topHalf = inner.removeFromTop(inner.getHeight() / 2);
        auto bottomHalf = inner;

        placeOne(topHalf, reverbRoom, reverbRoomLbl,
            &reverbRoomMinus, &reverbRoomPlus);

        placeOne(bottomHalf, reverbMix, reverbMixLbl,
            &reverbMixMinus, &reverbMixPlus);
        break;
    }
    }
}

void FxComponent::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.getPosition();

    draggingIndex = -1;
    for (int i = 0; i < (int)panels.size(); ++i)
    {
        if (panels[i].bounds.contains(pos))
        {
            draggingIndex = i;
            dragOffsetX = pos.x - panels[i].bounds.getX();
            break;
        }
    }
}

void FxComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (draggingIndex < 0)
        return;

    auto& p = panels[(size_t)draggingIndex];
    auto b = p.bounds;


    b.setX(e.x - dragOffsetX);
    p.bounds = b;
    repaint();

    auto centreX = b.getCentreX();


    if (draggingIndex > 0 &&
        centreX < panels[(size_t)draggingIndex - 1].bounds.getCentreX())
    {
        std::swap(panels[(size_t)draggingIndex],
            panels[(size_t)draggingIndex - 1]);
        draggingIndex--;
        resized();
    }
  
    else if (draggingIndex < (int)panels.size() - 1 &&
        centreX > panels[(size_t)draggingIndex + 1].bounds.getCentreX())
    {
        std::swap(panels[(size_t)draggingIndex],
            panels[(size_t)draggingIndex + 1]);
        draggingIndex++;
        resized();
    }
}

void FxComponent::mouseUp(const juce::MouseEvent&)
{
    if (draggingIndex >= 0)
    {
        draggingIndex = -1;
        resized();

        if (onOrderChanged)
        {
            std::array<int, 4> order{};
            for (int i = 0; i < 4; ++i)
                order[i] = (int)panels[(size_t)i].type;

            onOrderChanged(order);
        }
    }
}
