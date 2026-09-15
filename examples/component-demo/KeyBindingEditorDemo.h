/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2026, various authors, as described in the GitHub
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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_KEYBINDINGEDITORDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_KEYBINDINGEDITORDEMO_H

#include <iostream>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/screens/KeyBindingEditor.h>

struct KeyBindingEditorDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "KeyBindingEditor";

    using kbe_t = sst::jucegui::screens::KeyBindingEditor;

    static kbe_t::Binding key(int code, bool cmd = false, bool shift = false, bool alt = false)
    {
        kbe_t::Binding b;
        b.type = kbe_t::Binding::KEYCODE;
        b.keyCode = code;
        b.command = cmd;
        b.shift = shift;
        b.alt = alt;
        return b;
    }

    KeyBindingEditorDemo()
    {
        std::vector<kbe_t::Entry> entries;
        auto add = [&](const std::string &n, const std::string &cat, const std::string &scope,
                       kbe_t::Binding b) {
            kbe_t::Entry e;
            e.id = (int)entries.size();
            e.name = n;
            e.category = cat;
            e.scope = scope;
            e.binding = b;
            e.defaultBinding = b;
            entries.push_back(e);
        };

        add("Undo", "Global", "", key('Z', true));
        add("Redo", "Global", "", key('Z', true, true));
        add("Show Keyboard Shortcuts", "Global", "", key('B', false, false, true));
        add("Select All", "List", "list", key('A', true));
        add("Delete Selection", "List", "list", key(juce::KeyPress::backspaceKey));
        add("Select Next", "List", "list", key(juce::KeyPress::downKey));
        add("Select Previous", "List", "list", key(juce::KeyPress::upKey));
        add("Rename", "List", "list", {});
        add("Select All Files", "Browser", "browser", key('A', true));
        add("Preview File", "Browser", "browser", key(juce::KeyPress::spaceKey));

        editor = std::make_unique<kbe_t>(entries);
        editor->onOK = [](const auto &res) {
            for (const auto &e : res)
                std::cout << e.name << " -> '" << e.binding.toDisplayString() << "'"
                          << (e.binding.active ? "" : " (inactive)") << std::endl;
        };
        editor->onCancel = []() { std::cout << "Cancelled" << std::endl; };
        addAndMakeVisible(*editor);
    }

    void resized() override { editor->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<kbe_t> editor;
};

#endif // SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_KEYBINDINGEDITORDEMO_H
