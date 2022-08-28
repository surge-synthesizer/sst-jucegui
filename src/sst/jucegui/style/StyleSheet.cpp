//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/style/StyleSheet.h>
#include <unordered_map>

#include <sst/jucegui/components/DraggableTextEditableValue.h>
#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/VSlider.h>
#include <sst/jucegui/components/HSlider.h>
#include <sst/jucegui/components/MultiSwitch.h>
#include <sst/jucegui/components/TabularizedTreeViewer.h>
#include <sst/jucegui/components/ToggleButton.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/components/Label.h>
#include <sst/jucegui/util/DebugHelpers.h>

namespace sst::jucegui::style
{

std::unordered_map<std::string, std::string> StyleSheet::inheritFromTo;

void StyleSheet::extendInheritanceMap(const StyleSheet::Class &from, const StyleSheet::Class &to)
{
    inheritFromTo[from.cname] = to.cname;
}

static std::unordered_map<StyleSheet::BuiltInTypes, StyleSheet::ptr_t> builtInSheets;

struct BuiltInDeleteStyleResetter;
static BuiltInDeleteStyleResetter *onDeleteResetter{nullptr};
struct BuiltInDeleteStyleResetter : juce::DeletedAtShutdown
{
    ~BuiltInDeleteStyleResetter()
    {
        builtInSheets.clear();
        onDeleteResetter = nullptr;
    }
};

struct StyleSheetBuiltInImpl : public StyleSheet
{
    StyleSheetBuiltInImpl() {}
    ~StyleSheetBuiltInImpl() { DBGMARK; }

    std::unordered_map<std::string, std::unordered_map<std::string, juce::Colour>> colours;
    std::unordered_map<std::string, std::unordered_map<std::string, juce::Font>> fonts;
    void setColour(const StyleSheet::Class &c, const StyleSheet::Property &p,
                   const juce::Colour &col) override
    {
        jassert(isValidPair(c, p));
        colours[c.cname][p.pname] = col;
    }
    void setFont(const StyleSheet::Class &c, const StyleSheet::Property &p,
                 const juce::Font &f) override
    {
        jassert(isValidPair(c, p));
        fonts[c.cname][p.pname] = f;
    }

    bool hasColour(const Class &c, const Property &p) const override
    {
        assert(p.type == Property::COLOUR);
        auto byC = colours.find(c.cname);
        if (byC != colours.end())
        {
            auto byP = byC->second.find(p.pname);
            if (byP != byC->second.end())
                return true;
        }
        return false;
    }

    juce::Colour getColour(const Class &c, const Property &p) const override
    {
        assert(p.type == Property::COLOUR);
        auto byC = colours.find(c.cname);
        if (byC != colours.end())
        {
            auto byP = byC->second.find(p.pname);
            if (byP != byC->second.end())
            {
                jassert(isValidPair(c, p));

                return byP->second;
            }
        }

        auto parC = inheritFromTo.find(c.cname);
        if (parC != inheritFromTo.end())
        {
            // FIXME gross still not right
            return getColour({parC->second.c_str()}, p);
        }
        jassertfalse;
        return juce::Colours::red;
    }

    bool hasFont(const Class &c, const Property &p) const override
    {
        assert(p.type == Property::FONT);
        auto byC = fonts.find(c.cname);
        if (byC != fonts.end())
        {
            auto byP = byC->second.find(p.pname);
            if (byP != byC->second.end())
                return true;
        }
        return false;
    }

    juce::Font getFont(const Class &c, const Property &p) const override
    {
        assert(p.type == Property::FONT);
        auto byC = fonts.find(c.cname);
        if (byC != fonts.end())
        {
            auto byP = byC->second.find(p.pname);
            if (byP != byC->second.end())
            {
                jassert(isValidPair(c, p));
                return byP->second;
            }
        }

        auto parC = inheritFromTo.find(c.cname);
        if (parC != inheritFromTo.end())
        {
            // FIXME gross still not right
            return getFont({parC->second.c_str()}, p);
        }
        jassertfalse;
        return juce::Font(36, juce::Font::italic);
    }
};

struct DarkSheet : public StyleSheetBuiltInImpl
{
    bool initialized{false};
    DarkSheet() {}
    void initialize()
    {
        if (initialized)
            return;

        initialized = true;
        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::backgroundgradstart, juce::Colour(90, 30, 0));
            setColour(w::styleClass, w::backgroundgradend, juce::Colours::black);
        }

