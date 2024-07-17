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

#ifndef INCLUDE_SST_JUCEGUI_LAYOUTS_EXPLICITLAYOUT_H
#define INCLUDE_SST_JUCEGUI_LAYOUTS_EXPLICITLAYOUT_H

#include <juce_gui_basics/juce_gui_basics.h>
namespace sst::jucegui::layout
{
struct ExplicitLayout
{
    static constexpr int labelHeight{18};
    static constexpr int toggleSize{14};

    ExplicitLayout() {}

    struct NamedPosition
    {
        NamedPosition() {}
        NamedPosition(const std::string &n) : name(n) {}

        NamedPosition at(const juce::Rectangle<float> &t)
        {
            auto res = *this;
            res.rect = t;
            return res;
        }

        // Fractionally scale the 0...1 x and 0...h/w y
        NamedPosition scaled(const juce::Rectangle<int> &into, float x, float y, float w, float h)
        {
            auto res = *this;
            res.rect = juce::Rectangle<float>(into.getX() + x * into.getWidth(),
                                              into.getY() + y * into.getWidth(),
                                              w * into.getWidth(), h * into.getWidth());
            return res;
        }
        NamedPosition scaled(const juce::Rectangle<int> &into, float x, float y, float w)
        {
            return scaled(into, x, y, w, w);
        }

        std::string name;
        juce::Rectangle<float> rect{};
    };
    void addNamedPosition(const NamedPosition &n) { positions[n.name] = n; }
    void addNamedPositionAndLabel(const NamedPosition &n)
    {
        addNamedPosition(n);
        addLabelPositionTo(n.name);
    }
    void addPowerButtonPositionTo(const std::string &name, int w = toggleSize, int off = -1)
    {
        auto op = positionFor(name);
        auto oh = op.getHeight();
        op = op.withTrimmedLeft(op.getWidth() - w).withTrimmedBottom(op.getHeight() - w);
        if (off < 0)
            off = w / 4;
        op = op.translated(off, -off);
        addNamedPosition(NamedPosition(name + ".power").at(op.toFloat()));
    }
    void addLabelPositionTo(const std::string &name)
    {
        auto op = positionFor(name);
        auto oh = op.getHeight();
        op = op.withBottom(op.getBottom() + 18).withTrimmedTop(oh);
        addNamedPosition(NamedPosition(name + ".label").at(op.toFloat()));
    }

    juce::Rectangle<int> positionFor(const std::string &s) const
    {
        auto p = positions.find(s);
        if (p == positions.end())
        {
            std::cout << "Unable to find name " << s << std::endl;
            assert(false);
            return {};
        }
        return p->second.rect.toNearestIntEdges();
    }

    juce::Rectangle<int> labelPositionFor(const std::string &s) const
    {
        return positionFor(s + ".label");
    }

    juce::Rectangle<int> powerButtonPositionFor(const std::string &s) const
    {
        return positionFor(s + ".power");
    }

    std::unordered_map<std::string, NamedPosition> positions;
};
} // namespace sst::jucegui::layout

#endif // SHORTCIRCUITXT_EXPLICITLAYOUT_H
