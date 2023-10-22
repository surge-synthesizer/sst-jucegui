/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-basic-blocks is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-juce-gui available at
 * https://github.com/surge-synthesizer/sst-juce-gui
 */

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMEDITOR_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMEDITOR_H

#include <sst/jucegui/components/ComponentBase.h>

namespace sst::jucegui::components
{
struct DiscreteParamEditor : public juce::Component,
                             public EditableComponentBase<DiscreteParamEditor>,
                             public data::Discrete::DataListener
{
    ~DiscreteParamEditor()
    {
        if (data)
            data->removeGUIDataListener(this);
    }
    void dataChanged() override { repaint(); }
    void sourceVanished(data::Discrete *d) override
    {
        assert(d == data);
        setSource(nullptr);
    }
    void setSource(data::Discrete *d)
    {
        if (data)
            data->removeGUIDataListener(this);
        data = d;
        if (data)
            data->addGUIDataListener(this);
        repaint();
    }

    void mouseEnter(const juce::MouseEvent &e) override
    {
        hoverX = e.x;
        hoverY = e.y;
        wheel0 = 0.;
        startHover();
        repaint();
    }
    void mouseExit(const juce::MouseEvent &e) override
    {
        endHover();
        repaint();
        wheel0 = 0;
    }

    void mouseMove(const juce::MouseEvent &e) override
    {
        resetTimer(e);
        hoverX = e.x;
        hoverY = e.y;
        repaint();
    }

    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override
    {
        auto thresh = 0.05;
        wheel0 += wheel.deltaY;

        if (wheel0 > thresh)
        {
            if (data)
            {
                onBeginEdit();
                data->jog(-1);
                onEndEdit();
            }
            wheel0 = 0;
        }
        if (wheel0 < -thresh)
        {
            if (data)
            {
                onBeginEdit();
                data->jog(+1);
                onEndEdit();
            }
            wheel0 = 0;
        }
    }

    data::Discrete *data{nullptr};

  protected:
    double wheel0{0};

    float hoverX{0}, hoverY{0};
    bool didPopup{false};
};
} // namespace sst::jucegui::components
#endif // CONDUIT_DISCRETEPARAMEDITOR_H
