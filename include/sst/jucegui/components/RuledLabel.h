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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_RULEDLABEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_RULEDLABEL_H

#include <functional>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "Label.h"
#include "GlyphPainter.h"

namespace sst::jucegui::components
{
struct RuledLabel : public juce::Component,
                    public style::StyleConsumer,
                    public style::SettingsConsumer
{
    struct Styles : base_styles::BaseLabel, base_styles::Outlined
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"label"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withBaseClass(base_styles::Outlined::styleClass);
        }
    };

    RuledLabel(int pad = 5) : style::StyleConsumer(Styles::styleClass), labelPad(pad)
    {
        setAccessible(true);
    };
    ~RuledLabel() = default;

    void setText(const std::string &s)
    {
        text = s;
        setTitle(s);
        repaint();
    }
    std::string getText() const { return text; }

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return std::make_unique<juce::AccessibilityHandler>(*this, juce::AccessibilityRole::label);
    }

    // Optional hamburger affordance at the right edge (mirrors NamedPanel's API). When
    // enabled, a three-ellipsis menu mark is drawn at the right and clicking it invokes
    // onHamburger.
    bool hasHamburger{false};
    std::function<void()> onHamburger{nullptr};

    juce::Rectangle<int> hamburgerBounds() const
    {
        static constexpr auto hamburgerSize = 22;
        auto ht = getLocalBounds();
        auto hb = ht.withLeft(ht.getRight() - hamburgerSize);
        return hb;
    }

    int labelPad{5};
    void paint(juce::Graphics &g) override
    {
        g.setColour(getColour(Styles::labelcolor));
        if (!isEnabled())
            g.setColour(getColour(Styles::labelcolor).withAlpha(0.5f));
        g.setFont(getFont(Styles::labelfont));
        g.drawText(text, getLocalBounds(), juce::Justification::centred);

        auto labelWidth = SST_STRING_WIDTH_INT(g.getCurrentFont(), text);

        auto ht = getLocalBounds();
        // Stop the right-hand rule before the hamburger glyph when present.
        auto rightEnd = hasHamburger ? hamburgerBounds().getX() : ht.getWidth();
        g.setColour(getColour(Styles::brightoutline));
        g.drawHorizontalLine(ht.getHeight() / 2, 0, (ht.getWidth() - labelWidth) / 2 - labelPad);
        g.drawHorizontalLine(ht.getHeight() / 2, (ht.getWidth() + labelWidth) / 2 + labelPad,
                             rightEnd);

        if (hasHamburger)
        {
            auto col = getColour(Styles::brightoutline);
            if (!isEnabled())
                col = col.withAlpha(0.5f);
            g.setColour(col);

            // Three ellipses, copied from NamedPanel's hamburger so the affordance matches.
            auto hb = hamburgerBounds();
            auto cx = hb.getCentreX();
            auto cy = hb.getCentreY();
            auto w = hb.getWidth() / 3.5;

            auto sz = 1.5;
            g.fillEllipse(cx - sz, cy - sz, 2 * sz, 2 * sz);
            g.fillEllipse(cx - w - sz, cy - sz, 2 * sz, 2 * sz);
            g.fillEllipse(cx + w - sz, cy - sz, 2 * sz, 2 * sz);
        }
    }

    void mouseDown(const juce::MouseEvent &e) override
    {
        if (hasHamburger && onHamburger && hamburgerBounds().contains(e.getPosition()))
            onHamburger();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuledLabel)

  protected:
    std::string text;
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_RULEDLABEL_H
