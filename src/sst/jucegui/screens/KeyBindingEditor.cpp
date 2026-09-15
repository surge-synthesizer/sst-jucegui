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

#include "sst/jucegui/screens/KeyBindingEditor.h"

#include "sst/jucegui/components/GlyphPainter.h"
#include "sst/jucegui/components/ListView.h"
#include "sst/jucegui/components/TextPushButton.h"
#include "sst/jucegui/components/ToggleButton.h"
#include "sst/jucegui/data/Discrete.h"

namespace sst::jucegui::screens
{
namespace
{
juce::String capitaliseWords(const juce::String &s)
{
    juce::String res;
    bool atWordStart{true};
    for (auto c : s)
    {
        if (atWordStart && juce::CharacterFunctions::isLetter(c))
            c = juce::CharacterFunctions::toUpperCase(c);
        atWordStart = !juce::CharacterFunctions::isLetterOrDigit(c);
        res += juce::String::charToString(c);
    }
    return res;
}
} // namespace

bool KeyBindingEditor::Binding::sameKeyAs(const Binding &other) const
{
    if (type != other.type || type == UNBOUND)
        return false;
    if (shift != other.shift || alt != other.alt)
        return false;
    if (commandIsControl)
    {
        if ((command || control) != (other.command || other.control))
            return false;
    }
    else if (command != other.command || control != other.control)
    {
        return false;
    }

    if (type == KEYCODE)
        return keyCode == other.keyCode;
    return textChar == other.textChar;
}

bool KeyBindingEditor::Binding::operator==(const Binding &other) const
{
    if (type != other.type || active != other.active)
        return false;
    return type == UNBOUND || sameKeyAs(other);
}

KeyBindingEditor::Binding KeyBindingEditor::Binding::fromKeyPress(const juce::KeyPress &key)
{
    Binding res;
    res.type = KEYCODE;
    res.keyCode = key.getKeyCode();
    // linux can report lower case letters, and matching and streaming use upper
    if (res.keyCode >= 'a' && res.keyCode <= 'z')
        res.keyCode += 'A' - 'a';

    auto mods = key.getModifiers();
    res.shift = mods.isShiftDown();
    res.alt = mods.isAltDown();
    if (commandIsControl)
    {
        res.command = mods.isCommandDown() || mods.isCtrlDown();
    }
    else
    {
        res.command = mods.isCommandDown();
        res.control = mods.isCtrlDown();
    }
    return res;
}

juce::KeyPress KeyBindingEditor::Binding::toKeyPress() const
{
    int flags{juce::ModifierKeys::noModifiers};
    if (shift)
        flags |= juce::ModifierKeys::shiftModifier;
    if (alt)
        flags |= juce::ModifierKeys::altModifier;
    if (command)
        flags |= juce::ModifierKeys::commandModifier;
    if (control)
        flags |= juce::ModifierKeys::ctrlModifier;

    switch (type)
    {
    case KEYCODE:
        return juce::KeyPress(keyCode, juce::ModifierKeys(flags), 0);
    case TEXTCHAR:
        return juce::KeyPress((int)textChar, juce::ModifierKeys(flags), textChar);
    default:
        break;
    }
    return {};
}

std::string KeyBindingEditor::Binding::toDisplayString() const
{
    if (!isBound())
        return {};

    auto kp = toKeyPress();
#if JUCE_MAC
    // the open box space glyph is too small to read at label sizes
    auto desc =
        kp.getTextDescriptionWithIcons().replace(juce::String::charToString(0x2423), "Space");
#else
    auto desc = kp.getTextDescription()
                    .replace("ctrl + ", "Ctrl+")
                    .replace("shift + ", "Shift+")
                    .replace("alt + ", "Alt+");
#endif
    desc = desc.replace("cursor ", "").replace("spacebar", "space");
    return capitaliseWords(desc).toStdString();
}

struct KeyBindingEditor::ToggleData : data::Discrete
{
    std::string label;
    std::function<bool()> getter{nullptr};
    std::function<void(bool)> setter{nullptr};

    std::string getLabel() const override { return label; }
    int getValue() const override { return (getter && getter()) ? 1 : 0; }
    void setValueFromGUI(const int &v) override
    {
        if (setter)
            setter(v != 0);
    }
    void setValueFromModel(const int &) override {}
    std::string getValueAsStringFor(int i) const override { return i ? "On" : "Off"; }
};

// scrolling over a row should scroll the list rather than stop at the button
struct KeyBindingEditor::ForwardingWheelButton : components::TextPushButton
{
    void mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &w) override
    {
        if (auto p = getParentComponent())
            p->mouseWheelMove(e.getEventRelativeTo(p), w);
    }
};

