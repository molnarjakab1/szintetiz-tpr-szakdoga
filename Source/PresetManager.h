#pragma once
#include <JuceHeader.h>

class PresetManager {
public:
    PresetManager(juce::AudioProcessorValueTreeState& s, juce::String appName = "TapSynth")
        : apvts(s), appName(std::move(appName))
    {
        auto d = dir();
        DBG("Preset folder: " << d.getFullPathName());
    }

    struct PresetInfo { juce::String name; juce::File file; };

    static juce::String normalisePresetName(juce::String name)
    {
       
        name = name.removeCharacters("\\/:*?\"<>|");
       
        if (name.endsWithIgnoreCase(".preset.xml")) name = name.dropLastCharacters(11);
        else if (name.endsWithIgnoreCase(".xml"))   name = name.dropLastCharacters(4);
        else if (name.endsWithIgnoreCase(".preset"))name = name.dropLastCharacters(7);
     
        return name.trim();
    }

    juce::Array<PresetInfo> list() const {
        juce::Array<PresetInfo> out;
        auto files = dir().findChildFiles(juce::File::findFiles, false, "*.preset.xml"); 
        files.sort();
        for (auto& f : files)
            out.add({ f.getFileNameWithoutExtension(), f }); 
        return out;
    }

    bool save(const juce::String& rawName) {
        const auto name = normalisePresetName(rawName);
        if (name.isEmpty()) return false;

        const juce::File target = dir().getChildFile(name + ".preset.xml");

        juce::ValueTree root("Preset");
        root.setProperty("name", name, nullptr);
        root.addChild(apvts.copyState(), -1, nullptr);

        if (auto xml = root.createXml())
            return target.replaceWithText(xml->toString());
        return false;
    }

    bool loadByName(const juce::String& rawName) {
        const auto name = normalisePresetName(rawName);
        const auto f = dir().getChildFile(name + ".preset.xml");
        return f.existsAsFile() && load(f);
    }

    bool load(const juce::File& f) {
        DBG("[PresetManager] load file: " + f.getFullPathName());
        std::unique_ptr<juce::XmlElement> xml(juce::parseXML(f));
        if (!xml) { DBG("  parseXML FAILED"); return false; }

        juce::ValueTree root = juce::ValueTree::fromXml(*xml);
        if (!root.hasType("Preset")) { DBG("  not a Preset"); return false; }

        auto params = root.getChild(0);
        if (!params.isValid()) { DBG("  no Params child"); return false; }

        apvts.replaceState(params);
        DBG("  replaceState OK");
        return true;
    }

    juce::File dir() const {
      
        juce::File base = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
        if (base == juce::File{})
            base = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile("TapSynthData");

        juce::String basePath = base.getFullPathName();
        if (!juce::File::isAbsolutePath(basePath))
            base = juce::File::getCurrentWorkingDirectory().getChildFile(basePath);

        juce::File folder = base.getChildFile(appName).getChildFile("Presets");
        folder.createDirectory();
        return folder;
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::String appName;

    void ensureDir() const { auto d = dir(); if (!d.exists()) d.createDirectory(); }

    static juce::var valueTreeToVar(const juce::ValueTree& vt) {
        auto* o = new juce::DynamicObject();
        o->setProperty("_type", vt.getType().toString());
        auto* props = new juce::DynamicObject();
        for (int i = 0; i < vt.getNumProperties(); ++i)
            props->setProperty(vt.getPropertyName(i).toString(), vt.getProperty(vt.getPropertyName(i)));
        o->setProperty("props", props);

        juce::Array<juce::var> kids;
        for (int i = 0; i < vt.getNumChildren(); ++i) kids.add(valueTreeToVar(vt.getChild(i)));
        o->setProperty("kids", kids);
        return o;
    }
    static juce::ValueTree varToValueTree(const juce::var& v) {
        auto* o = v.getDynamicObject();
        juce::ValueTree vt(juce::Identifier(o->getProperty("_type").toString()));
        auto* props = o->getProperty("props").getDynamicObject();
        if (props) for (auto& p : props->getProperties()) vt.setProperty(p.name, p.value, nullptr);
        auto kids = o->getProperty("kids");
        if (kids.isArray()) for (auto& kv : *kids.getArray()) vt.addChild(varToValueTree(kv), -1, nullptr);
        return vt;
    }
};