        {
            using n = components::BaseStyles;
            setColour(n::styleClass, n::regionBG, juce::Colour(40, 40, 40));
            setColour(n::styleClass, n::regionBorder, juce::Colour(150, 140, 130));
            setColour(n::styleClass, n::regionLabelCol, juce::Colour(220, 220, 220));
            setFont(n::styleClass, n::regionLabelFont, juce::Font(14));
        }

        {
            using n = components::ControlStyles;
            setColour(n::styleClass, n::controlLabelCol, juce::Colours::white);
            setFont(n::styleClass, n::controlLabelFont, juce::Font(11));
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::labelrulecol, juce::Colour(170, 170, 170));
        }

        {
            using n = components::GraphicalControlStyles;
            setColour(n::styleClass, n::backgroundcol, juce::Colour(70, 70, 70));
            setColour(n::styleClass, n::guttercol, juce::Colour(50, 20, 00));
            setColour(n::styleClass, n::gutterhovcol, juce::Colour(80, 30, 00));
            setColour(n::styleClass, n::valcol, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::handlehovcol, juce::Colour(0xFF, 0xFF, 0xFF));
            setColour(n::styleClass, n::handlecol, juce::Colour(0xFF, 0xD0, 0xA0));
            setColour(n::styleClass, n::modhandlecol, juce::Colour(0xA0, 0xA0, 0xFF));
            setColour(n::styleClass, n::modhandlehovcol, juce::Colour(0xF0, 0xA0, 0xFF));
            setColour(n::styleClass, n::modvalcol, juce::Colour(0x00, 0xFF, 0x00));
            setColour(n::styleClass, n::modvalnegcol, juce::Colour(0x66, 0xBB, 0x66));
            setColour(n::styleClass, n::modactivecol, juce::Colour(0x33, 0x77, 0x33));
            setColour(n::styleClass, n::modothercol, juce::Colour(0x00, 0x55, 0x00));

            setColour(n::styleClass, n::labeltextcol, juce::Colour(220, 220, 220));
            setColour(n::styleClass, n::valuetextcol, juce::Colour(220, 180, 80));

            setFont(n::styleClass, n::labeltextfont, juce::Font(11));
            setFont(n::styleClass, n::valuetextfont, juce::Font(11));
        }

        {
            using n = components::Knob::Styles;
            setColour(n::styleClass, n::gradientcenter, juce::Colour(0x80, 0x50, 0x30));
        }

        {
            using n = components::VSlider::Styles;
        }

        {
            using n = components::HSlider::Styles;
        }

        {
            using n = components::TextualControlStyles;
            setColour(n::styleClass, n::bordercol, juce::Colour(70, 70, 70));
            setColour(n::styleClass, n::onbgcol, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::offbgcol, juce::Colour(50, 20, 0));
            setColour(n::styleClass, n::hoveronbgcol, juce::Colour(0xFF, 0xA0, 0x30));
            setColour(n::styleClass, n::hoveroffbgcol, juce::Colour(0x55, 0x22, 0x00));

            setColour(n::styleClass, n::textoncol, juce::Colour(0xFF, 0xFF, 0xFF));
            setColour(n::styleClass, n::textoffcol, juce::Colour(0xE0, 0xA0, 0x80));
            setColour(n::styleClass, n::texthoveroncol, juce::Colour(0xFF, 0xEE, 0xDD));
            setColour(n::styleClass, n::texthoveroffcol, juce::Colour(0xB0, 0xB0, 0xB0));

            setFont(n::styleClass, n::labelfont, juce::Font(11));
        }

        {
            using n = components::MultiSwitch::Styles;
        }

        {
            using n = components::TabularizedTreeViewer::Styles;

            setColour(n::styleClass, n::toggleboxcol, juce::Colour(190, 190, 190));
            setColour(n::styleClass, n::toggleglyphcol, juce::Colours::white);
            setColour(n::styleClass, n::toggleglyphhovercol, juce::Colour(0xFF, 90, 80));

            setColour(n::styleClass, n::connectorcol, juce::Colour(160, 160, 160));
        }
    }
};

