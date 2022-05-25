//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/style/StyleSheet.h>
#include <unordered_map>

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>

namespace sst::jucegui::style
{
static std::unordered_map<StyleSheet::BuiltInTypes, StyleSheet::ptr_t> builtInSheets;

struct StyleSheetBuiltInImpl : public StyleSheet
{
    StyleSheetBuiltInImpl() {}

    std::unordered_map<std::string, std::unordered_map<std::string, juce::Colour>> colours;
    void setColour(const StyleSheet::Class &c, const StyleSheet::Property &p,
                   const juce::Colour &col)
    {
        colours[c.cname][p.pname] = col;
    }

    juce::Colour getColour(const Class &c, const Property &p) override
    {
        auto byC = colours.find(c.cname);
        if (byC != colours.end())
        {
            auto byP = byC->second.find(p.pname);
            if (byP != byC->second.end())
                return byP->second;
        }
        return juce::Colours::red;
    }
};

struct DarkSheet : public StyleSheetBuiltInImpl
{
    DarkSheet()
    {
        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::backgroundcol, juce::Colours::black);
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::backgroundcol, juce::Colour(40, 40, 40));
            setColour(n::styleClass, n::bordercol, juce::Colour(70, 70, 70));
            setColour(n::styleClass, n::labelcol, juce::Colour(220, 220, 220));
            setColour(n::styleClass, n::labelrulecol, juce::Colour(170, 170, 170));
        }
    }
};

struct LightSheet : public StyleSheetBuiltInImpl
{
    LightSheet()
    {
        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::backgroundcol, juce::Colour(225, 225, 225));
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::backgroundcol, juce::Colours::white);
            setColour(n::styleClass, n::bordercol, juce::Colour(160, 160, 160));
            setColour(n::styleClass, n::labelcol, juce::Colours::black);
            setColour(n::styleClass, n::labelrulecol, juce::Colour(50, 50, 50));
        }
    }
};

StyleSheet::ptr_t StyleSheet::getBuiltInStyleSheet(const BuiltInTypes &t)
{
    auto f = builtInSheets.find(t);
    if (f != builtInSheets.end())
        return f->second;

    switch (t)
    {
    case DARK:
    {
        auto res = std::make_shared<DarkSheet>();
        builtInSheets[t] = res;
        return res;
    }
    case LIGHT:
    {
        auto res = std::make_shared<LightSheet>();
        builtInSheets[t] = res;
        return res;
    }
    default:
    {
        auto res = std::make_shared<StyleSheetBuiltInImpl>();
        builtInSheets[t] = res;
        return res;
    }
    }
}
} // namespace sst::jucegui::style
