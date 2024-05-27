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

#include <sst/jucegui/components/NamedPanelDivider.h>
#include <cassert>

namespace sst::jucegui::components
{

NamedPanelDivider::NamedPanelDivider() : style::StyleConsumer(Styles::styleClass) {}

NamedPanelDivider::~NamedPanelDivider() {}

void NamedPanelDivider::paint(juce::Graphics &g)
{
    g.setColour(getColour(Styles::outline));

    auto divH = 2;
    auto margin = 10;
    auto innerMargin = 5;
    auto r = getLocalBounds().toFloat().withSizeKeepingCentre(getWidth() - 10, divH);
    auto ls = r.withWidth(r.getWidth() / 2 - innerMargin / 2);
    g.fillRoundedRectangle(ls, divH / 2);
    ls = ls.translated(r.getWidth() / 2 + innerMargin / 2, 0);
    g.fillRoundedRectangle(ls, divH / 2);
}
} // namespace sst::jucegui::components