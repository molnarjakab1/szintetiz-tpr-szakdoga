
#include "PluginProcessor.h"
#include "PluginEditor.h"

TapSynthAudioProcessorEditor::TapSynthAudioProcessorEditor (TapSynthAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc (audioProcessor.apvts, "OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
, adsr ("Amplitudo burkologorbe", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filterAdsr ("Modulacias burkologorbe", audioProcessor.apvts, "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE")
, filter (audioProcessor.apvts, "FILTERTYPE", "FILTERFREQ", "FILTERRES")
{
    setSize (620, 500);
    addAndMakeVisible (osc);
    addAndMakeVisible (adsr);
    addAndMakeVisible (filterAdsr);
    addAndMakeVisible (filter);
}

TapSynthAudioProcessorEditor::~TapSynthAudioProcessorEditor()
{
}

void TapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(20,20,20));
}

void TapSynthAudioProcessorEditor::resized()
{
    const int margin = 10;
    const int panelWidth = (getWidth() - margin * 3) / 2;
    const int panelHeight = (getHeight() - margin * 3) / 2;

    osc.setBounds(margin, margin, panelWidth, panelHeight);
    adsr.setBounds(osc.getRight() + margin, margin, panelWidth, panelHeight);
    filterAdsr.setBounds(margin, osc.getBottom() + margin, panelWidth, panelHeight);
    filter.setBounds(filterAdsr.getRight() + margin, adsr.getBottom() + margin, panelWidth, panelHeight);

   /* const auto paddingX = 5;
    const auto paddingY = 35;
    const auto paddingY2 = 235;
    
    osc.setBounds (paddingX, paddingY, 300, 200);
    adsr.setBounds (osc.getRight(), paddingY, 300, 200);
    filterAdsr.setBounds (paddingX, paddingY2, 300, 200);
    filter.setBounds (filterAdsr.getRight(), paddingY2, 300, 200);*/
}


