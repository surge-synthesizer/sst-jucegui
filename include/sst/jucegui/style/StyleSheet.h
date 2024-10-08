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

#ifndef INCLUDE_SST_JUCEGUI_STYLE_STYLESHEET_H
#define INCLUDE_SST_JUCEGUI_STYLE_STYLESHEET_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cassert>

#if JUCE_VERSION >= 0x080000
#define SST_JUCE_FONT_CTOR(...) juce::Font(juce::FontOptions(__VA_ARGS__))
#else
#define SST_JUCE_FONT_CTOR(...) juce::Font(__VA_ARGS__)
#endif

// Yes, a non-compatible point release! Fun eh?
#if JUCE_VERSION >= 0x080002
#define SST_STRING_WIDTH_INT(a, b) juce::GlyphArrangement::getStringWidthInt(a, b)
#define SST_STRING_WIDTH_FLOAT(a, b) juce::GlyphArrangement::getStringWidth(a, b)
#else
#define SST_STRING_WIDTH_INT(a, b) a.getStringWidth(b)
#define SST_STRING_WIDTH_FLOAT(a, b) a.getStringWidthFloat(b)
#endif

namespace sst::jucegui::style
{
struct StyleConsumer;

/**
 * A StyleSheet is the class which allows us to get fonts, colors, and other properties
 * which we apply to widgets at render time. It relies on a few core features
 *
 * The StyleSheet object itself can answer questions like hasColor/getColor and
 * hasFont/getFont for a Class/Property pair
 *
 * The StyleSheet has an internal class (called "StyleSheet::Class") which
 * is the class a widget uses in the style sheet. A Class can advertise inheritance
 * from another class using `StyleSheet::extendInheritanceMap`. Inheritance relationships
 * are program wide and class level not instance level.
 *
 * The StyleSheet has a second internal class ('StyleSheet::Property') which
 * is the class used to query a StyleSheet Class for a color or font.
 *
 * A typical pattern for a widget is to implement an inner struct `Styles`
 * which contains constexpr declarations of the base class and associated
 * properties for subsequenty queries. Look at the ToggleButton.h header
 * for a worked example.
 *
 * Finally we have a pair of built in style sheets (light and dark) which set
 * up default colors and fonts for the base class widgets.
 */
struct StyleSheet
{
    StyleSheet() {}
    virtual ~StyleSheet() = default;

    static void initializeStyleSheets(std::function<void()> userClassInitializers);

    struct Class
    {
        static constexpr int nameLength = 256;
        char cname[nameLength]{0};

        static constexpr int maxProps = 4096;
        int nProps{0};

        constexpr Class(const char *s)
        {
            for (int i = 0; i < nameLength - 1; ++i)
            {
                cname[i] = s[i];
                if (cname[i] == 0)
                    break;
            }
            cname[nameLength - 1] = 0;
        }

        void copyFrom(const Class &other) { strncpy(cname, other.cname, nameLength); }
        Class(const Class &other) = delete;
        Class &operator=(const Class &) = delete;
        Class &operator=(Class &&) = delete;
    };

    struct Property
    {
        static constexpr int nameLength = 256;
        char pname[nameLength]{0};

        enum Type
        {
            COLOUR,
            FONT
        } type;
        constexpr Property(const char *s, Type t = COLOUR) : type(t)
        {
            for (int i = 0; i < nameLength - 1; ++i)
            {
                pname[i] = s[i];
                if (pname[i] == 0)
                    break;
            }
            pname[nameLength - 1] = 0;
        }

        Property(const Property &other) = delete;
        Property &operator=(const Property &) = delete;
        Property &operator=(Property &&) = delete;
    };

    struct Declaration
    {
        const Class &of;
        Declaration(const Class &o) : of(o) {}
        Declaration &withProperty(const Property &p);
        Declaration &withBaseClass(const Class &c);
    };
    static Declaration addClass(const Class &c);

    // These are weak references but the things they point to are constexpr long lived
    static std::unordered_set<const Class *> allClasses;
    static std::unordered_map<const Class *, std::vector<const Property *>> allProperties;
    static std::unordered_map<const StyleSheet::Property *, const StyleSheet::Class *>
        classByProperty;

    static std::unordered_map<std::string, std::vector<std::string>> inheritFromTo;
    static std::unordered_map<const Class *, std::vector<const Class *>>
        inheritanceStructureDerivedFrom;
    static std::unordered_map<const Class *, std::vector<const Class *>>
        inheritanceStructureParentTo;
    static bool isValidPair(const Class &c, const Property &p);

    virtual bool hasColour(const Class &c, const Property &p) const = 0;
    virtual juce::Colour getColour(const Class &c, const Property &p) const = 0;
    virtual std::optional<juce::Colour> getColourOptional(const Class &c,
                                                          const Property &p) const = 0;
    virtual void setColour(const Class &c, const Property &p, const juce::Colour &) = 0;

    virtual bool hasFont(const Class &c, const Property &p) const = 0;
    virtual juce::Font getFont(const Class &c, const Property &p) const = 0;
    virtual std::optional<juce::Font> getFontOptional(const Class &c, const Property &p) const = 0;
    virtual void setFont(const Class &c, const Property &p, const juce::Font &) = 0;

    virtual void replaceFontsWithTypeface(const juce::Typeface::Ptr &p) = 0;
    virtual void replaceFontsWithFamily(const juce::String familyName) = 0;

    typedef std::shared_ptr<StyleSheet> ptr_t;

    enum BuiltInTypes
    {
        DARK,
        LIGHT,
        EMPTY
    };
    static ptr_t getBuiltInStyleSheet(const BuiltInTypes &t);

    friend struct StyleConsumer;
    friend struct Declaration;

    std::ostream &dumpStyleSheetTo(std::ostream &os);

  private:
    static void extendInheritanceMap(const StyleSheet::Class &from, const StyleSheet::Class &to);
    static std::set<std::pair<std::string, std::string>> validPairs;

  public:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleSheet)
};

} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLESHEET_H
