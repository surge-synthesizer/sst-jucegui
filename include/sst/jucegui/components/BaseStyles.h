//
// Created by Paul Walker on 8/7/22.
//

#ifndef SST_JUCEGUI_BASESTYLES_H
#define SST_JUCEGUI_BASESTYLES_H

#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct BaseStyles
{
    using sclass = style::StyleSheet::Class;
    using sprop = style::StyleSheet::Property;
    static constexpr sclass styleClass{"base"};

    static constexpr sprop regionBorder{"region.border.color"};
    static constexpr sprop regionBG{"region.background.color"};
    static constexpr sprop regionLabelCol{"region.label.color"};
    static constexpr sprop regionLabelFont{"region.label.font", style::StyleSheet::Property::FONT};

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withProperty(regionBorder)
            .withProperty(regionBG)
            .withProperty(regionLabelCol)
            .withProperty(regionLabelFont);
    }
};

struct ControlStyles : BaseStyles
{
    using sclass = style::StyleSheet::Class;
    using sprop = style::StyleSheet::Property;
    static constexpr sclass styleClass{"controls"};

    static constexpr sprop controlLabelCol{"control.label.color"};
    static constexpr sprop controlLabelFont{"control.label.font",
                                            style::StyleSheet::Property::FONT};

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withBaseClass(BaseStyles::styleClass)
            .withProperty(controlLabelCol)
            .withProperty(controlLabelFont);
    }
};

// Knobs, Sliders, etc...
struct GraphicalControlStyles : ControlStyles
{
    using sclass = style::StyleSheet::Class;
    using sprop = style::StyleSheet::Property;
    static constexpr sclass styleClass{"graphical.controls"};

    static constexpr sprop backgroundcol{"background.color"};
    static constexpr sprop valcol{"value.color"};
    static constexpr sprop handlecol{"handle.color"};
    static constexpr sprop guttercol{"gutter.color"};
    static constexpr sprop modvalcol{"modulationvalue.color"};
    static constexpr sprop modvalnegcol{"modulationnegativevalue.color"};
    static constexpr sprop modactivecol{"modulationactive.color"};
    static constexpr sprop modothercol{"modulationother.color"};
    static constexpr sprop modhandlecol{"modhandle.color"};

    static constexpr sprop labeltextcol{"labeltext.color"};
    static constexpr sprop valuetextcol{"valuetext.color"};

    static constexpr sprop labeltextfont{"labeltext.font", sprop::FONT};
    static constexpr sprop valuetextfont{"valuetext.font", sprop::FONT};

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withBaseClass(ControlStyles::styleClass)
            .withProperty(backgroundcol)
            .withProperty(valcol)
            .withProperty(handlecol)
            .withProperty(guttercol)
            .withProperty(modvalcol)
            .withProperty(modactivecol)
            .withProperty(modvalnegcol)
            .withProperty(modothercol)
            .withProperty(modhandlecol)
            .withProperty(labeltextfont)
            .withProperty(labeltextcol)
            .withProperty(valuetextfont)
            .withProperty(valuetextcol);
    }
};

// Switches, Buttons, Draggable Typeins, Combo Boxes, etc...
struct TextualControlStyles : ControlStyles
{
    using sclass = style::StyleSheet::Class;
    using sprop = style::StyleSheet::Property;
    static constexpr sclass styleClass{"textual.controls"};

    static constexpr sprop bordercol{"border.color"};
    static constexpr sprop onbgcol{"onbg.color"};
    static constexpr sprop offbgcol{"offbg.color"};
    static constexpr sprop hoveronbgcol{"hoveronbg.color"};
    static constexpr sprop hoveroffbgcol{"hoveroffbg.color"};
    static constexpr sprop textoncol{"texton.color"};
    static constexpr sprop textoffcol{"textoff.color"};
    static constexpr sprop texthoveroncol{"texthoveron.color"};
    static constexpr sprop texthoveroffcol{"texthoveroff.color"};

    static constexpr sprop labelfont{"label.font", sprop::FONT};

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withBaseClass(ControlStyles::styleClass)
            .withProperty(bordercol)
            .withProperty(onbgcol)
            .withProperty(offbgcol)
            .withProperty(hoveronbgcol)
            .withProperty(hoveroffbgcol)
            .withProperty(textoncol)
            .withProperty(textoffcol)
            .withProperty(texthoveroncol)
            .withProperty(texthoveroffcol)

            .withProperty(labelfont);
    }
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_BASESTYLES_H
