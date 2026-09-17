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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEJOGLABEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEJOGLABEL_H

#include <functional>
#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/components/MenuButton.h>

namespace sst::jucegui::components
{
/*
 * The name of the thing you are looking at, edited in place by clicking it, with
 * optional jog arrows for stepping to whatever comes before or after it. An
 * unchanging prefix ("Part 3:") can sit ahead of the editable part.
 *
 * It wears the menubutton style so it sits where one used to.
 */
struct NameJogLabel : juce::Component,
                      style::StyleConsumer,
                      style::SettingsConsumer,
                      juce::TextEditor::Listener
{
    using Styles = MenuButtonPainter<MenuButton>::Styles;

    NameJogLabel();
    ~NameJogLabel();

    void setPrefix(const std::string &);
    void setLabel(const std::string &);
    const std::string &getLabel() const { return name; }

    // both arrows are live together: there is nowhere to step in a folder of one
    void setJogEnabled(bool);
    bool getJogEnabled() const { return jogEnabled; }

    std::function<void(const std::string &)> onRename{nullptr};
    std::function<void(int)> onJog{nullptr}; // -1 previous, +1 next
    std::function<void()> onPopupMenu{nullptr};
    // the click that starts an edit may mean something to the owner too
    std::function<void()> onBeforeRename{nullptr};

    void beginRename();

    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseMove(const juce::MouseEvent &) override;
    void mouseEnter(const juce::MouseEvent &) override;
    void mouseExit(const juce::MouseEvent &) override;

    void textEditorReturnKeyPressed(juce::TextEditor &) override;
    void textEditorEscapeKeyPressed(juce::TextEditor &) override;
    void textEditorFocusLost(juce::TextEditor &) override;

  protected:
    static constexpr int jogWidth{18};

    std::string prefix, name;
    bool jogEnabled{false};
    int hoveredJog{0}; // -1 up, +1 down, 0 neither
    std::unique_ptr<juce::TextEditor> renameEditor;

    juce::Rectangle<int> jogArea() const;
    juce::Rectangle<int> textArea() const;
    void restyleEditor();
    // the prefix stays on show while the name is edited
    int prefixWidth();
    juce::Rectangle<int> editorArea();
    void commitRename();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NameJogLabel);
};
} // namespace sst::jucegui::components
#endif // INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEJOGLABEL_H
