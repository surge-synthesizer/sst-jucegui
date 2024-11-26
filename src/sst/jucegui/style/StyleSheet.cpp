/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-jucegui available at
 * https://github.com/surge-synthesizer/sst-jucegui
 */

#include "sst/jucegui/components/GlyphButton.h"
#include "sst/jucegui/components/GlyphPainter.h"
#include <sst/jucegui/style/StyleSheet.h>
#include <unordered_map>

#include <sst/jucegui/components/DraggableTextEditableValue.h>
#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/VSlider.h>
#include <sst/jucegui/components/HSlider.h>
#include <sst/jucegui/components/MultiSwitch.h>
#include <sst/jucegui/components/TabularizedTreeViewer.h>
#include <sst/jucegui/components/MenuButton.h>
#include <sst/jucegui/components/TextPushButton.h>
#include <sst/jucegui/components/ToggleButton.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/NamedPanelDivider.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/components/SevenSegmentControl.h>
#include <sst/jucegui/components/Label.h>
#include <sst/jucegui/components/RuledLabel.h>
#include <sst/jucegui/components/VUMeter.h>
#include <sst/jucegui/components/JogUpDownButton.h>
#include <sst/jucegui/components/ToolTip.h>
#include <sst/jucegui/components/ScrollBar.h>
#include <sst/jucegui/components/Viewport.h>
#include <sst/jucegui/components/TabbedComponent.h>
#include <sst/jucegui/components/TypeInOverlay.h>
#include <sst/jucegui/util/DebugHelpers.h>

#include <cassert>

namespace sst::jucegui::style
{

std::unordered_map<std::string, std::vector<std::string>> StyleSheet::inheritFromTo;
std::unordered_set<const StyleSheet::Class *> StyleSheet::allClasses;
std::unordered_map<const StyleSheet::Class *, std::vector<const StyleSheet::Property *>>
    StyleSheet::allProperties;
std::unordered_map<const StyleSheet::Property *, const StyleSheet::Class *>
    StyleSheet::classByProperty;
std::unordered_map<const StyleSheet::Class *, std::vector<const StyleSheet::Class *>>
    StyleSheet::inheritanceStructureDerivedFrom, StyleSheet::inheritanceStructureParentTo;

void StyleSheet::extendInheritanceMap(const StyleSheet::Class &from, const StyleSheet::Class &to)
{
    inheritFromTo[from.cname].push_back(to.cname);
    inheritanceStructureDerivedFrom[&from].push_back(&to);
    inheritanceStructureParentTo[&to].push_back(&from);
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
    ~StyleSheetBuiltInImpl() {}

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

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

        fonts[c.cname][p.pname] = f;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
    }

    void replaceFontsWithTypeface(const juce::Typeface::Ptr &p) override
    {
        for (auto &[cn, propFonts] : fonts)
        {
            for (auto &[pn, f] : propFonts)
            {
                auto nf = SST_JUCE_FONT_CTOR(p);
                nf.setHeight(f.getHeight());
                f = nf;
            }
        }
    }
    void replaceFontsWithFamily(const juce::String familyName) override { assert(false); }

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
        auto r = getColourOptional(c, p);
        if (r.has_value())
            return *r;

