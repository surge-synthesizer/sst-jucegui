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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_SCREENHOLDER_H
#define INCLUDE_SST_JUCEGUI_SCREENS_SCREENHOLDER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <set>
#include <memory>

namespace sst::jucegui::screens
{
template <typename T> struct ScreenHolder
{
    juce::Component *asC() { return static_cast<T *>(this); }

    struct InternalListener : juce::ComponentListener
    {
        ScreenHolder<T> &holder;
        InternalListener(ScreenHolder<T> &sh) : holder(sh) {}

        void eraseLater(juce::Component *ca)
        {
            for (auto c = holder.modalOverlays.begin(); c != holder.modalOverlays.end(); ++c)
            {
                if (c->get() == ca)
                {
                    holder.modalOverlays.erase(c);
                    break;
                }
            }
        }

        void componentVisibilityChanged(juce::Component &component) override
        {
            if (!component.isVisible())
            {
                // yield the ui thread in case after setVisible false there's still component work
                // to do in callbacks etc
                juce::Timer::callAfterDelay(1, [this, ca = &component]() { eraseLater(ca); });
            }
        }
    } listener;

    ScreenHolder() : listener(*this) {}

    void displayModalOverlay(std::unique_ptr<juce::Component> &&mo)
    {
        mo->addComponentListener(&listener);
        mo->setBounds(asC()->getLocalBounds());
        asC()->addAndMakeVisible(*mo);
        mo->toFront(true);
        modalOverlays.insert(std::move(mo));
    }

    template <typename OT> OT *searchForOverlay()
    {
        for (auto &c : modalOverlays)
        {
            if (auto t = dynamic_cast<OT *>(c.get()))
                return t;
        }
        return nullptr;
    }

    std::set<std::unique_ptr<juce::Component>> modalOverlays;
};

} // namespace sst::jucegui::screens
#endif // SCREENHOLDER_H
