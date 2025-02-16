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

#ifndef INCLUDE_SST_JUCEGUI_STYLE_JUCELOOKANDFEELADAPTER_H
#define INCLUDE_SST_JUCEGUI_STYLE_JUCELOOKANDFEELADAPTER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "StyleSheet.h"
#include "sst/jucegui/components/BaseStyles.h"
#include <memory>
#include <thread>

namespace sst::jucegui::style
{

struct LookAndFeelManager
{
    LookAndFeelManager(juce::Component *intoThis)
    {
        lnf = std::make_shared<SSTJGLookAndFeel>();
        intoThis->setLookAndFeel(lnf.get());
    }

    void setStyle(StyleSheet::ptr_t ss) { lnf->setStyle(ss); }

  private:
    struct SSTJGLookAndFeel : juce::LookAndFeel_V4
    {
        SSTJGLookAndFeel()
        {
            // std::cout << "Making LNF " << this << std::endl;
        }
        ~SSTJGLookAndFeel()
        {
            // std::cout << "Deleting LNF " << this << std::endl;
        }

        juce::Font popupMenuFont{SST_JUCE_FONT_CTOR("Comic Sans MS", "", 18)};

        /*
         * Critically do *not* keep a long lived reference to style
         * since it can go away and this can live in shutdown paths.
         * Grab values when informed of a style change.
         */

        void setStyle(StyleSheet::ptr_t style)
        {
            if (!style)
                return;

            namespace bst = components::base_styles;
            setColour(juce::PopupMenu::ColourIds::backgroundColourId,
                      style->getColour(bst::PopupMenu::styleClass, bst::PopupMenu::background));
            setColour(juce::PopupMenu::ColourIds::textColourId,
                      style->getColour(bst::PopupMenu::styleClass, bst::PopupMenu::labelcolor));
            setColour(juce::PopupMenu::ColourIds::headerTextColourId,
                      style->getColour(bst::PopupMenu::styleClass, bst::PopupMenu::labelcolor));

            setColour(
                juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
                style->getColour(bst::PopupMenu::styleClass, bst::PopupMenu::hightlightbackground));
            setColour(
                juce::PopupMenu::ColourIds::highlightedTextColourId,
                style->getColour(bst::PopupMenu::styleClass, bst::PopupMenu::hightlighttextcolor));

            setColour(juce::TabbedComponent::ColourIds::backgroundColourId,
                      juce::Colours::black.withAlpha(0.f));
            setColour(juce::TabbedComponent::ColourIds::outlineColourId,
                      juce::Colours::black.withAlpha(0.f));
            setColour(juce::TabbedButtonBar::ColourIds::tabOutlineColourId,
                      juce::Colours::black.withAlpha(0.f));

            popupMenuFont = style->getFont(components::base_styles::PopupMenu::styleClass,
                                           components::base_styles::PopupMenu::menufont);
        }

        juce::Font getPopupMenuFont() override { return popupMenuFont; }
        void drawPopupMenuBackgroundWithOptions(juce::Graphics &g, int width, int height,
                                                const juce::PopupMenu::Options &o) override
        {
            auto background = findColour(juce::PopupMenu::backgroundColourId);

            g.fillAll(background);

            g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.6f));
            g.drawRect(0, 0, width, height);
        }
    };
#if 0
    static inline std::weak_ptr<SSTJGLookAndFeel> weakPtr;
    static inline std::mutex weakPtrMutex;
    static std::shared_ptr<SSTJGLookAndFeel> getSingleton()
    {
        // this really is likely to be single threaded, but better safe etc in case
        // a linux host gets excitable with multiple main threads
        std::lock_guard<std::mutex> grd(weakPtrMutex);
        auto sp = weakPtr.lock();
        if (sp)
        {
            return sp;
        }
        sp = std::make_shared<SSTJGLookAndFeel>();
        weakPtr = sp;
        return sp;
    }
#endif
    std::shared_ptr<SSTJGLookAndFeel> lnf;
};

} // namespace sst::jucegui::style

#endif // JUCELOOKANDFEELADAPTER_H