        std::cout << __FILE__ << ":" << __LINE__ << " COLOUR Missing : " << c.cname
                  << "::" << p.pname << std::endl;
        return juce::Colours::red;
    }

    std::optional<juce::Colour> getColourOptional(const Class &c, const Property &p) const override
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
            for (const auto &k : parC->second)
            {
                auto sub = getColourOptional({k.c_str()}, p);
                if (sub.has_value())
                    return sub;
            }
        }
        return std::nullopt;
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
        auto r = getFontOptional(c, p);
        if (r.has_value())
            return *r;

        std::cout << __FILE__ << ":" << __LINE__ << " FONT Missing : " << c.cname << "::" << p.pname
                  << std::endl;
        return SST_JUCE_FONT_CTOR(36, juce::Font::italic);
    }
    std::optional<juce::Font> getFontOptional(const Class &c, const Property &p) const override
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
            for (const auto &k : parC->second)
            {
                auto q = getFontOptional({k.c_str()}, p);
                if (q.has_value())
                    return *q;
            }
        }

        return std::nullopt;
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
            using n = components::base_styles::Base;
            setColour(n::styleClass, n::background, juce::Colour(0x25, 0x25, 0x28));
        }

        {
            using n = components::base_styles::SelectableRegion;
            setColour(n::styleClass, n::backgroundSelected, juce::Colour(0x45, 0x45, 0x48));
        }

        {
            using n = components::base_styles::Outlined;
            setColour(n::styleClass, n::outline, juce::Colour(0x50, 0x50, 0x50));
            setColour(n::styleClass, n::brightoutline, juce::Colour(0x70, 0x70, 0x70));
        }

        {
            using n = components::base_styles::BaseLabel;
            setColour(n::styleClass, n::labelcolor, juce::Colour(220, 220, 220));
            setColour(n::styleClass, n::labelcolor_hover, juce::Colour(240, 240, 235));
            setFont(n::styleClass, n::labelfont, SST_JUCE_FONT_CTOR(13));
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::labelrule, juce::Colour(0x70, 0x70, 0x70));
            setColour(n::styleClass, n::selectedtab, juce::Colour(0xFF, 0x90, 00));
            setColour(n::styleClass, n::accentedPanel, juce::Colour(0xFF, 0x90, 00));
        }

        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::bgstart, juce::Colour(0x3B, 0x3D, 0x40));
            setColour(w::styleClass, w::bgend, juce::Colour(0x1B, 0x1D, 0x20));
        }

        {
            using n = components::base_styles::PushButton;
            setColour(n::styleClass, n::fill, juce::Colour(0x60, 0x60, 0x60));
            setColour(n::styleClass, n::fill_hover, juce::Colour(0x90, 0x85, 0x83));
            setColour(n::styleClass, n::fill_pressed, juce::Colour(0x80, 0x80, 0x80));
        }

        {
            using n = components::MenuButton::Styles;
            setColour(n::styleClass, n::menuarrow_hover, juce::Colour(0xFF, 0x90, 0x00));
        }

        {
            using n = components::JogUpDownButton::Styles;
            setColour(n::styleClass, n::jogbutton_hover, juce::Colour(0xFF, 0x90, 0x00));
        }

        {
            using n = components::base_styles::ValueBearing;
            setColour(n::styleClass, n::value, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::value_hover, juce::Colour(0xFF, 0xA0, 0x30));
            setColour(n::styleClass, n::valuelabel, juce::Colour(0x20, 0x10, 0x20));
            setColour(n::styleClass, n::valuelabel_hover, juce::Colour(0x30, 0x20, 0x10));
        }

        {
            using n = components::MultiSwitch::Styles;
            setColour(n::styleClass, n::unselected_hover, juce::Colour(0x50, 0x50, 0x50));
            setColour(n::styleClass, n::valuebg, juce::Colour(0x30, 0x20, 0x00));
        }

        {
            using n = components::base_styles::ValueGutter;
            setColour(n::styleClass, n::gutter, juce::Colour(0x05, 0x05, 0x00));
            setColour(n::styleClass, n::gutter_hover, juce::Colour(0x40, 0x25, 0x00));
        }

        {
            using n = components::base_styles::GraphicalHandle;
            setColour(n::styleClass, n::handle, juce::Colour(0xD0, 0xD0, 0xD0));
            setColour(n::styleClass, n::handle_outline, juce::Colour(0x0F, 0x09, 0x00));
            setColour(n::styleClass, n::handle_hover, juce::Colour(0xFF, 0xE0, 0xC0));

            setColour(n::styleClass, n::modulation_handle, juce::Colour(0xA0, 0xF0, 0xA0));
            setColour(n::styleClass, n::modulation_handle_hover, juce::Colour(0xB0, 0xFF, 0xB0));
        }

        {
            using n = components::base_styles::ModulationValueBearing;
            setColour(n::styleClass, n::modulated_by_other, juce::Colour(0x20, 0x40, 0x20));
            setColour(n::styleClass, n::modulated_by_selected, juce::Colour(0x30, 0x50, 0x30));
            setColour(n::styleClass, n::modulation_value, juce::Colour(0x20, 0xA0, 0x20));
            setColour(n::styleClass, n::modulation_opposite_value, juce::Colour(0x20, 0x80, 0x20));
            setColour(n::styleClass, n::modulation_value_hover, juce::Colour(0x40, 0xA0, 0x40));
            setColour(n::styleClass, n::modulation_opposite_value_hover,
                      juce::Colour(0x40, 0x80, 0x40));
        }

        {
            using n = components::Knob::Styles;
            setColour(n::styleClass, n::knobbase, juce::Colour(82, 82, 82));
        }

        {
            using n = components::VUMeter::Styles;
            setColour(n::styleClass, n::vu_gutter, juce::Colour(0, 0, 0));
            setColour(n::styleClass, n::vu_gradstart, juce::Colour(200, 200, 100));
            setColour(n::styleClass, n::vu_gradend, juce::Colour(100, 100, 220));
            setColour(n::styleClass, n::vu_overload, juce::Colour(200, 50, 50));
        }

        {
            using n = components::TabbedComponent::Styles;
            setColour(n::styleClass, n::tabSelectedLabelColor, juce::Colour(0xFF, 0x90, 0x00));
            setColour(n::styleClass, n::tabUnselectedLabelColor, juce::Colour(0xAF, 0xA0, 0xA0));
            setColour(n::styleClass, n::tabSelectedFillColor, juce::Colour(0x0A, 0x0A, 0x0A));
            setColour(n::styleClass, n::tabUnselectedOutlineColor, juce::Colour(0xA0, 0xA0, 0xA0));
        }

        {
            using n = components::TabularizedTreeViewer::Styles;

            setColour(n::styleClass, n::toggleboxcol, juce::Colour(190, 190, 190));
            setColour(n::styleClass, n::toggleglyphcol, juce::Colours::white);
            setColour(n::styleClass, n::toggleglyphhovercol, juce::Colour(0xFF, 90, 80));

            setColour(n::styleClass, n::connectorcol, juce::Colour(160, 160, 160));
        }

        {
            using n = components::ToolTip::Styles;
            setFont(n::styleClass, n::datafont,
                    getFont(components::base_styles::BaseLabel::styleClass,
                            components::base_styles::BaseLabel::labelfont));
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
            using n = components::base_styles::Base;
            setColour(n::styleClass, n::background, juce::Colour(0xF0, 0xF0, 0xF0));
        }

        {
            using n = components::base_styles::SelectableRegion;
            setColour(n::styleClass, n::backgroundSelected, juce::Colour(0xD0, 0xD0, 0xD0));
        }

        {
            using n = components::base_styles::Outlined;
            setColour(n::styleClass, n::outline, juce::Colour(0xA0, 0xA0, 0xA0));
            setColour(n::styleClass, n::brightoutline, juce::Colour(0x70, 0x70, 0x70));
        }

        {
            using n = components::base_styles::BaseLabel;
            setColour(n::styleClass, n::labelcolor, juce::Colour(30, 30, 30));
            setColour(n::styleClass, n::labelcolor_hover, juce::Colour(30, 30, 40));
            setFont(n::styleClass, n::labelfont, SST_JUCE_FONT_CTOR(13));
        }

        {
            using n = components::NamedPanel::Styles;
            setColour(n::styleClass, n::labelrule, juce::Colour(50, 50, 50));
            setColour(n::styleClass, n::selectedtab, juce::Colour(0x00, 0x00, 50));
            setColour(n::styleClass, n::accentedPanel, juce::Colour(0x00, 0x00, 50));
        }

        {
            using w = components::WindowPanel::Styles;
            setColour(w::styleClass, w::bgstart, juce::Colour(220, 220, 220));
            setColour(w::styleClass, w::bgend, juce::Colour(200, 200, 200));
        }

        {
            using n = components::base_styles::PushButton;
            setColour(n::styleClass, n::fill, juce::Colour(0xB0, 0xB0, 0xC0));
            setColour(n::styleClass, n::fill_hover, juce::Colour(0xC0, 0xC0, 0xD0));
            setColour(n::styleClass, n::fill_pressed, juce::Colour(0xB0, 0xB0, 0xD0));
        }

        {
            using n = components::MenuButton::Styles;
            setColour(n::styleClass, n::menuarrow_hover, juce::Colour(0x20, 0x20, 0xD0));
        }

        {
            using n = components::JogUpDownButton::Styles;
            setColour(n::styleClass, n::jogbutton_hover, juce::Colour(0x20, 0x20, 0xD0));
        }

        {
            using n = components::base_styles::ValueBearing;
            setColour(n::styleClass, n::value, juce::Colour(0x30, 0x30, 0xA0));
            setColour(n::styleClass, n::value_hover, juce::Colour(0x50, 0x50, 0xC0));
            setColour(n::styleClass, n::valuelabel, juce::Colour(0xE0, 0xE0, 0xF0));
            setColour(n::styleClass, n::valuelabel_hover, juce::Colour(0xF0, 0xF0, 0xFF));
        }

        {
            using n = components::MultiSwitch::Styles;
            setColour(n::styleClass, n::unselected_hover, juce::Colour(0xD0, 0xD0, 0xE0));
        }

        {
            using n = components::base_styles::ValueGutter;
            setColour(n::styleClass, n::gutter, juce::Colour(0xB5, 0xB5, 0xD5));
            setColour(n::styleClass, n::gutter_hover, juce::Colour(0xC5, 0xC5, 0xE0));
        }

        {
            using n = components::base_styles::GraphicalHandle;
            setColour(n::styleClass, n::handle, juce::Colour(0x30, 0x30, 0x60));
            setColour(n::styleClass, n::handle_outline, juce::Colour(0xA0, 0xA9, 0xFF));
            setColour(n::styleClass, n::handle_hover, juce::Colour(0x40, 0x40, 0x80));

            setColour(n::styleClass, n::modulation_handle, juce::Colour(0xA0, 0xF0, 0xA0));
            setColour(n::styleClass, n::modulation_handle_hover, juce::Colour(0xB0, 0xFF, 0xB0));
        }

        {
            using n = components::base_styles::ModulationValueBearing;
            setColour(n::styleClass, n::modulated_by_other, juce::Colour(0xA5, 0xC5, 0xA5));
            setColour(n::styleClass, n::modulated_by_selected, juce::Colour(0xA4, 0xE5, 0xA5));
            setColour(n::styleClass, n::modulation_value, juce::Colour(0x20, 0xA0, 0x20));
            setColour(n::styleClass, n::modulation_opposite_value, juce::Colour(0x20, 0x80, 0x20));
            setColour(n::styleClass, n::modulation_value_hover, juce::Colour(0x40, 0xA0, 0x40));
            setColour(n::styleClass, n::modulation_opposite_value_hover,
                      juce::Colour(0x40, 0x80, 0x40));
        }

        {
            using n = components::VUMeter::Styles;
            setColour(n::styleClass, n::vu_gutter, juce::Colour(0xE5, 0xE5, 0xF5));
            setColour(n::styleClass, n::vu_gradstart, juce::Colour(210, 210, 80));
            setColour(n::styleClass, n::vu_gradend, juce::Colour(90, 90, 170));
            setColour(n::styleClass, n::vu_overload, juce::Colour(200, 50, 50));
        }

        {
            using n = components::Knob::Styles;
            setColour(n::styleClass, n::knobbase, juce::Colour(194, 194, 194));
        }

        {
            using n = components::TabularizedTreeViewer::Styles;

            setColour(n::styleClass, n::toggleboxcol, juce::Colour(80, 80, 80));
            setColour(n::styleClass, n::toggleglyphcol, juce::Colours::black);
            setColour(n::styleClass, n::toggleglyphhovercol, juce::Colour(0xFF, 90, 80));

            setColour(n::styleClass, n::connectorcol, juce::Colour(160, 160, 160));
        }

        {
            using n = components::ToolTip::Styles;
            setFont(n::styleClass, n::datafont,
                    getFont(components::base_styles::BaseLabel::styleClass,
                            components::base_styles::BaseLabel::labelfont));
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
    case EMPTY:
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
    allClasses.insert(&c);
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
    allProperties[&of].push_back(&p);
    classByProperty[&p] = &of;
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
            for (const auto &k : inheritFromTo[c.cname])
            {
                auto ivp = isValidPair({k.c_str()}, p);
                if (ivp)
                    return true;
            }
        }
    }

    return res;
}

