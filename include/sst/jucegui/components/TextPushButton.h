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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TEXTPUSHBUTTON_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TEXTPUSHBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>
#include "ToggleButton.h"

#include <string>

#include "ComponentBase.h"
#include "CallbackButtonComponent.h"

namespace sst::jucegui::components
{
struct TextPushButton : public CallbackButtonComponent<TextPushButton>,
                        public style::StyleConsumer,
                        public style::SettingsConsumer,
                        public EditableComponentBase<TextPushButton>

{
    TextPushButton();
    ~TextPushButton();

    struct Styles : base_styles::BaseLabel, base_styles::PushButton
    {
        SCLASS(textpushbutton);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::PushButton::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass);
        }
    };

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextPushButton);
};
} // namespace sst::jucegui::components

#endif // SHORTCIRCUITXT_TEXTPUSHBUTTON_H
