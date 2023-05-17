/*
 * sst-juce-guil - an open source library of juce widgets
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_COMPONENTBASE_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_COMPONENTBASE_H

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/data/Continuous.h>

namespace sst::jucegui::components
{
template <typename T> struct EditableComponentBase
{
    EditableComponentBase() = default;

    std::function<void(void)> onBeginEdit = []() {};
    std::function<void(void)> onEndEdit = []() {};
    std::function<void(const juce::ModifierKeys &m)> onPopupMenu = [](const juce::ModifierKeys &m) {
    };

    T *asT() { return static_cast<T *>(this); }

    virtual void startHover()
    {
        isHovered = true;
        asT()->repaint();
    }
    virtual void endHover()
    {
        isHovered = false;
        asT()->repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditableComponentBase<T>);

  protected:
    bool isHovered{false};
};

template <typename T> struct Modulatable : public data::Continuous::DataListener
{
    Modulatable() = default;

    virtual ~Modulatable()
    {
        if (source)
            source->removeGUIDataListener(this);
    }

    T *asT() { return static_cast<T *>(this); }

    enum ModulationDisplay
    {
        NONE,
        FROM_ACTIVE,
        FROM_OTHER
    };

    void setModulationDisplay(const ModulationDisplay &d)
    {
        modulationDisplay = d;
        asT()->repaint();
    }

    void setEditingModulation(const bool &b)
    {
        isEditingMod = b;
        asT()->repaint();
    }
    void setSource(data::ContinunousModulatable *s)
    {
        if (source)
            source->removeGUIDataListener(this);
        source = s;
        if (source)
            source->addGUIDataListener(this);
        asT()->repaint();
    }

    void dataChanged() override { asT()->repaint(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Modulatable<T>);

  protected:
    data::ContinunousModulatable *source{nullptr};
    bool isEditingMod{false};
    ModulationDisplay modulationDisplay{NONE};
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_COMPONENTBASE_H
