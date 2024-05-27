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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_LABEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_LABEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct Label : public juce::Component, public style::StyleConsumer, public style::SettingsConsumer
{
    struct Styles : base_styles::BaseLabel
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"label"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass);
        }
    };

    Label() : style::StyleConsumer(Styles::styleClass) { setAccessible(true); };
    ~Label() = default;

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

    juce::Justification justification{juce::Justification::centred};
    void setJustification(juce::Justification j)
    {
        justification = j;
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        g.setColour(getColour(Styles::labelcolor));
        if (!isEnabled())
            g.setColour(getColour(Styles::labelcolor).withAlpha(0.5f));
        g.setFont(getFont(Styles::labelfont));
        g.drawText(text, getLocalBounds(), justification);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label)

  private:
    std::string text;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_LABEL_H