struct KeyBindingEditor::Row : juce::Component, style::StyleConsumer
{
    KeyBindingEditor &editor;
    size_t rowIndex{0};
    juce::Rectangle<int> nameArea, keyArea;

    // data outlives the toggles which listen to it
    std::unique_ptr<ToggleData> activeData, learnData;
    std::unique_ptr<components::ToggleButton> activeToggle, learnToggle;
    std::unique_ptr<ForwardingWheelButton> resetButton;

    static constexpr int toggleWidth{rowHeight}, keyWidth{150}, learnWidth{60}, resetWidth{60};

    explicit Row(KeyBindingEditor &e) : style::StyleConsumer(Styles::styleClass), editor(e)
    {
        setAccessible(true);
        setFocusContainerType(juce::Component::FocusContainerType::focusContainer);

        activeData = std::make_unique<ToggleData>();
        activeData->getter = [this]() { return isEntry() && entry().binding.active; };
        activeData->setter = [this](bool b) {
            if (isEntry())
                editor.setActive(entryIndex(), b);
        };
        activeToggle = std::make_unique<components::ToggleButton>();
        activeToggle->setDrawMode(components::ToggleButton::DrawMode::FILLED);
        activeToggle->wheelForwardsToParent = true;
        activeToggle->setSource(activeData.get());
        addAndMakeVisible(*activeToggle);

        learnData = std::make_unique<ToggleData>();
        learnData->getter = [this]() {
            return isEntry() && editor.learningIndex == std::optional<size_t>(entryIndex());
        };
        learnData->setter = [this](bool b) {
            if (!isEntry())
                return;
            if (b)
                editor.startLearning(entryIndex());
            else if (editor.learningIndex == std::optional<size_t>(entryIndex()))
                editor.stopLearning();
        };
        learnToggle = std::make_unique<components::ToggleButton>();
        learnToggle->setLabel("Learn");
        learnToggle->wheelForwardsToParent = true;
        learnToggle->setSource(learnData.get());
        addAndMakeVisible(*learnToggle);

        resetButton = std::make_unique<ForwardingWheelButton>();
        resetButton->setLabel("Reset");
        resetButton->setOnCallback([this]() {
            if (isEntry())
                editor.resetToDefault(entryIndex());
        });
        addAndMakeVisible(*resetButton);
    }

    bool isEntry() const
    {
        return rowIndex < editor.rows.size() && !editor.rows[rowIndex].isHeader;
    }
    size_t entryIndex() const { return editor.rows[rowIndex].entryIndex; }
    const Entry &entry() const { return editor.entries[entryIndex()]; }

    void assign(size_t r)
    {
        rowIndex = r;
        if (rowIndex >= editor.rows.size())
            return;

        auto header = editor.rows[rowIndex].isHeader;
        activeToggle->setVisible(!header);
        learnToggle->setVisible(!header);
        resetButton->setVisible(!header);

        if (header)
        {
            setTitle(editor.rows[rowIndex].header);
        }
        else
        {
            const auto &e = entry();
            auto desc = e.binding.toDisplayString();
            setTitle(e.name + (desc.empty() ? std::string() : ", " + desc));

            activeData->label = "Toggle " + e.name;
            activeToggle->setTitle(activeData->label);
            learnData->label = "Learn " + e.name;
            learnToggle->setTitle(learnData->label);
            resetButton->setTitle("Reset " + e.name);
            resetButton->setEnabled(e.binding != e.defaultBinding);
        }

        activeToggle->repaint();
        learnToggle->repaint();
        repaint();
    }

    void resized() override
    {
        auto b = getLocalBounds();
        activeToggle->setBounds(b.removeFromLeft(toggleWidth).reduced(2));
        resetButton->setBounds(b.removeFromRight(resetWidth).reduced(2));
        learnToggle->setBounds(b.removeFromRight(learnWidth).reduced(2));
        keyArea = b.removeFromRight(keyWidth).reduced(margin, 0);
        nameArea = b.reduced(margin, 0);
    }

