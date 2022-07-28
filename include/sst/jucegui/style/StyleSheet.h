/*
 * sst-jucegui - a set of widgets for ShortCircuit, SurgeXT2, and others
 * Copyright 2022 to authors per the github transaction log
 *
 * Released under the MIT license. See 'LICENSE.md' for details.
 */

#ifndef SST_JUCEGUI_STYLESHEET_H
#define SST_JUCEGUI_STYLESHEET_H

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>

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

    struct Class
    {
        static constexpr int nameLength = 256;
        char cname[nameLength]{0};
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
        Class(const Class &other) { strncpy(cname, other.cname, nameLength); }
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
    };

    virtual bool hasColour(const Class &c, const Property &p) const = 0;
    virtual juce::Colour getColour(const Class &c, const Property &p) const = 0;

    virtual bool hasFont(const Class &c, const Property &p) const = 0;
    virtual juce::Font getFont(const Class &c, const Property &p) const = 0;

    typedef std::shared_ptr<StyleSheet> ptr_t;

    enum BuiltInTypes
    {
        DARK,
        LIGHT
    };
    static ptr_t getBuiltInStyleSheet(const BuiltInTypes &t);

    friend struct StyleConsumer;
    static void extendInheritanceMap(const StyleSheet::Class &from, const StyleSheet::Class &to);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleSheet);

  protected:
    static std::unordered_map<std::string, std::string> inheritFromTo;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLESHEET_H
