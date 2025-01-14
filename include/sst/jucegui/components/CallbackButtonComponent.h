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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_CALLBACKBUTTONCOMPONENT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_CALLBACKBUTTONCOMPONENT_H

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::components
{
template <typename T> struct CallbackButtonComponent : public juce::Component
{
    CallbackButtonComponent()
    {
        setAccessible(true);
        setWantsKeyboardFocus(true);
        setTitle("Unlabeled Callback Button");
    }
    T *asT() { return static_cast<T *>(this); }

    void setOnCallback(const std::function<void()> &cb) { onCB = cb; }

    void setLabel(const std::string &l) { setLabelAndTitle(l, l); }

    void setLabelAndTitle(const std::string &l, const std::string &t)
    {
        label = l;
        setTitle(t);
        if (auto h = createAccessibilityHandler())
        {
            h->notifyAccessibilityEvent(juce::AccessibilityEvent::titleChanged);
            h->notifyAccessibilityEvent(juce::AccessibilityEvent::textChanged);
        }
        repaint();
    }

    std::string getLabel() const { return label; }

    void mouseEnter(const juce::MouseEvent &e) override
    {
        asT()->startHover();
        asT()->repaint();
    }
    void mouseExit(const juce::MouseEvent &e) override
    {
        asT()->endHover();
        asT()->repaint();
    }

    void mouseDown(const juce::MouseEvent &e) override
    {
        isPressed = true;
        if (onCB)
            onCB();
        repaint();
    }

    void mouseUp(const juce::MouseEvent &e) override
    {
        isPressed = false;
        repaint();
    }

    bool keyPressed(const juce::KeyPress &key) override
    {
        if (key.getKeyCode() == juce::KeyPress::returnKey && onCB)
        {
            onCB();
            repaint();
            return true;
        }
        return false;
    }

    bool isPressed{false};

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return std::make_unique<juce::AccessibilityHandler>(*this, juce::AccessibilityRole::button);
    }

  protected:
    std::string label;
    std::function<void()> onCB{nullptr};
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_CALLBACKBUTTONCOMPONENT_H
