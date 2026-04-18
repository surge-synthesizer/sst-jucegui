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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_COLOREDITOR_H
#define INCLUDE_SST_JUCEGUI_SCREENS_COLOREDITOR_H

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../components/Label.h"
#include "../components/NamedPanel.h"
#include "../components/ListView.h"
#include "../components/TextEditor.h"
#include "../style/StyleSheet.h"
#include "ModalBase.h"

namespace sst::jucegui::screens
{

struct ColorEditor : components::NamedPanel, private juce::ChangeListener
{
    // -------------------------------------------------------------------------
    // Public types
    // -------------------------------------------------------------------------
    struct ColorEntry
    {
        std::string tag;
        juce::Colour color;
    };

    using ColorChangedFn = std::function<void(const std::string &, juce::Colour)>;

    struct StyleKey
    {
        const style::StyleSheet::Class &cls;
        const style::StyleSheet::Property &prop;
        std::string label;
    };

    struct ColorMapEntry
    {
        std::string name;
        std::vector<StyleKey> keys;
    };

    // -------------------------------------------------------------------------
    // Public data — entries is public so callers can refresh after theme load
    // -------------------------------------------------------------------------
    std::vector<ColorEntry> entries;

    // Optional extra callback fired after every colour update (any entry, including
    // live slider drags). Use this to repaint a parent component in real time.
    std::function<void()> onAnyColorChanged;

    // -------------------------------------------------------------------------
    // Constructor / destructor
    // -------------------------------------------------------------------------
    ColorEditor(std::vector<ColorEntry> initialEntries, ColorChangedFn callback,
                bool includeAlpha = false)
        : components::NamedPanel("Color Editor"), entries(std::move(initialEntries)),
          onColorChanged(std::move(callback)), includeAlpha(includeAlpha)
    {
        setupListView();
    }

    ~ColorEditor()
    {
        if (activePicker)
            activePicker->removeChangeListener(this);
    }

    // -------------------------------------------------------------------------
    // resized
    // -------------------------------------------------------------------------
    void resized() override
    {
        components::NamedPanel::resized();
        listView->setBounds(getContentArea());
    }

    // -------------------------------------------------------------------------
    // Static factory: forStyleKeys
    // -------------------------------------------------------------------------
    static std::unique_ptr<ColorEditor> forStyleKeys(style::StyleSheet::ptr_t stylesheet,
                                                     std::vector<StyleKey> keys,
                                                     bool includeAlpha = false)
    {
        std::vector<ColorEntry> ents;
        ents.reserve(keys.size());
        for (auto &k : keys)
        {
            std::string tag =
                k.label.empty() ? (k.cls.cname + std::string("::") + k.prop.pname) : k.label;
            ents.push_back({tag, stylesheet->getColour(k.cls, k.prop)});
        }

        ColorChangedFn cb = [stylesheet, keys](const std::string &tag, juce::Colour c) {
            for (auto &k : keys)
            {
                std::string t =
                    k.label.empty() ? (k.cls.cname + std::string("::") + k.prop.pname) : k.label;
                if (t == tag)
                {
                    stylesheet->setColour(k.cls, k.prop, c);
                    break;
                }
            }
        };

        return std::make_unique<ColorEditor>(std::move(ents), std::move(cb), includeAlpha);
    }

    // -------------------------------------------------------------------------
    // Static factory: forColorMap
    // -------------------------------------------------------------------------
    static std::unique_ptr<ColorEditor> forColorMap(style::StyleSheet::ptr_t stylesheet,
                                                    std::vector<ColorMapEntry> colorMap,
                                                    bool includeAlpha = false)
    {
        std::vector<ColorEntry> ents;
        ents.reserve(colorMap.size());
        for (auto &group : colorMap)
        {
            juce::Colour firstColor;
            if (!group.keys.empty())
                firstColor = stylesheet->getColour(group.keys[0].cls, group.keys[0].prop);
            ents.push_back({group.name, firstColor});
        }

        ColorChangedFn cb = [stylesheet, colorMap](const std::string &tag, juce::Colour c) {
            for (auto &group : colorMap)
            {
                if (group.name == tag)
                {
                    for (auto &k : group.keys)
                        stylesheet->setColour(k.cls, k.prop, c);
                    break;
                }
            }
        };

        return std::make_unique<ColorEditor>(std::move(ents), std::move(cb), includeAlpha);
    }

