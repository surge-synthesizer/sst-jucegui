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

#ifndef INCLUDE_SST_JUCEGUI_MARKDOWN_MARKDOWNCOMPONENT_H
#define INCLUDE_SST_JUCEGUI_MARKDOWN_MARKDOWNCOMPONENT_H

#include <memory>
#include <string>
#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/components/BaseStyles.h>

namespace sst::jucegui::markdown
{
/*
 * MarkdownComponent renders a Markdown document (parsed with the vendored md4c)
 * into a JUCE component. It follows the sst-jucegui stylesheet conventions: the
 * body text uses the shared BaseLabel font/colour (the same as Label), and the
 * component adds properties for its code font, code background, links, headings
 * and blockquotes.
 *
 * The component lays a document out top-to-bottom for a fixed width and knows
 * the pixel height it needs (getHeightForWidth). By default it will resize its
 * own height to fit its content, which makes it convenient to drop inside a
 * sst::jucegui::components::Viewport.
 */
struct MarkdownComponent : public juce::Component,
                           public style::StyleConsumer,
                           public style::SettingsConsumer
{
    struct Styles : components::base_styles::Base,
                    components::base_styles::BaseLabel,
                    components::base_styles::Outlined
    {
        SCLASS(markdown);

        PROP_FONT(codefont);  // monospace font for code spans / blocks
        PROP(codebackground); // fill behind code spans and code blocks
        PROP(codetext);       // text colour inside code
        PROP(link);           // link text colour
        PROP(heading);        // heading text colour
        PROP(quotebar);       // blockquote left-rule colour

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(components::base_styles::Base::styleClass)
                .withBaseClass(components::base_styles::BaseLabel::styleClass)
                .withBaseClass(components::base_styles::Outlined::styleClass)
                .withProperty(codefont)
                .withProperty(codebackground)
                .withProperty(codetext)
                .withProperty(link)
                .withProperty(heading)
                .withProperty(quotebar);
        }
    };

    MarkdownComponent();
    ~MarkdownComponent();

    // Set the document. Triggers a relayout.
    void setMarkdown(const std::string &markdown);
    const std::string &getRawMarkdown() const { return rawMarkdown; }

    // Pixel height needed to render the current document at the given width.
    // Lays out on demand; needs a resolved style to be meaningful.
    int getHeightForWidth(int width);

    // When true (the default) the component sets its own height to fit its
    // content whenever it is laid out, so it can live inside a Viewport whose
    // width tracks the viewport and whose height scrolls.
    void setAutoSizeHeight(bool b) { autoSizeHeight = b; }

    // Called when a link is clicked. If unset, http(s) links are opened in the
    // default browser and other hrefs are ignored.
    std::function<void(const std::string &href)> onLinkClick{nullptr};

    // Read-only text selection: drag to select, double/triple-click for word/
    // line, Cmd/Ctrl-A to select all, Cmd/Ctrl-C to copy. Copies rendered plain
    // text (no Markdown syntax, no list markers).
    void selectAll();
    void copySelectionToClipboard();
    std::string getSelectedText() const;

    void paint(juce::Graphics &g) override;
    void resized() override;
    void onStyleChanged() override;

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseMove(const juce::MouseEvent &e) override;
    void mouseExit(const juce::MouseEvent &e) override;
    bool keyPressed(const juce::KeyPress &key) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkdownComponent)

  private:
    void relayoutIfNeeded();
    void doLayout(int forWidth);
    const juce::String *linkHrefAt(juce::Point<float> p) const;
    bool pointIsOnText(juce::Point<float> p) const;

    std::string rawMarkdown;
    bool autoSizeHeight{true};
    bool layoutDirty{true};

    struct Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace sst::jucegui::markdown

#endif // INCLUDE_SST_JUCEGUI_MARKDOWN_MARKDOWNCOMPONENT_H
