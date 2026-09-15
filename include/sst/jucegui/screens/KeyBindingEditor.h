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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_KEYBINDINGEDITOR_H
#define INCLUDE_SST_JUCEGUI_SCREENS_KEYBINDINGEDITOR_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "../components/BaseStyles.h"
#include "../components/NamedPanel.h"
#include "../style/StyleSheet.h"
#include "ModalBase.h"

namespace sst::jucegui::components
{
struct ListView;
struct TextPushButton;
} // namespace sst::jucegui::components

namespace sst::jucegui::screens
{
// works on a copy of the entries; the templates below adapt a plugininfra KeyMapManager
struct KeyBindingEditor : components::NamedPanel
{
    struct Styles : components::base_styles::Base,
                    components::base_styles::Outlined,
                    components::base_styles::BaseLabel,
                    components::base_styles::ValueBearing
    {
        SCLASS(keybindingeditor);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(components::base_styles::Base::styleClass)
                .withBaseClass(components::base_styles::Outlined::styleClass)
                .withBaseClass(components::base_styles::BaseLabel::styleClass)
                .withBaseClass(components::base_styles::ValueBearing::styleClass);
        }
    };

    struct Binding
    {
        enum Type
        {
            UNBOUND,
            KEYCODE,
            TEXTCHAR
        } type{UNBOUND};

        int keyCode{0};
        juce::juce_wchar textChar{0};
        bool shift{false}, alt{false}, command{false}, control{false};
        bool active{true};

        // juce folds command into control off macOS
        static constexpr bool commandIsControl =
            juce::ModifierKeys::commandModifier == juce::ModifierKeys::ctrlModifier;

        bool isBound() const { return type != UNBOUND; }
        bool sameKeyAs(const Binding &other) const;
        bool operator==(const Binding &other) const;
        bool operator!=(const Binding &other) const { return !(*this == other); }

        static Binding fromKeyPress(const juce::KeyPress &key);
        juce::KeyPress toKeyPress() const;
        std::string toDisplayString() const;
    };

    struct Entry
    {
        // opaque to the editor; usually the command enum value
        int id{0};
        std::string name;
        // a header row is shown whenever this changes between entries
        std::string category{};
        // empty is global; bindings clash when scopes match or either is global
        std::string scope{};
        Binding binding{}, defaultBinding{};
    };

    explicit KeyBindingEditor(std::vector<Entry> entries,
                              const std::string &title = "Keyboard Shortcuts");
    ~KeyBindingEditor() override;

    const std::vector<Entry> &getEntries() const { return entries; }

    std::function<void(const std::vector<Entry> &)> onOK{nullptr};
    std::function<void()> onCancel{nullptr};

    void okPressed();
    void cancelPressed();

    void resetAllToDefault();
    void resetToDefault(size_t index);
    void setActive(size_t index, bool active);

    // refuses a binding which clashes with another active one and says why
    bool assignBinding(size_t index, const Binding &binding);
    std::optional<size_t> findConflict(size_t index, const Binding &binding) const;
    bool hasConflict(size_t index) const;

    void startLearning(size_t index);
    void stopLearning();
    std::optional<size_t> getLearningIndex() const { return learningIndex; }

    std::string getStatusMessage() const { return statusMessage; }

    bool keyPressed(const juce::KeyPress &key) override;
    void paint(juce::Graphics &g) override;
    void resized() override;

    template <typename M>
    static std::vector<Entry>
    entriesFromKeyMapManager(const M &manager, const std::function<std::string(int)> &nameOf,
                             const std::function<std::string(int)> &categoryOf = nullptr,
                             const std::function<std::string(int)> &scopeOf = nullptr);

    template <typename M>
    static void applyEntriesToKeyMapManager(M &manager, const std::vector<Entry> &entries);

    template <typename M> static Binding fromKeyMapBinding(const typename M::Binding &b);
    template <typename M> static typename M::Binding toKeyMapBinding(const Binding &b);

  private:
    struct Row;
    struct ToggleData;
    struct ForwardingWheelButton;

    struct RowInfo
    {
        bool isHeader{false};
        std::string header{};
        size_t entryIndex{0};
    };

    std::vector<Entry> entries;
    std::vector<RowInfo> rows;
    std::optional<size_t> learningIndex;
    std::string statusMessage;
    bool statusIsWarning{false};

    std::unique_ptr<components::ListView> listView;
    std::unique_ptr<components::TextPushButton> okButton, cancelButton, resetAllButton;
    juce::Rectangle<int> statusArea;

    void rebuildRows();
    void refreshRows();
    void setStatus(const std::string &msg, bool warning);
    void warnIfConflicting(size_t index);

