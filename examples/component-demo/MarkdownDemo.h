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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_MARKDOWNDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_MARKDOWNDEMO_H

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/components/Viewport.h>
#include <sst/jucegui/markdown/MarkdownComponent.h>
#include "ExampleUtils.h"

struct MarkdownDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Markdown";

    // A scrollable markdown surface: a Viewport whose viewed component is the
    // MarkdownComponent. The component auto-sizes its height; we track its
    // width to the viewport.
    struct MarkdownHolder : juce::Component
    {
        std::unique_ptr<sst::jucegui::components::Viewport> viewport;
        std::unique_ptr<sst::jucegui::markdown::MarkdownComponent> markdown;

        MarkdownHolder()
        {
            markdown = std::make_unique<sst::jucegui::markdown::MarkdownComponent>();
            markdown->setMarkdown(loadDocument());

            viewport = std::make_unique<sst::jucegui::components::Viewport>();
            viewport->setViewedComponent(markdown.get(), false);
            viewport->setScrollBarsShown(true, false);
            addAndMakeVisible(*viewport);
        }

        void resized() override
        {
            viewport->setBounds(getLocalBounds());
            static constexpr int scrollBarRoom = 10;
            auto w = std::max(50, getWidth() - scrollBarRoom);
            markdown->setSize(w, markdown->getHeightForWidth(w));
        }

        static std::string loadDocument()
        {
#ifdef SST_JUCEGUI_MARKDOWN_DEMO_FILE
            auto f = juce::File(SST_JUCEGUI_MARKDOWN_DEMO_FILE);
            if (f.existsAsFile())
                return f.loadFileAsString().toStdString();
#endif
            return sampleMarkdown();
        }

        static std::string sampleMarkdown()
        {
            return R"MD(# Markdown Demo

This is the built-in fallback document, shown when the test document could not
be found on disk. It exercises **bold**, *italic*, `inline code`, ~~strike~~ and a
[link](https://github.com/surge-synthesizer/sst-jucegui).

## Lists

- First bullet
- Second bullet with a longer line so that we can watch the greedy word wrap do
  its thing across the available width
  - A nested bullet
- Third bullet

1. Ordered one
2. Ordered two

> A block quote, which gets its own left rule and indent.

## Code

```cpp
juce::Font fontFor(uint8_t style, const Theme &t) {
    juce::Font f = (style & Code) ? t.mono : t.body;
    return f;
}
```

---

That's all.
)MD";
        }
    };

    MarkdownDemo()
    {
        panel = std::make_unique<sst::jucegui::components::NamedPanel>("Markdown Renderer");
        panel->setContentAreaComponent(std::make_unique<MarkdownHolder>());
        addAndMakeVisible(*panel);
    }

    void resized() override { panel->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panel;
};

#endif // SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_MARKDOWNDEMO_H