void StyleSheet::initializeStyleSheets(std::function<void()> userClassInitializers)
{
    static bool initializedBase{false};
    if (!initializedBase)
    {
        namespace n = sst::jucegui::components;

        n::base_styles::initialize();

        n::NamedPanel::Styles::initialize();
        n::NamedPanelDivider::Styles::initialize();
        n::Label::Styles::initialize();
        n::RuledLabel::Styles::initialize();
        n::GlyphPainter::Styles::initialize();
        n::WindowPanel::Styles::initialize();

        n::ContinuousParamEditor::Styles::initialize();
        n::VSlider::Styles::initialize();
        n::HSlider::Styles::initialize();
        n::Knob::Styles::initialize();

        n::ToggleButton::Styles::initialize();
        n::MenuButton::Styles::initialize();
        n::TextPushButton::Styles::initialize();
        n::GlyphButton::Styles::initialize();
        n::MultiSwitch::Styles::initialize();
        n::JogUpDownButton::Styles::initialize();
        n::DraggableTextEditableValue::Styles::initialize();
        n::SevenSegmentControl::Styles::initialize();
        n::VUMeter::Styles::initialize();
        n::ToolTip::Styles::initialize();
        n::ScrollBar::Styles::initialize();
        n::Viewport::Styles::initialize();
        n::TabbedComponent::Styles::initialize();
        n::TypeInOverlay::Styles::initialize();

        n::TabularizedTreeViewer::Styles::initialize();
    }

    userClassInitializers();
}