    static constexpr int rowHeight{22}, footerHeight{26}, buttonWidth{80}, margin{4};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyBindingEditor)
};

// hosts a KeyBindingEditor in a ScreenHolder style modal overlay
struct KeyBindingEditorModal : ModalBase
{
    explicit KeyBindingEditorModal(std::vector<KeyBindingEditor::Entry> entries,
                                   const std::string &title = "Keyboard Shortcuts");
    ~KeyBindingEditorModal() override;

    std::function<void(const std::vector<KeyBindingEditor::Entry> &)> onOK{nullptr};
    std::function<void()> onCancel{nullptr};

    juce::Point<int> innerContentSize() override { return {640, 560}; }
    void resized() override;
    void onEscape() override;
    void onReturn() override;

    std::unique_ptr<KeyBindingEditor> editor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyBindingEditorModal)
};

template <typename M>
KeyBindingEditor::Binding KeyBindingEditor::fromKeyMapBinding(const typename M::Binding &b)
{
    using mb_t = typename M::Binding;

    Binding res;
    switch (b.type)
    {
    case mb_t::KEYCODE:
        res.type = Binding::KEYCODE;
        break;
    case mb_t::TEXTCHAR:
        res.type = Binding::TEXTCHAR;
        break;
    default:
        res.type = Binding::UNBOUND;
        break;
    }
    res.keyCode = b.keyCode;
    res.textChar = (juce::juce_wchar)(unsigned char)b.textChar;
    res.shift = (b.modifier & M::SHIFT) != 0;
    res.alt = (b.modifier & M::ALT) != 0;
    res.command = (b.modifier & M::COMMAND) != 0;
    res.control = (b.modifier & M::CONTROL) != 0;
    if (Binding::commandIsControl && res.control)
    {
        res.command = true;
        res.control = false;
    }
    res.active = b.active;
    return res;
}

template <typename M>
typename M::Binding KeyBindingEditor::toKeyMapBinding(const KeyBindingEditor::Binding &b)
{
    using mb_t = typename M::Binding;

    mb_t res;
    switch (b.type)
    {
    case Binding::KEYCODE:
        res.type = mb_t::KEYCODE;
        break;
    case Binding::TEXTCHAR:
        res.type = mb_t::TEXTCHAR;
        break;
    default:
        res.type = mb_t::INVALID;
        break;
    }
    res.keyCode = b.keyCode;
    res.textChar = (char)b.textChar;
    uint32_t mod{0};
    if (b.shift)
        mod |= M::SHIFT;
    if (b.alt)
        mod |= M::ALT;
    if (b.command)
        mod |= M::COMMAND;
    if (b.control)
        mod |= M::CONTROL;
    res.modifier = mod;
    res.active = b.active;
    return res;
}

template <typename M>
std::vector<KeyBindingEditor::Entry>
KeyBindingEditor::entriesFromKeyMapManager(const M &manager,
                                           const std::function<std::string(int)> &nameOf,
                                           const std::function<std::string(int)> &categoryOf,
                                           const std::function<std::string(int)> &scopeOf)
{
    using funcs_t = typename std::decay_t<decltype(manager.bindings)>::key_type;

    std::vector<Entry> res;
    res.reserve(M::numFuncs);
    for (int i = 0; i < M::numFuncs; ++i)
    {
        Entry e;
        e.id = i;
        e.name = nameOf(i);
        // an empty name keeps the action out of the editor
        if (e.name.empty())
            continue;
        if (categoryOf)
            e.category = categoryOf(i);
        if (scopeOf)
            e.scope = scopeOf(i);

        auto f = (funcs_t)i;
        if (auto b = manager.bindings.find(f); b != manager.bindings.end())
            e.binding = fromKeyMapBinding<M>(b->second);
        if (auto d = manager.defaultBindings.find(f); d != manager.defaultBindings.end())
            e.defaultBinding = fromKeyMapBinding<M>(d->second);
        res.push_back(std::move(e));
    }
    return res;
}

template <typename M>
void KeyBindingEditor::applyEntriesToKeyMapManager(M &manager, const std::vector<Entry> &entries)
{
    using funcs_t = typename std::decay_t<decltype(manager.bindings)>::key_type;

    for (const auto &e : entries)
    {
        auto f = (funcs_t)e.id;
        auto hasDefault = manager.defaultBindings.find(f) != manager.defaultBindings.end();
        // an unbound binding only needs storing if it hides a default
        if (!e.binding.isBound() && !hasDefault)
            manager.bindings.erase(f);
        else
            manager.bindings[f] = toKeyMapBinding<M>(e.binding);
    }
}
} // namespace sst::jucegui::screens

#endif // INCLUDE_SST_JUCEGUI_SCREENS_KEYBINDINGEDITOR_H
