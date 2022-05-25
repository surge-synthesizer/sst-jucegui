//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/style/StyleSheet.h>
#include <unordered_map>

namespace sst::jucegui::style
{
static std::unordered_map<StyleSheet::BuiltInTypes, StyleSheet::ptr_t> builtInSheets;

struct StyleSheetBuiltInImpl : public StyleSheet
{
    juce::Colour getColour(StyleConsumer *c, const Property &p) override
    {
        return juce::Colours::yellow;
    }
};

StyleSheet::ptr_t StyleSheet::getBuiltInStyleSheet(const BuiltInTypes &t)
{
    auto f = builtInSheets.find(t);
    if (f != builtInSheets.end())
        return f->second;

    auto res = std::make_shared<StyleSheetBuiltInImpl>();
    builtInSheets[t] = res;
    return res;
}
} // namespace sst::jucegui::style