std::ostream &StyleSheet::dumpStyleSheetTo(std::ostream &os)
{
    os << "StyleSheet Dump"
       << "\n";

    std::function<void(const Class *)> dlist;
    dlist = [&](auto c) {
        auto ih = inheritanceStructureDerivedFrom.find(c);
        if (ih != inheritanceStructureDerivedFrom.end())
        {
            for (auto *q : ih->second)
            {
                os << q->cname << " ";
            }
            os << " / ";
            for (auto *q : ih->second)
            {
                dlist(q);
            }
        }
    };

    for (const auto &[a, b] : allProperties)
    {
        os << "--- " << a->cname << " : ";
        dlist(a);
        os << "\n";
        for (const auto &p : b)
        {
            os << "    |-- " << p->pname << " ";
            switch (p->type)
            {
            case Property::COLOUR:
            {
                auto cc = getColourOptional(*a, *p);
                if (cc.has_value())
                    os << " (color=" << cc->toString() << ")";
                else
                    os << " (color missing)";
            }
            break;
            case Property::FONT:
            {
                auto ff = getFontOptional(*a, *p);

                if (ff.has_value())
                    os << " (font=" << ff->toString() << ")";
                else
                    os << " (font missing)";
                break;
            }
            }
            os << "\n";
        }
    }
    os << "\n";

    std::unordered_set<const Class *> rootClasses;
    for (auto c : allClasses)
    {
        auto ifi = inheritanceStructureDerivedFrom.find(c);
        if (ifi == inheritanceStructureDerivedFrom.end() || ifi->second.empty())
        {
            rootClasses.insert(c);
        }
    }

    std::function<void(const Class *, const std::string &)> rprint;
    rprint = [&](auto a, auto b) {
        os << b << " " << a->cname << "\n";
        auto ifi = inheritanceStructureParentTo.find(a);
        if (ifi != inheritanceStructureParentTo.end())
        {
            for (auto q : ifi->second)
            {
                rprint(q, b + "---|");
            }
        }
    };
    for (auto c : rootClasses)
    {
        rprint(c, "|");
    }

    os << std::flush;
    return os;
}
} // namespace sst::jucegui::style
