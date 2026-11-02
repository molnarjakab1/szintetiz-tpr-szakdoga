
#include <JuceHeader.h>
#include "AdsrComponent.h"

AdsrComponent::AdsrComponent(juce::String name,
    juce::AudioProcessorValueTreeState& apvts,
    juce::String attackId,
    juce::String decayId,
    juce::String sustainId,
    juce::String releaseId)
{
    componentName = name;

    setSliderWithLabel(attackSlider, attackLabel, apvts, attackId, attackAttachment, attackMinus, attackPlus);
    setSliderWithLabel(decaySlider, decayLabel, apvts, decayId, decayAttachment, decayMinus, decayPlus);
    setSliderWithLabel(sustainSlider, sustainLabel, apvts, sustainId, sustainAttachment, sustainMinus, sustainPlus);
    setSliderWithLabel(releaseSlider, releaseLabel, apvts, releaseId, releaseAttachment, releaseMinus, releasePlus);
}

AdsrComponent::~AdsrComponent() {}

void AdsrComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    auto labelSpace = bounds.removeFromTop(25.0f);

    juce::ColourGradient gradient(juce::Colour::fromRGB(20, 20, 30),
        0, 0,
        juce::Colour::fromRGB(10, 40, 60),
        0, (float)getHeight(), false);
    g.setGradientFill(gradient);
    g.setFont(20.0f);
    g.drawText(componentName, labelSpace.withX(5), juce::Justification::left);
    g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
    g.setColour(juce::Colours::aqua);
    g.drawRoundedRectangle(bounds.toFloat(), 10.0f, 2.0f);
}

void AdsrComponent::resized()
{
    const auto bounds = getLocalBounds().reduced(10);
    const auto padding = 10;
    const auto sliderWidth = bounds.getWidth() / 4 - padding;
    const auto sliderHeight = bounds.getHeight() - 65;   // a bit more room for +/- buttons
    const auto sliderStartX = padding + 5;
    const auto sliderStartY = 55;
    const auto labelYOffset = 20;
    const auto labelHeight = 20;
    const auto labelStart = sliderStartY - labelYOffset;

    auto placeColumn = [labelHeight](juce::Slider& slider,
        juce::Label& label,
        juce::TextButton& minusBtn,
        juce::TextButton& plusBtn,
        int x, int sliderY, int sliderW, int sliderH)
    {
        slider.setBounds(x, sliderY, sliderW, sliderH);
        label.setBounds(x, sliderY - labelHeight, sliderW, labelHeight);

        const int btnHeight = 16;
        int btnY = slider.getBottom() + 2;
        int halfW = sliderW / 2 - 2;

        minusBtn.setBounds(x, btnY, halfW, btnHeight);
        plusBtn.setBounds(x + halfW + 4, btnY, halfW, btnHeight);
    };

    placeColumn(attackSlider, attackLabel, attackMinus, attackPlus,
        sliderStartX, sliderStartY, sliderWidth, sliderHeight);

    placeColumn(decaySlider, decayLabel, decayMinus, decayPlus,
        attackSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);

    placeColumn(sustainSlider, sustainLabel, sustainMinus, sustainPlus,
        decaySlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);

    placeColumn(releaseSlider, releaseLabel, releaseMinus, releasePlus,
        sustainSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);
}

void AdsrComponent::setSliderWithLabel(juce::Slider& slider,
    juce::Label& label,
    juce::AudioProcessorValueTreeState& apvts,
    juce::String paramId,
    std::unique_ptr<Attachment>& attachment,
    juce::TextButton& minusButton,
    juce::TextButton& plusButton)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
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

    // +/- buttons
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

        // Interval from parameter/slider (0 if continuous)
        double baseStep = slider.getInterval();

        // Fallback for continuous: 1% of range
        double fineStep = (max - min) * 0.01;
        if (fineStep <= 0.0)
            fineStep = 0.001;

        // Use whichever is larger so we always cross at least one quantisation step
        double step = baseStep > 0.0 ? std::max(baseStep, fineStep) : fineStep;

        double newVal = slider.getValue() + (up ? step : -step);
        slider.setValue(juce::jlimit(min, max, newVal), juce::sendNotificationSync);
    };

    minusButton.onClick = [adjust] { adjust(false); };
    plusButton.onClick = [adjust] { adjust(true); };
}
