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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_PROMPTFORVALUE_H
#define INCLUDE_SST_JUCEGUI_SCREENS_PROMPTFORVALUE_H

#include "ModalBase.h"

#include "../components/Label.h"
#include "../components/TextEditor.h"
#include "../components/TextPushButton.h"

namespace sst::jucegui::screens
{
struct PromptForValue : ModalBase
{
    std::optional<std::string> title, message, initialValue;
    std::function<void(const std::string &)> onOK{nullptr};
    std::function<void()> onCancel{nullptr};

    static std::unique_ptr<PromptForValue>
    Prompt(const std::string &title, const std::string &message, const std::string &initial,
           const std::function<void(const std::string &)> &onOK,
           const std::function<void()> &onCancel)
    {
        auto pv = std::make_unique<PromptForValue>();
        pv->title = title;
        pv->message = message;
        pv->initialValue = initial;
        pv->onOK = onOK;
        pv->onCancel = onCancel;
        pv->prepare();
        return pv;
    }

    void prepare()
    {
        prepared = true;
        if (title.has_value())
        {
            titleL = std::make_unique<components::Label>();
            titleL->setText(*title);
            titleL->setFontHeightOverride(18);
            addAndMakeVisible(*titleL);
        }

        if (message.has_value())
        {
            messageL = std::make_unique<components::Label>();
            messageL->setText(*message);
            messageL->setJustification(juce::Justification::topLeft);
            messageL->setIsMultiline(true);
            addAndMakeVisible(*messageL);
        }

        editor = std::make_unique<components::TextEditor>();
        if (initialValue.has_value())
            editor->setAllText(*initialValue);
        editor->onReturnKey = [w = juce::Component::SafePointer(this)]() {
            if (w)
                w->commitOK();
        };
        editor->onEscapeKey = [w = juce::Component::SafePointer(this)]() {
            if (w)
                w->commitCancel();
        };
        addAndMakeVisible(*editor);

        if (onOK)
        {
            ok = std::make_unique<components::TextPushButton>();
            ok->setLabel("OK");
            ok->setOnCallback([w = juce::Component::SafePointer(this)]() {
                if (w)
                    w->commitOK();
            });
            addAndMakeVisible(*ok);
        }
        if (onCancel)
        {
            cancel = std::make_unique<components::TextPushButton>();
            cancel->setLabel("Cancel");
            cancel->setOnCallback([w = juce::Component::SafePointer(this)]() {
                if (w)
                    w->commitCancel();
            });
            addAndMakeVisible(*cancel);
        }
    }

    void commitOK()
    {
        if (onOK)
            onOK(editor ? editor->getText().toStdString() : std::string{});
        if (isVisible())
            setVisible(false);
    }

    void commitCancel()
    {
        if (onCancel)
            onCancel();
        if (isVisible())
            setVisible(false);
    }

    void paintContents(juce::Graphics &g) override
    {
        auto p = getContentArea().reduced(4);
        if (title)
        {
            g.setColour(getColour(Styles::brightoutline));
            g.drawHorizontalLine(p.getY() + titleSize + margin, p.getX(), p.getRight());
        }
    }

    void visibilityChanged() override
    {
        if (isVisible() && editor)
        {
            editor->grabKeyboardFocus();
            editor->selectAll();
        }
    }

    static constexpr int titleSize{18}, margin{4}, buttonWidth{80}, editorHeight{24};
    void resized() override
    {
        if (!prepared)
            prepare();

        ModalBase::resized();
        auto p = getContentArea().reduced(4);
        if (titleL)
            titleL->setBounds(p.withHeight(titleSize));

        auto buttonRow = p.withHeight(titleSize).translated(0, p.getHeight() - titleSize);
        auto editorRow = p.withHeight(editorHeight)
                             .translated(0, buttonRow.getY() - editorHeight - 2 * margin - p.getY())
                             .reduced(20, 0);

        auto messageTop = title ? (titleSize + 2 * margin) : 0;
        auto messageBottomTrim = p.getBottom() - editorRow.getY() + margin;
        if (messageL)
            messageL->setBounds(p.withTrimmedTop(messageTop).withTrimmedBottom(messageBottomTrim));

        if (editor)
            editor->setBounds(editorRow);

        auto buttonBox = buttonRow.withWidth(buttonWidth).translated(p.getWidth() - buttonWidth, 0);
        if (cancel)
        {
            cancel->setBounds(buttonBox);
            buttonBox = buttonBox.translated(-buttonWidth - margin, 0);
        }
        if (ok)
        {
            ok->setBounds(buttonBox);
        }
    }

    juce::Point<int> innerContentSize() override { return {300, 170}; }

    void onStyleChanged() override
    {
        if (editor)
            editor->onStyleChanged();
        ModalBase::onStyleChanged();
    }

  private:
    bool prepared{false};
    std::unique_ptr<components::Label> titleL, messageL;
    std::unique_ptr<components::TextEditor> editor;
    std::unique_ptr<components::TextPushButton> ok, cancel;
};
} // namespace sst::jucegui::screens

#endif // PROMPTFORVALUE_H
