#pragma once
#include <JuceHeader.h>
#include <functional>

class PresetSavePopup : public juce::Component
{
public:
    explicit PresetSavePopup(std::function<void(juce::String)> onSaveCb)
        : onSave(std::move(onSaveCb))
    {
        addAndMakeVisible(nameLabel);
        nameLabel.setText("Preset name:", juce::dontSendNotification);

        addAndMakeVisible(nameEditor);
        nameEditor.setText("");

        addAndMakeVisible(saveButton);
        saveButton.setButtonText("Save");
        saveButton.onClick = [this]
        {
            const auto name = nameEditor.getText().trim();
            if (onSave) onSave(name);

            if (auto* callout = findParentComponentOfClass<juce::CallOutBox>())
                callout->dismiss();
        };

        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("Cancel");
        cancelButton.onClick = [this]
        {
            if (auto* callout = findParentComponentOfClass<juce::CallOutBox>())
                callout->dismiss();
        };

        setSize(280, 100);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(10);
        nameLabel.setBounds(r.removeFromTop(18));
        nameEditor.setBounds(r.removeFromTop(24));
        auto row = r.removeFromTop(28);
        saveButton.setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        cancelButton.setBounds(row.removeFromLeft(100));
    }

private:
    std::function<void(juce::String)> onSave;

    juce::Label nameLabel;
    juce::TextEditor nameEditor;
    juce::TextButton saveButton, cancelButton;
};

