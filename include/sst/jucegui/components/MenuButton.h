/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-basic-blocks is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-juce-gui available at
 * https://github.com/surge-synthesizer/sst-juce-gui
 */

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_MENUBUTTON_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_MENUBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>
#include "ToggleButton.h"

#include <string>

#include "ComponentBase.h"
#include "CallbackButtonComponent.h"
#include <sst/jucegui/components/DiscreteParamMenuBuilder.h>

namespace sst::jucegui::components
{
template <typename T>
struct MenuButtonPainter : public style::StyleConsumer, public style::SettingsConsumer
{
    struct Styles : base_styles::Outlined, base_styles::BaseLabel
    {
        SCLASS(menubutton);

        PROP_HOVER(menuarrow);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Outlined::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withProperty(menuarrow_hover);
        }
    };

    MenuButtonPainter() : style::StyleConsumer(Styles::styleClass) {}
    void paintMenuButton(juce::Graphics &g, const std::string &);
};

struct MenuButton : public CallbackButtonComponent<MenuButton>,
                    public MenuButtonPainter<MenuButton>,
                    public EditableComponentBase<MenuButton>

{
    MenuButton();
    ~MenuButton();

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuButton)
};

struct MenuButtonDiscreteEditor : public DiscreteParamEditor,
                                  public MenuButtonPainter<MenuButtonDiscreteEditor>,
                                  public HasDiscreteParamMenuBuilder
{
    MenuButtonDiscreteEditor() {}
    ~MenuButtonDiscreteEditor() = default;

    void mouseDown(const juce::MouseEvent &e) override;

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuButtonDiscreteEditor);
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_MENUBUTTON_H