struct LightSheet : public StyleSheetBuiltInImpl
{
    bool initialized{false};
    LightSheet() {}
    void initialize()
    {
        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::backgroundgradstart, juce::Colour(240, 240, 240));
            setColour(w::styleClass, w::backgroundgradend, juce::Colour(200, 200, 200));
        }

        {
            using n = components::BaseStyles;
            setColour(n::styleClass, n::regionBG, juce::Colours::white);
            setColour(n::styleClass, n::regionBorder, juce::Colour(160, 160, 160));
            setColour(n::styleClass, n::regionLabelCol, juce::Colours::black);
            setFont(n::styleClass, n::regionLabelFont, juce::Font(14));
        }

        {
            using n = components::ControlStyles;
            setColour(n::styleClass, n::controlLabelCol, juce::Colours::black);
            setFont(n::styleClass, n::controlLabelFont, juce::Font(11));
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::labelrulecol, juce::Colour(50, 50, 50));
        }

        {
            using n = components::GraphicalControlStyles;
            setColour(n::styleClass, n::backgroundcol, juce::Colour(240, 240, 240));
            setColour(n::styleClass, n::guttercol, juce::Colour(220, 220, 230));
            setColour(n::styleClass, n::gutterhovcol, juce::Colour(250, 250, 255));
            setColour(n::styleClass, n::valcol, juce::Colour(0x20, 0x20, 0x60));
            setColour(n::styleClass, n::handlecol, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::handlehovcol, juce::Colour(0xFF, 0xC0, 0x40));
            setColour(n::styleClass, n::modhandlecol, juce::Colour(0x00, 0x90, 0xF0));
            setColour(n::styleClass, n::modhandlehovcol, juce::Colour(0x50, 0xA0, 0xF0));
            setColour(n::styleClass, n::modvalcol, juce::Colour(0x33, 0xAA, 0x33));
            setColour(n::styleClass, n::modvalnegcol, juce::Colour(0x33, 0x66, 0x33));
            setColour(n::styleClass, n::modactivecol, juce::Colour(0x00, 0x77, 0x00));
            setColour(n::styleClass, n::modothercol, juce::Colour(0x44, 0x88, 0x44));

            setColour(n::styleClass, n::labeltextcol, juce::Colours::black);
            setColour(n::styleClass, n::valuetextcol, juce::Colour(0x50, 0x20, 0x00));
            setFont(n::styleClass, n::labeltextfont, juce::Font(11));
            setFont(n::styleClass, n::valuetextfont, juce::Font(11));
        }

        {
            using n = components::Knob::Styles;
            setColour(n::styleClass, n::gradientcenter, juce::Colour(0xFF, 0xCC, 0xAA));
        }

        {
            using n = components::VSlider::Styles;
            // bass class is fine
        }

        {
            using n = components::HSlider::Styles;
            // bass class is fine
        }

        {
            using n = components::TextualControlStyles;
            setColour(n::styleClass, n::bordercol, juce::Colour(160, 160, 160));
            setColour(n::styleClass, n::onbgcol, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::offbgcol, juce::Colour(0x60, 0x60, 0x60));
            setColour(n::styleClass, n::hoveronbgcol, juce::Colour(0xFF, 0xA0, 0x30));
            setColour(n::styleClass, n::hoveroffbgcol, juce::Colour(0x70, 0x70, 0x70));

            setColour(n::styleClass, n::textoncol, juce::Colour(0xFF, 0xFF, 0xFF));
            setColour(n::styleClass, n::textoffcol, juce::Colour(0xEE, 0xEE, 0xEE));
            setColour(n::styleClass, n::texthoveroncol, juce::Colour(0xFF, 0xEE, 0xDD));
            setColour(n::styleClass, n::texthoveroffcol, juce::Colour(0xFF, 0x90, 0x00));

            setFont(n::styleClass, n::labelfont, juce::Font(12));
        }

        {
            using n = components::ToggleButton::Styles;
        }
        {
            using n = components::MultiSwitch::Styles;
        }

        {
            using n = components::TabularizedTreeViewer::Styles;

            setColour(n::styleClass, n::toggleboxcol, juce::Colour(80, 80, 80));
            setColour(n::styleClass, n::toggleglyphcol, juce::Colours::black);
            setColour(n::styleClass, n::toggleglyphhovercol, juce::Colour(0xFF, 90, 80));

            setColour(n::styleClass, n::connectorcol, juce::Colour(160, 160, 160));
        }
    }
};