    void paint(juce::Graphics &g) override
    {
        if (!style() || rowIndex >= editor.rows.size())
            return;

        auto b = getLocalBounds();
        auto font = getFont(Styles::labelfont);
        auto textCol = getColour(Styles::labelcolor);
        const auto &ri = editor.rows[rowIndex];

        if (ri.isHeader)
        {
            g.setFont(font.boldened());
            g.setColour(textCol);
            g.drawText(ri.header, b.reduced(margin, 0), juce::Justification::centredLeft);
            g.setColour(getColour(Styles::brightoutline));
            g.drawHorizontalLine(b.getBottom() - 1, (float)b.getX(), (float)b.getRight());
            return;
        }

        const auto &e = entry();
        auto learning = editor.learningIndex == std::optional<size_t>(entryIndex());

        if (learning)
        {
            g.setColour(getColour(Styles::value).withAlpha(0.18f));
            g.fillRect(b);
        }
        else if (entryIndex() % 2 == 1)
        {
            g.setColour(textCol.withAlpha(0.04f));
            g.fillRect(b);
        }

        if (!e.binding.active)
            textCol = textCol.withAlpha(0.5f);

        g.setFont(font);
        g.setColour(textCol);
        g.drawText(e.name, nameArea, juce::Justification::centredLeft, true);

        auto keyText = e.binding.toDisplayString();
        auto keyCol = textCol;
        if (learning)
        {
            keyText = "Press a key...";
            keyCol = getColour(Styles::value);
        }
        else if (!e.binding.isBound())
        {
            keyText = "Unassigned";
            keyCol = textCol.withAlpha(0.4f);
        }
        else if (editor.hasConflict(entryIndex()))
        {
            keyCol = getColour(Styles::value);
        }
        g.setColour(keyCol);
        g.drawText(keyText, keyArea, juce::Justification::centredLeft, true);
    }
};

KeyBindingEditor::KeyBindingEditor(std::vector<Entry> e, const std::string &title)
    : components::NamedPanel(title), entries(std::move(e))
{
    listView = std::make_unique<components::ListView>();
    listView->strategy = components::ListView::BRUTE_FORCE;
    listView->selectionMode = components::ListView::NO_SELECTION;
    listView->getRowCount = [this]() { return (uint32_t)rows.size(); };
    listView->getRowHeight = []() { return (uint32_t)rowHeight; };
    listView->makeRowComponent = [this]() -> std::unique_ptr<juce::Component> {
        return std::make_unique<Row>(*this);
    };
    listView->assignComponentToRow = [](const std::unique_ptr<juce::Component> &c, uint32_t r) {
        if (auto rc = dynamic_cast<Row *>(c.get()))
            rc->assign(r);
    };
    addAndMakeVisible(*listView);

    auto mk = [this](const std::string &label, std::function<void(KeyBindingEditor *)> fn) {
        auto res = std::make_unique<components::TextPushButton>();
        res->setLabel(label);
        res->setOnCallback([w = juce::Component::SafePointer(this), fn]() {
            if (w)
                fn(w.getComponent());
        });
        addAndMakeVisible(*res);
        return res;
    };
    resetAllButton = mk("Reset All", [](auto *w) { w->resetAllToDefault(); });
    resetAllButton->setTitle("Reset all shortcuts to default");
    cancelButton = mk("Cancel", [](auto *w) { w->cancelPressed(); });
    okButton = mk("OK", [](auto *w) { w->okPressed(); });

    rebuildRows();
}

KeyBindingEditor::~KeyBindingEditor() = default;

void KeyBindingEditor::rebuildRows()
{
    rows.clear();
    std::optional<std::string> lastCategory;
    for (size_t i = 0; i < entries.size(); ++i)
    {
        const auto &e = entries[i];
        if (!e.category.empty() && lastCategory != e.category)
        {
            RowInfo h;
            h.isHeader = true;
            h.header = e.category;
            rows.push_back(h);
        }
        lastCategory = e.category;

        RowInfo r;
        r.entryIndex = i;
        rows.push_back(r);
    }
    listView->refresh(true);
}

void KeyBindingEditor::refreshRows()
{
    listView->reassignAllComponents();
    repaint();
}

void KeyBindingEditor::setStatus(const std::string &msg, bool warning)
{
    statusMessage = msg;
    statusIsWarning = warning;
    repaint(statusArea);
}

void KeyBindingEditor::okPressed()
{
    stopLearning();
    if (onOK)
        onOK(entries);
}

void KeyBindingEditor::cancelPressed()
{
    stopLearning();
    if (onCancel)
        onCancel();
}

void KeyBindingEditor::resetAllToDefault()
{
    stopLearning();
    for (auto &e : entries)
        e.binding = e.defaultBinding;
    setStatus("All shortcuts reset to their defaults", false);
    refreshRows();
}

void KeyBindingEditor::resetToDefault(size_t index)
{
    if (index >= entries.size())
        return;
    stopLearning();
    entries[index].binding = entries[index].defaultBinding;
    warnIfConflicting(index);
    refreshRows();
}

void KeyBindingEditor::setActive(size_t index, bool active)
{
    if (index >= entries.size())
        return;
    stopLearning();
    entries[index].binding.active = active;
    if (active)
        warnIfConflicting(index);
    refreshRows();
}

void KeyBindingEditor::warnIfConflicting(size_t index)
{
    if (auto c = findConflict(index, entries[index].binding))
        setStatus(entries[index].binding.toDisplayString() + " is also assigned to " +
                      entries[*c].name,
                  true);
    else
        setStatus("", false);
}

