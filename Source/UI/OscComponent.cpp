
#include <JuceHeader.h>
#include "OscComponent.h"

OscComponent::OscComponent (juce::AudioProcessorValueTreeState& apvts, juce::String waveSelectorId, juce::String fmFreqId, juce::String fmDepthId)
{
    juce::StringArray choices { "Szinusz", "Fureszfog", "Negyszog" };
    oscWaveSelector.addItemList (choices, 1);
    addAndMakeVisible (oscWaveSelector);
    
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, waveSelectorId, oscWaveSelector);
    
    waveSelectorLabel.setColour (juce::Label::ColourIds::textColourId, juce::Colours::white);
    waveSelectorLabel.setFont (15.0f);
    waveSelectorLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (waveSelectorLabel);
    
    setSliderWithLabel(fmFreqSlider, fmFreqLabel, apvts, fmFreqId, fmFreqAttachment,
        fmFreqMinus, fmFreqPlus);

    setSliderWithLabel(fmDepthSlider, fmDepthLabel, apvts, fmDepthId, fmDepthAttachment,
        fmDepthMinus, fmDepthPlus);
    
}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced (5);
    auto labelSpace = bounds.removeFromTop (25.0f);
    
    g.fillAll (juce::Colour::fromRGB(30,30,30));
    g.setColour (juce::Colours::lightgreen);
    g.setFont (15.0f);
    g.drawText ("Oszcillator", labelSpace.withX (5), juce::Justification::left);
    g.drawRoundedRectangle (bounds.toFloat(), 5.0f, 1.5f);
}

void OscComponent::resized()
{
    const auto startY = 55;
    const auto sliderWidth = 100;
    const auto sliderHeight = 90;
    const auto labelYOffset = 20;
    const auto labelHeight = 20;

    oscWaveSelector.setBounds(10, startY + 5, 90, 30);
    waveSelectorLabel.setBounds(10, startY - labelYOffset, 90, labelHeight);

    fmFreqSlider.setBounds(oscWaveSelector.getRight(), startY,
        sliderWidth, sliderHeight);
    fmFreqLabel.setBounds(fmFreqSlider.getX(),
        fmFreqSlider.getY() - labelYOffset,
        fmFreqSlider.getWidth(), labelHeight);

    {
        auto sb = fmFreqSlider.getBounds();
        const int btnH = 16;
        const int btnW = sb.getWidth() / 2 - 2;
        int btnY = sb.getBottom() + 2;

        fmFreqMinus.setBounds(sb.getX(), btnY, btnW, btnH);
        fmFreqPlus.setBounds(sb.getX() + btnW + 4, btnY, btnW, btnH);
    }

    fmDepthSlider.setBounds(fmFreqSlider.getRight(), startY,
        sliderWidth, sliderHeight);
    fmDepthLabel.setBounds(fmDepthSlider.getX(),
        fmDepthSlider.getY() - labelYOffset,
        fmDepthSlider.getWidth(), labelHeight);

    {
        auto sb2 = fmDepthSlider.getBounds();
        const int btnH2 = 16;
        const int btnW2 = sb2.getWidth() / 2 - 2;
        int btnY2 = sb2.getBottom() + 2;

        fmDepthMinus.setBounds(sb2.getX(), btnY2, btnW2, btnH2);
        fmDepthPlus.setBounds(sb2.getX() + btnW2 + 4, btnY2, btnW2, btnH2);
    }
}


using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

void OscComponent::setSliderWithLabel(juce::Slider& slider,
    juce::Label& label,
    juce::AudioProcessorValueTreeState& apvts,
    juce::String paramId,
    std::unique_ptr<Attachment>& attachment,
    juce::TextButton& minusButton,
    juce::TextButton& plusButton)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    slider.setNumDecimalPlacesToDisplay(3);
    slider.setVelocityBasedMode(true);
    slider.setVelocityModeParameters(0.1, 1.0, 0.1);
    addAndMakeVisible(slider);

    attachment = std::make_unique<Attachment>(apvts, paramId, slider);

    label.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    label.setFont(juce::Font(13.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    minusButton.setButtonText("-");
    plusButton.setButtonText("+");
    minusButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    plusButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addAndMakeVisible(minusButton);
    addAndMakeVisible(plusButton);

    auto adjust = [&slider](bool up)
    {
        double min = slider.getMinimum();
        double max = slider.getMaximum();
        double step = (max - min) * 0.01;

        if (step <= 0.0)
            step = 0.001;

        double newVal = slider.getValue() + (up ? step : -step);
        slider.setValue(juce::jlimit(min, max, newVal), juce::sendNotificationSync);
    };

    minusButton.onClick = [adjust] { adjust(false); };
    plusButton.onClick = [adjust] { adjust(true); };
}

