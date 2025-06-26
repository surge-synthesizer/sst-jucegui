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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_ALERTORPROMPT_H
#define INCLUDE_SST_JUCEGUI_SCREENS_ALERTORPROMPT_H

#include "ModalBase.h"

#include "../components/Label.h"
#include "../components/TextPushButton.h"

namespace sst::jucegui::screens
{
struct AlertOrPrompt : ModalBase
{
    std::optional<std::string> title, message;
    std::function<void()> onOK{nullptr}, onCancel{nullptr}, onApply{nullptr};

    static std::unique_ptr<AlertOrPrompt> Alert(const std::string &title, const std::string message)
    {
        auto al = std::make_unique<sst::jucegui::screens::AlertOrPrompt>();
        al->title = title;
        al->message = message;
        al->closeOnOK();
        al->prepare();
        return al;
    }

    static std::unique_ptr<AlertOrPrompt> OKCancel(const std::string &title,
                                                   const std::string message,
                                                   const std::function<void()> &onOK,
                                                   const std::function<void()> &onCancel)
    {
        auto al = std::make_unique<sst::jucegui::screens::AlertOrPrompt>();
        al->title = title;
        al->message = message;
        al->onOK = onOK;
        al->onCancel = onCancel;

        al->prepare();
        return al;
    }

    void prepare()
    {
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

        if (onOK)
        {
            ok = std::make_unique<components::TextPushButton>();
            ok->setLabel("OK");
            ok->setOnCallback([oo = onOK, w = juce::Component::SafePointer(this)]() {
                oo();
                if (w && w->isVisible())
                    w->setVisible(false);
            });
            addAndMakeVisible(*ok);
        }
        if (onCancel)
        {
            cancel = std::make_unique<components::TextPushButton>();
            cancel->setLabel("Cancel");
            cancel->setOnCallback([oo = onCancel, w = juce::Component::SafePointer(this)]() {
                oo();
                if (w && w->isVisible())
                    w->setVisible(false);
            });
            addAndMakeVisible(*cancel);
        }
        if (onApply)
        {
            apply = std::make_unique<components::TextPushButton>();
            apply->setLabel("Apply");
            apply->setOnCallback(onApply);
            addAndMakeVisible(*apply);
        }
    }

    void closeOnOK()
    {
        onOK = [this]() { setVisible(false); };
    }

    void mouseDown(const juce::MouseEvent &event) override
    {
        if (!onOK && !onCancel)
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

    static constexpr int titleSize{18}, margin{4}, buttonWidth{80};
    void resized() override
    {
        if (!prepared)
            prepare();

        ModalBase::resized();
        auto p = getContentArea().reduced(4);
        if (titleL)
            titleL->setBounds(p.withHeight(titleSize));
        if (messageL)
            messageL->setBounds(p.withTrimmedTop(title ? (titleSize + 2 * margin) : 0)
                                    .withTrimmedBottom(titleSize + 4));

        auto buttonBox = p.withTrimmedTop(p.getHeight() - titleSize)
                             .withWidth(buttonWidth)
                             .translated(p.getWidth() - buttonWidth, 0);
        if (apply)
        {
            apply->setBounds(buttonBox);
            buttonBox = buttonBox.translated(-buttonWidth - margin, 0);
        }
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

    // TODO: Consider dynamic sizing here
    juce::Point<int> innerContentSize() override { return {400, 220}; }

  private:
    bool prepared{false};
    std::unique_ptr<components::Label> titleL, messageL;
    std::unique_ptr<components::TextPushButton> ok, cancel, apply;
};
} // namespace sst::jucegui::screens

#endif // ALERTORPROMPT_H