std::optional<size_t> KeyBindingEditor::findConflict(size_t index, const Binding &binding) const
{
    if (index >= entries.size() || !binding.isBound() || !binding.active)
        return std::nullopt;

    const auto &scope = entries[index].scope;
    for (size_t i = 0; i < entries.size(); ++i)
    {
        if (i == index)
            continue;
        const auto &other = entries[i];
        if (!other.binding.active || !other.binding.sameKeyAs(binding))
            continue;
        if (scope.empty() || other.scope.empty() || scope == other.scope)
            return i;
    }
    return std::nullopt;
}

bool KeyBindingEditor::hasConflict(size_t index) const
{
    return index < entries.size() && findConflict(index, entries[index].binding).has_value();
}

bool KeyBindingEditor::assignBinding(size_t index, const Binding &binding)
{
    if (index >= entries.size())
        return false;

    if (auto c = findConflict(index, binding))
    {
        setStatus(binding.toDisplayString() + " is already assigned to " + entries[*c].name +
                      ". Try another shortcut, or press Escape to cancel.",
                  true);
        return false;
    }

    entries[index].binding = binding;
    stopLearning();
    refreshRows();
    return true;
}

void KeyBindingEditor::startLearning(size_t index)
{
    if (index >= entries.size())
        return;

    learningIndex = index;
    setStatus("Press the new shortcut for " + entries[index].name + ", or Escape to cancel", false);
    refreshRows();

    // otherwise a focused toggle eats arrows, space and return
    if (isShowing())
        grabKeyboardFocus();
}

void KeyBindingEditor::stopLearning()
{
    if (!learningIndex.has_value())
        return;
    learningIndex.reset();
    setStatus("", false);
    refreshRows();
}

bool KeyBindingEditor::keyPressed(const juce::KeyPress &key)
{
    if (learningIndex.has_value())
    {
        if (key.getKeyCode() == juce::KeyPress::escapeKey &&
            !key.getModifiers().isAnyModifierKeyDown())
        {
            stopLearning();
            return true;
        }

        auto b = Binding::fromKeyPress(key);
        assignBinding(*learningIndex, b);
        return true;
    }

    if (key.getKeyCode() == juce::KeyPress::escapeKey && onCancel)
    {
        cancelPressed();
        return true;
    }

    return false;
}

void KeyBindingEditor::paint(juce::Graphics &g)
{
    components::NamedPanel::paint(g);

    auto s = style();
    if (statusMessage.empty() || !s)
        return;

    // the panel itself is styled as a namedpanel, which has no value colour
    g.setFont(s->getFont(Styles::styleClass, Styles::labelfont));
    g.setColour(
        s->getColour(Styles::styleClass, statusIsWarning ? Styles::value : Styles::labelcolor));
    g.drawFittedText(statusMessage, statusArea, juce::Justification::centredLeft, 2);
}

void KeyBindingEditor::resized()
{
    components::NamedPanel::resized();

    auto ca = getContentArea();
    auto footer = ca.removeFromBottom(footerHeight);
    ca.removeFromBottom(margin);
    listView->setBounds(ca);

    okButton->setBounds(footer.removeFromRight(buttonWidth).reduced(1));
    footer.removeFromRight(margin);
    cancelButton->setBounds(footer.removeFromRight(buttonWidth).reduced(1));
    footer.removeFromRight(margin * 4);
    resetAllButton->setBounds(footer.removeFromRight(buttonWidth).reduced(1));
    footer.removeFromRight(margin);
    statusArea = footer.reduced(margin, 0);
}

KeyBindingEditorModal::KeyBindingEditorModal(std::vector<KeyBindingEditor::Entry> entries,
                                             const std::string &title)
{
    processKeys = true;
    consumesAllKeys = true;

    editor = std::make_unique<KeyBindingEditor>(std::move(entries), title);
    editor->onOK = [w = juce::Component::SafePointer(this)](const auto &e) {
        if (!w)
            return;
        if (w->onOK)
            w->onOK(e);
        if (w)
            w->setVisible(false);
    };
    editor->onCancel = [w = juce::Component::SafePointer(this)]() {
        if (!w)
            return;
        if (w->onCancel)
            w->onCancel();
        if (w)
            w->setVisible(false);
    };
    addAndMakeVisible(*editor);
}

KeyBindingEditorModal::~KeyBindingEditorModal() = default;

void KeyBindingEditorModal::resized()
{
    ModalBase::resized();
    editor->setBounds(getContentArea());
}

void KeyBindingEditorModal::onEscape() { editor->cancelPressed(); }

void KeyBindingEditorModal::onReturn() { editor->okPressed(); }

} // namespace sst::jucegui::screens