  private:
    // -------------------------------------------------------------------------
    // Constants
    // -------------------------------------------------------------------------
    static constexpr int rowHeight = 28;
    static constexpr int swatchWidth = 32;
    static constexpr int labelWidth = 160;
    static constexpr int hexFieldWidth = 120;

    // -------------------------------------------------------------------------
    // Private members
    // -------------------------------------------------------------------------
    ColorChangedFn onColorChanged;
    bool includeAlpha;
    std::unique_ptr<components::ListView> listView;
    int activePickerIdx{-1};
    juce::Component::SafePointer<juce::ColourSelector> activePicker;

    // -------------------------------------------------------------------------
    // ChangeListener callback — receives colour changes from juce::ColourSelector
    // -------------------------------------------------------------------------
    void changeListenerCallback(juce::ChangeBroadcaster *source) override
    {
        if (!activePicker || source != activePicker.getComponent())
            return;
        if (activePickerIdx < 0 || activePickerIdx >= (int)entries.size())
            return;
        updateEntry(activePickerIdx, activePicker->getCurrentColour());
    }

    // -------------------------------------------------------------------------
    // Private methods
    // -------------------------------------------------------------------------
    void setupListView()
    {
        listView = std::make_unique<components::ListView>();
        listView->strategy = components::ListView::BRUTE_FORCE_NO_REUSE;
        listView->selectionMode = components::ListView::NO_SELECTION;

        listView->getRowCount = [this]() -> uint32_t { return (uint32_t)entries.size(); };
        listView->getRowHeight = [this]() -> uint32_t { return (uint32_t)rowHeight; };
        listView->makeRowComponent = [this]() -> std::unique_ptr<juce::Component> {
            return std::make_unique<RowComponent>(*this);
        };
        listView->assignComponentToRow = [](const std::unique_ptr<juce::Component> &comp,
                                            uint32_t row) {
            if (auto *rc = dynamic_cast<RowComponent *>(comp.get()))
                rc->assign((int)row);
        };

        addAndMakeVisible(*listView);
    }

    std::string toHexString(juce::Colour c) const
    {
        return "#" + c.toDisplayString(includeAlpha).toUpperCase().toStdString();
    }

    std::optional<juce::Colour> fromHexString(const std::string &s) const
    {
        std::string str = s;
        if (!str.empty() && str[0] == '#')
            str = str.substr(1);
        if (!includeAlpha && str.size() == 6)
            str = "ff" + str;
        if (str.size() != 8)
            return std::nullopt;
        for (char ch : str)
            if (!std::isxdigit((unsigned char)ch))
                return std::nullopt;
        return juce::Colour::fromString(str);
    }

    void openPickerFor(int idx, juce::Rectangle<int> screenBounds)
    {
        if (idx < 0 || idx >= (int)entries.size())
            return;

        // Remove listener from any existing picker before launching a new one
        if (activePicker)
            activePicker->removeChangeListener(this);

        activePickerIdx = idx;

        int flags = juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders |
                    juce::ColourSelector::showColourspace;
        if (!includeAlpha)
            flags |= juce::ColourSelector::editableColour;

        auto picker = std::make_unique<juce::ColourSelector>(flags, 4, 0);
        picker->setCurrentColour(entries[idx].color);
        picker->setSize(300, 380);
        picker->addChangeListener(this);

        activePicker = picker.get();

        juce::CallOutBox::launchAsynchronously(std::move(picker), screenBounds, nullptr);
    }

    void updateEntry(int idx, juce::Colour c)
    {
        entries[idx].color = c;
        onColorChanged(entries[idx].tag, c);
        if (onAnyColorChanged)
            onAnyColorChanged();
        listView->refresh(true);
    }

    // -------------------------------------------------------------------------
    // Private inner struct: RowComponent
    // -------------------------------------------------------------------------
    struct RowComponent : juce::Component
    {
        ColorEditor &owner;
        int rowIdx{-1};
        bool swatchHovered{false};
        std::unique_ptr<components::Label> nameLabel;
        std::unique_ptr<components::TextEditor> hexField;

        RowComponent(ColorEditor &o) : owner(o)
        {
            nameLabel = std::make_unique<components::Label>();
            nameLabel->setJustification(juce::Justification::centredLeft);
            addAndMakeVisible(*nameLabel);

            hexField = std::make_unique<components::TextEditor>();
            hexField->setJustification(juce::Justification::centredLeft);
            hexField->onReturnKey = [this]() { commitHex(); };
            hexField->onFocusLost = [this]() { commitHex(); };
            addAndMakeVisible(*hexField);
        }

