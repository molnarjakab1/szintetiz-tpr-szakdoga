
#include <JuceHeader.h>
#include "FilterComponent.h"

FilterComponent::FilterComponent (juce::AudioProcessorValueTreeState& apvts, juce::String filterTypeSelectorId, juce::String filterFreqId, juce::String filterResId)
{
    juce::StringArray choices { "Low - Pass", "High - Pass", "Band - Pass" };
    filterTypeSelector.addItemList (choices, 1);
    addAndMakeVisible (filterTypeSelector);
    
    filterTypeSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, filterTypeSelectorId, filterTypeSelector);
    
    filterSelectorLabel.setColour (juce::Label::ColourIds::textColourId, juce::Colours::white);
    filterSelectorLabel.setFont (15.0f);
    filterSelectorLabel.setJustificationType (juce::Justification::left);
    addAndMakeVisible (filterSelectorLabel);
    
    setSliderWithLabel (filterFreqSlider, filterFreqLabel, apvts, filterFreqId, filterFreqAttachment, filterFreqMinus, filterFreqPlus);
    setSliderWithLabel (filterResSlider, filterResLabel, apvts, filterResId, filterResAttachment, filterResMinus, filterResPlus);
}

FilterComponent::~FilterComponent()
{
}

void FilterComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced (5);
    auto labelSpace = bounds.removeFromTop (25.0f);
    
    g.fillAll (juce::Colour::fromRGB(30,30,30));
    g.setColour (juce::Colours::whitesmoke);
    g.setFont (15.0f);
    g.drawText ("Szuro", labelSpace.withX (5), juce::Justification::left);
    g.drawRoundedRectangle (bounds.toFloat(), 5.0f, 1.5f);
}

void FilterComponent::resized()
{
    const auto startY = 55;
    const auto sliderWidth = 100;
    const auto sliderHeight = 90;
    const auto labelYOffset = 20;
    const auto labelHeight = 20;
    
    filterTypeSelector.setBounds (10, startY + 5, 90, 30);
    filterSelectorLabel.setBounds (10, startY - labelYOffset, 90, labelHeight);
    
    filterFreqSlider.setBounds (filterTypeSelector.getRight(), startY, sliderWidth, sliderHeight);
    filterFreqLabel.setBounds (filterFreqSlider.getX(), filterFreqSlider.getY() - labelYOffset, filterFreqSlider.getWidth(), labelHeight);

    {
        auto sb = filterFreqSlider.getBounds();
        const int btnH = 16;
        const int btnW = sb.getWidth() / 2 - 2;
        int btnY = sb.getBottom() + 2;

        filterFreqMinus.setBounds(sb.getX(), btnY, btnW, btnH);
        filterFreqPlus.setBounds(sb.getX() + btnW + 4, btnY, btnW, btnH);


    }

    filterResSlider.setBounds (filterFreqSlider.getRight(), startY, sliderWidth, sliderHeight);
    filterResLabel.setBounds (filterResSlider.getX(), filterResSlider.getY() - labelYOffset, filterResSlider.getWidth(), labelHeight);

    {
        auto sb2 = filterResSlider.getBounds();
        const int btnH = 16;
        const int btnW = sb2.getWidth() / 2 - 2;
        int btnY = sb2.getBottom() + 2;

        filterResMinus.setBounds(sb2.getX(), btnY, btnW, btnH);
        filterResPlus.setBounds(sb2.getX() + btnW + 4, btnY, btnW, btnH);
    }

}

using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

void FilterComponent::setSliderWithLabel (juce::Slider& slider, juce::Label& label, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment, juce::TextButton& minusButton, juce::TextButton& plusButton)
{
    slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible (slider);
    
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);
    
    label.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    label.setFont (juce::Font(13.0f, juce::Font::plain));
    label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (label);


    minusButton.setButtonText("-");
    plusButton.setButtonText("+");
    minusButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    plusButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addAndMakeVisible(minusButton);
    addAndMakeVisible(plusButton);

    auto adjust = [&slider](bool up)
    {
        const double min = slider.getMinimum();
        const double max = slider.getMaximum();
        double step = (max - min) * 0.01;

        if (step <= 0.0)
            step = 0.001;

        const double newVal = slider.getValue() + (up ? step : -step);
        slider.setValue(juce::jlimit(min, max, newVal), juce::sendNotificationSync);
    };
    minusButton.onClick = [adjust] {adjust(false); };
    plusButton.onClick = [adjust] {adjust(true); };

}
