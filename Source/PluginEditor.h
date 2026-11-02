
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/FilterComponent.h"
#include "Data/AdsrData.h"
#include "PresetManager.h"
#include "UI/FxRackComponent.h"


class TapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TapSynthAudioProcessorEditor (TapSynthAudioProcessor&);
    ~TapSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    TapSynthAudioProcessor& audioProcessor;
    OscComponent osc;
    AdsrComponent adsr;
    AdsrComponent filterAdsr;
    FilterComponent filter;
    FxRackComponent fxRack;

    juce::ComboBox presetBox;
    juce::TextButton saveButton{ "Save..." };
    juce::TextButton quickSave{ "Quick Save (test)" };

    std::unique_ptr<juce::FileChooser> presetSaver;

    int findPresetIndexByName(const juce::String& name) const;



    void refreshPresetList();
    void onPresetChanged();
    void onSaveClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapSynthAudioProcessorEditor)
};