        void assign(int idx)
        {
            rowIdx = idx;
            if (rowIdx >= 0 && rowIdx < (int)owner.entries.size())
            {
                nameLabel->setText(owner.entries[rowIdx].tag);
                hexField->setText(owner.toHexString(owner.entries[rowIdx].color), false);
            }
            repaint();
        }

        void resized() override
        {
            auto b = getLocalBounds();
            nameLabel->setBounds(b.withTrimmedLeft(swatchWidth + 4).withWidth(labelWidth));
            int x = swatchWidth + 4 + labelWidth + 4;
            hexField->setBounds(b.withTrimmedLeft(x).withWidth(hexFieldWidth).reduced(1, 0));
        }

        void paint(juce::Graphics &g) override
        {
            if (rowIdx < 0 || rowIdx >= (int)owner.entries.size())
                return;

            auto &entry = owner.entries[rowIdx];

            // Swatch — colour drawn directly, border tracks hover state
            auto sr = swatchRect();
            g.setColour(entry.color);
            g.fillRect(sr);
            if (swatchHovered)
            {
                g.setColour(juce::Colours::white);
                g.drawRect(sr, 2);
            }
            else
            {
                g.setColour(juce::Colours::grey);
                g.drawRect(sr, 1);
            }
            // Name is rendered by nameLabel (jcmp::Label), which uses the stylesheet
            // label colour and font automatically.
        }

        juce::Rectangle<int> swatchRect() const
        {
            return getLocalBounds().reduced(1, 2).withWidth(swatchWidth).reduced(0, 1);
        }

        void mouseMove(const juce::MouseEvent &e) override
        {
            bool over = swatchRect().contains(e.getPosition());
            if (over != swatchHovered)
            {
                swatchHovered = over;
                repaint();
            }
        }

        void mouseEnter(const juce::MouseEvent &e) override { mouseMove(e); }

        void mouseExit(const juce::MouseEvent &) override
        {
            if (swatchHovered)
            {
                swatchHovered = false;
                repaint();
            }
        }

        void mouseDown(const juce::MouseEvent &e) override
        {
            if (swatchRect().contains(e.getPosition()) && rowIdx >= 0)
            {
                auto sr = swatchRect();
                owner.openPickerFor(rowIdx, sr.translated(getScreenX(), getScreenY()));
            }
        }

        void parentHierarchyChanged() override
        {
            // RowComponents are created lazily by ListView and inserted into the tree
            // *after* setStyle has already propagated from the ColorEditor. Walk up to
            // the nearest StyleConsumer ancestor and push the stylesheet down to our
            // styled children so they pick up the theme colours.
            for (auto *p = getParentComponent(); p; p = p->getParentComponent())
            {
                if (auto *sc = dynamic_cast<style::StyleConsumer *>(p))
                {
                    if (auto s = sc->style())
                    {
                        nameLabel->setStyle(s);
                        hexField->setStyle(s);
                        return;
                    }
                }
            }
        }

        void commitHex()
        {
            if (rowIdx < 0 || rowIdx >= (int)owner.entries.size())
                return;
            auto result = owner.fromHexString(hexField->getText().toStdString());
            if (result.has_value())
                owner.updateEntry(rowIdx, *result);
            else
                hexField->setText(owner.toHexString(owner.entries[rowIdx].color), false);
        }
    };
};

// -----------------------------------------------------------------------------
// ColorEditorModal — wraps ColorEditor as an in-app modal overlay
// -----------------------------------------------------------------------------
struct ColorEditorModal : ModalBase
{
    ColorEditorModal(std::vector<ColorEditor::ColorEntry> ents,
                     ColorEditor::ColorChangedFn callback, bool includeAlpha = false)
    {
        editor = std::make_unique<ColorEditor>(std::move(ents), std::move(callback), includeAlpha);
        addAndMakeVisible(*editor);
    }

    juce::Point<int> innerContentSize() override { return {500, 480}; }

    void resized() override
    {
        ModalBase::resized();
        editor->setBounds(getContentArea());
    }

  private:
    std::unique_ptr<ColorEditor> editor;
};

} // namespace sst::jucegui::screens

#endif // INCLUDE_SST_JUCEGUI_SCREENS_COLOREDITOR_H
