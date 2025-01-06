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

#ifndef INCLUDE_SST_JUCEGUI_ACCESSIBILITY_FOCUSDEBUGGER_H
#define INCLUDE_SST_JUCEGUI_ACCESSIBILITY_FOCUSDEBUGGER_H

#include <juce_gui_basics/juce_gui_basics.h>

#include "KeyboardTraverser.h"

namespace sst::jucegui::accessibility
{
struct FocusDebugger : public juce::FocusChangeListener
{
    juce::Component &debugInto;
    FocusDebugger(juce::Component &c) : debugInto(c)
    {
        juce::Desktop::getInstance().addFocusChangeListener(this);
    }
    ~FocusDebugger() { juce::Desktop::getInstance().removeFocusChangeListener(this); }
    void setDoFocusDebug(bool fd)
    {
        doFocusDebug = fd;
        if (doFocusDebug)
            guaranteeDebugComponent();
        if (debugComponent)
            debugComponent->setVisible(fd);
    }

    bool doFocusDebug{false};
    void globalFocusChanged(juce::Component *fc) override
    {
        if (!doFocusDebug)
            return;

        if (!fc)
            return;
        auto ofc = fc;
        guaranteeDebugComponent();
        debugComponent->toFront(false);
        auto bd = fc->getBounds();
        fc = fc->getParentComponent();
        while (fc && fc != &debugInto)
        {
            bd += fc->getBounds().getTopLeft();
            fc = fc->getParentComponent();
        }

        std::cout << "FD : [" << std::hex << ofc << std::dec << "] " << ofc->getTitle() << " @ "
                  << bd.toString() << " traversalId=" << KeyboardTraverser::traversalId(ofc)
                  << std::endl;
        debugComponent->setBounds(bd);
    }

    struct DbgComponent : juce::Component
    {
        DbgComponent()
        {
            setAccessible(false);
            setWantsKeyboardFocus(false);
            setMouseClickGrabsKeyboardFocus(false);
            setInterceptsMouseClicks(false, false);
            setTitle("Debug Component");
        }

        void paint(juce::Graphics &g) override
        {
            g.fillAll(juce::Colours::red.withAlpha(0.1f));
            g.setColour(juce::Colours::red);
            g.drawRect(getLocalBounds(), 1);
        }
    };

    void guaranteeDebugComponent()
    {
        if (!debugComponent)
        {
            debugComponent = std::make_unique<DbgComponent>();
            debugInto.addAndMakeVisible(*debugComponent);
        }
    }
    std::unique_ptr<juce::Component> debugComponent;
};
} // namespace sst::jucegui::accessibility

#endif // CONDUIT_FOCUSDEBUGGER_H
