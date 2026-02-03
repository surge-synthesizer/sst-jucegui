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

#ifndef INCLUDE_SST_JUCEGUI_ACCESSIBILITY_ACCESSIBILITYKEYBOARDEDITS_H
#define INCLUDE_SST_JUCEGUI_ACCESSIBILITY_ACCESSIBILITYKEYBOARDEDITS_H

#include <juce_gui_basics/juce_gui_basics.h>

#include "AccessibilityConfiguration.h"

namespace sst::jucegui::accessibility
{
template <typename T> struct AccessibilityKeyboardEditSupport
{
    T *asT() { return static_cast<T *>(this); }

    struct Action
    {
        enum AccessibleKeyEditAction
        {
            None,
            Trigger,
            Increase,
            Decrease,
            ToMax,
            ToMin,
            ToDefault,
            OpenMenu,
            OpenEditor
        } action{None};

        enum AccessibleKeyModifier
        {
            NoModifier,
            Fine,
            Quantized
        } mod{NoModifier};

        Action() {}
        Action(AccessibleKeyEditAction a) : action(a), mod(NoModifier) {}
        Action(AccessibleKeyEditAction a, AccessibleKeyModifier m) : action(a), mod(m) {}

        bool isNone() const { return action == None; }
    };

    Action accessibleEdit(const juce::KeyPress &key)
    {
        if (!asT()->accessibilityConfiguration->useKeyboardEdits)
        {
            return {};
        }

        if (key.getKeyCode() == juce ::KeyPress::downKey)
        {
            if (key.getModifiers().isShiftDown())
                return {Action::Decrease, Action::Fine};
            if (key.getModifiers().isCommandDown())
                return {Action::Decrease, Action::Quantized};
            return {Action::Decrease};
        }

        if (key.getKeyCode() == juce ::KeyPress::upKey)
        {
            if (key.getModifiers().isShiftDown())
                return {Action::Increase, Action::Fine};
            if (key.getModifiers().isCommandDown())
                return {Action::Increase, Action::Quantized};
            return {Action::Increase};
        }

        if (key.getKeyCode() == juce::KeyPress::F10Key && key.getModifiers().isShiftDown())
        {
            return {Action::OpenMenu};
        }

        if (key.getKeyCode() == 93)
        {
            return {Action::OpenMenu};
        }

        if (key.getKeyCode() == juce::KeyPress::returnKey && key.getModifiers().isShiftDown())
        {
            return {Action::OpenEditor};
        }

        if (key.getKeyCode() == juce::KeyPress::homeKey)
        {
            return {Action::ToMax};
        }

        if (key.getKeyCode() == juce::KeyPress::endKey)
        {
            return {Action::ToMin};
        }

        if (key.getKeyCode() == juce::KeyPress::deleteKey)
        {
            return {Action::ToDefault};
        }

        if (key.getKeyCode() == juce::KeyPress::returnKey)
        {
            return {Action::Trigger};
        }

        return {};
    }
};
} // namespace sst::jucegui::accessibility

#endif // CONDUIT_ACCESIBILITYKEYBOARDEDITS_H
