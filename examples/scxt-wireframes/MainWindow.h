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

#ifndef SSTJUCEGUI_EXAMPLES_SCXT_WIREFRAMES_MAINWINDOW_H
#define SSTJUCEGUI_EXAMPLES_SCXT_WIREFRAMES_MAINWINDOW_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace scxt::wireframe
{
struct MainWindow : public juce::Component
{
    struct DebugRect : public juce::Component
    {
        juce::Colour color;
        std::string label;
        DebugRect(const juce::Colour &c, const std::string &s) : color(c), label(s) {}
        void paint(juce::Graphics &g) override
        {
            g.fillAll(color);
            g.setColour(color.contrasting());
            g.drawText(label, getLocalBounds(), juce::Justification::centred);
        }
    };

    static constexpr int headerHeight = 34;
    static constexpr int sideWidths = 196;
    static constexpr int edgeDistance = 6;

    static constexpr int envHeight = 160, modHeight = 160, fxHeight = 176;
    static constexpr int pad = 2;

    std::unique_ptr<juce::Component> header, browser, parts, mainSection, sample, fx[4], mod, mix,
        eg[2], lfo;
    MainWindow()
    {
        sst::jucegui::style::StyleSheet::initializeStyleSheets([]() {});
        header = std::make_unique<DebugRect>(juce::Colour(240, 240, 100), "Header");
        addAndMakeVisible(*header);
        parts = std::make_unique<DebugRect>(juce::Colour(100, 120, 200), "Parts");
        addAndMakeVisible(*parts);
        mainSection = std::make_unique<DebugRect>(juce::Colour(80, 80, 80), "Main");
        addAndMakeVisible(*mainSection);
        browser = std::make_unique<DebugRect>(juce::Colour(200, 120, 100), "Browser");
        addAndMakeVisible(*browser);
        sample = std::make_unique<DebugRect>(juce::Colour(200, 100, 0), "Sample");
        addAndMakeVisible(*sample);

        for (int i = 0; i < 4; ++i)
        {
            fx[i] = std::make_unique<DebugRect>(juce::Colour(i * 60, 0, (4 - i) * 60),
                                                "FX" + std::to_string(i));
            addAndMakeVisible(*(fx[i]));
        }
        mod = std::make_unique<DebugRect>(juce::Colour(100, 200, 150), "Mod");
        addAndMakeVisible(*mod);
        mix = std::make_unique<DebugRect>(juce::Colour(150, 220, 100), "Mix");
        addAndMakeVisible(*mix);

        for (int i = 0; i < 2; ++i)
        {
            eg[i] = std::make_unique<DebugRect>(juce::Colour(250, 120 + i * 40, 120),
                                                "EG" + std::to_string(i));
            addAndMakeVisible(*(eg[i]));
        }
        lfo = std::make_unique<DebugRect>(juce::Colour(250, 80, 120), "LFO");
        addAndMakeVisible(*lfo);
    }
    void paint(juce::Graphics &g) override { g.fillAll(juce::Colours::black); }
    void resized() override { layout(); }

    void layout()
    {
        header->setBounds(pad, pad, getWidth() - 2 * pad, headerHeight - 2 * pad);
        parts->setBounds(pad, headerHeight + pad, sideWidths, getHeight() - headerHeight - 3 * pad);
        browser->setBounds(getWidth() - sideWidths - pad, headerHeight + pad, sideWidths,
                           getHeight() - headerHeight - 3 * pad);

        auto mainRect = juce::Rectangle<int>(sideWidths + 3 * pad, headerHeight + pad,
                                             getWidth() - 2 * sideWidths - 6 * pad,
                                             getHeight() - headerHeight - 3 * pad);
        mainSection->setBounds(mainRect);

        auto wavHeight = mainRect.getHeight() - envHeight - modHeight - fxHeight;
        sample->setBounds(mainRect.withHeight(wavHeight));

        auto fxRect = mainRect.withTrimmedTop(wavHeight).withHeight(fxHeight);
        auto fw = fxRect.getWidth() * 0.25;
        auto tfr = fxRect.withWidth(fw);
        for (int i = 0; i < 4; ++i)
        {
            fx[i]->setBounds(tfr);
            tfr.translate(fw, 0);
        }

        auto modRect = mainRect.withTrimmedTop(wavHeight + fxHeight).withHeight(modHeight);
        auto mw = modRect.getWidth() * 0.750;
        mod->setBounds(modRect.withWidth(mw));
        auto xw = modRect.getWidth() * 0.250;
        mix->setBounds(modRect.withWidth(xw).translated(mw, 0));

        auto envRect =
            mainRect.withTrimmedTop(wavHeight + fxHeight + modHeight).withHeight(envHeight);
        auto ew = envRect.getWidth() * 0.25;
        eg[0]->setBounds(envRect.withWidth(ew));
        eg[1]->setBounds(envRect.withWidth(ew).translated(ew, 0));
        lfo->setBounds(envRect.withWidth(ew * 2).translated(ew * 2, 0));
    }
};
} // namespace scxt::wireframe
#endif // SST_JUCEGUI_MAINWINDOW_H