StyleSheet::ptr_t StyleSheet::getBuiltInStyleSheet(const BuiltInTypes &t)
{
    // This object is in the juce memory cleanup path so it will remove
    // the lingering global stylesheets we factory here
    if (!onDeleteResetter)
        onDeleteResetter = new BuiltInDeleteStyleResetter();

    auto f = builtInSheets.find(t);
    if (f != builtInSheets.end())
        return f->second;

    switch (t)
    {
    case DARK:
    {
        auto res = std::make_shared<DarkSheet>();
        res->initialize();
        builtInSheets[t] = res;
        return res;
    }
    case LIGHT:
    {
        auto res = std::make_shared<LightSheet>();
        res->initialize();
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

StyleSheet::Declaration StyleSheet::addClass(const sst::jucegui::style::StyleSheet::Class &c)
{
    auto d = Declaration(c);
    return d;
}

StyleSheet::Declaration &
StyleSheet::Declaration::withBaseClass(const sst::jucegui::style::StyleSheet::Class &base)
{
    extendInheritanceMap(of, base);
    return *this;
}

StyleSheet::Declaration &
StyleSheet::Declaration::withProperty(const sst::jucegui::style::StyleSheet::Property &p)
{
    validPairs.insert({of.cname, p.pname});
    return *this;
}

std::set<std::pair<std::string, std::string>> StyleSheet::validPairs;
bool StyleSheet::isValidPair(const sst::jucegui::style::StyleSheet::Class &c,
                             const sst::jucegui::style::StyleSheet::Property &p)
{
    auto res = validPairs.find({c.cname, p.pname}) != validPairs.end();

    if (!res)
    {
        if (inheritFromTo.find(c.cname) != inheritFromTo.end())
        {
            res = isValidPair({inheritFromTo[c.cname].c_str()}, p);
        }
    }
    if (!res)
    {
        DBGOUT("Invalid Pair Resolved " << DBGVAL(c.cname) << DBGVAL(p.pname));
    }
    return res;
}

void StyleSheet::initializeStyleSheets(std::function<void()> userClassInitializers)
{
    static bool initializedBase{false};
    if (!initializedBase)
    {
        namespace n = sst::jucegui::components;
        n::BaseStyles::initialize();
        n::ControlStyles::initialize();

        n::NamedPanel::Styles::initialize();
        n::Label::Styles::initialize();
        n::WindowPanel::Styles::initialize();

        n::GraphicalControlStyles::initialize();
        n::ContinuousParamEditor::Styles::initialize();
        n::VSlider::Styles::initialize();
        n::HSlider::Styles::initialize();
        n::Knob::Styles::initialize();

        n::TextualControlStyles::initialize();
        n::ToggleButton::Styles::initialize();
        n::MultiSwitch::Styles::initialize();
        n::DraggableTextEditableValue::Styles::initialize();

        n::TabularizedTreeViewer::Styles::initialize();
    }

    userClassInitializers();
}
} // namespace sst::jucegui::style
