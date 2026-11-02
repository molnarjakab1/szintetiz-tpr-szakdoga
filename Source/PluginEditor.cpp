
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetManager.h"
#include "UI/PresetSavePopup.h"

TapSynthAudioProcessorEditor::TapSynthAudioProcessorEditor (TapSynthAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc (audioProcessor.apvts, "OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
, adsr ("Amplitudo burkologorbe", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filterAdsr ("Modulacias burkologorbe", audioProcessor.apvts, "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE")
, filter (audioProcessor.apvts, "FILTERTYPE", "FILTERFREQ", "FILTERRES"), fxRack(audioProcessor.apvts)
{
    setSize (1400, 600);

    addAndMakeVisible(presetBox);
    presetBox.onChange = [this] {onPresetChanged(); };

    addAndMakeVisible(saveButton);
    saveButton.onClick = [this] {onSaveClicked(); };

    addAndMakeVisible(quickSave);
    quickSave.onClick = [this]
    {
        const juce::String name = "QuickTest";
        const bool ok = audioProcessor.savePreset(name);
        if (ok)
        {
            refreshPresetList();
            int idx = findPresetIndexByName(name);
            if (idx >= 0)
                presetBox.setSelectedItemIndex(idx, juce::sendNotification);
        }
    };

  

    addAndMakeVisible (osc);
    addAndMakeVisible (adsr);
    addAndMakeVisible (filterAdsr);
    addAndMakeVisible (filter);
    addAndMakeVisible(fxRack);


  
    refreshPresetList();

    
 


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
    const int topBarH = 32;
    const int gap = margin;

    auto bounds = getLocalBounds().reduced(margin);

   
    auto topBar = bounds.removeFromTop(topBarH);
    presetBox.setBounds(topBar.removeFromLeft(240));
    topBar.removeFromLeft(8);
    saveButton.setBounds(topBar.removeFromLeft(100));
    topBar.removeFromLeft(8);
    quickSave.setBounds(topBar.removeFromLeft(140));

   
    int fxWidth = juce::jmax(480, bounds.getWidth() / 2);  

    auto rightFxArea = bounds.removeFromRight(fxWidth);
    bounds.removeFromRight(gap);

    auto gridArea = bounds;   

  
    const int colW = (gridArea.getWidth() - gap) / 2;
    const int rowH = (gridArea.getHeight() - gap) / 2;

    juce::Rectangle<int> leftTop(gridArea.getX(), gridArea.getY(), colW, rowH);
    juce::Rectangle<int> rightTop(leftTop.getRight() + gap, gridArea.getY(), colW, rowH);
    juce::Rectangle<int> leftBottom(gridArea.getX(), leftTop.getBottom() + gap, colW, rowH);
    juce::Rectangle<int> rightBottom(leftTop.getRight() + gap, leftTop.getBottom() + gap, colW, rowH);

    osc.setBounds(leftTop);
    adsr.setBounds(rightTop);
    filterAdsr.setBounds(leftBottom);
    filter.setBounds(rightBottom);

  
    fxRack.setBounds(rightFxArea.reduced(4));
}




void TapSynthAudioProcessorEditor::onPresetChanged() 
{
    const auto name = presetBox.getText();
    if (name.isNotEmpty())
    {
        const bool ok = audioProcessor.loadPresetByName(name);
        DBG("[PresetBox] load '" + name + "' -> " + juce::String(ok ? "OK" : "FAIL"));

        auto fmDepth = audioProcessor.apvts.getRawParameterValue("OSC1FMDEPTH")->load();
        auto cutoff = audioProcessor.apvts.getRawParameterValue("FILTERFREQ")->load();
        DBG("  OSC1FMDEPTH=" + juce::String(fmDepth) + "  FILTERFREQ=" + juce::String(cutoff));
    }
}

void TapSynthAudioProcessorEditor::onSaveClicked()
{
    auto popup = std::make_unique<PresetSavePopup>(
        [this](juce::String name)
    {
        if (name.isEmpty()) return;

    const bool ok = audioProcessor.savePreset(name);
    if (ok)
    {
        
        const auto clean = PresetManager::normalisePresetName(name);
        refreshPresetList();
        int idx = findPresetIndexByName(clean);
        if (idx >= 0)
            presetBox.setSelectedItemIndex(idx, juce::sendNotification); 
    }
    });

    juce::CallOutBox::launchAsynchronously(std::move(popup),
        saveButton.getScreenBounds(),
        nullptr);
}

void TapSynthAudioProcessorEditor::refreshPresetList()
{
    const juce::String prev = presetBox.getText();

    presetBox.clear(juce::dontSendNotification);

    auto list = audioProcessor.listPresets(); 
    int id = 1;
    for (auto& p : list)
        presetBox.addItem(p.name, id++); 

    if (presetBox.getNumItems() == 0)
    {
        presetBox.setText({}, juce::dontSendNotification);
        return;
    }

    int idx = findPresetIndexByName(prev);
    if (idx >= 0)
        presetBox.setSelectedItemIndex(idx, juce::sendNotification);
    else
        presetBox.setSelectedItemIndex(presetBox.getNumItems() - 1, juce::sendNotification);
}

int TapSynthAudioProcessorEditor::findPresetIndexByName(const juce::String& name) const
{
    for (int i = 0; i < presetBox.getNumItems(); ++i)
        if (presetBox.getItemText(i) == name)
            return i; 
    return -1;
}
