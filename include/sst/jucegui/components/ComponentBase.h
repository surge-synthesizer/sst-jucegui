//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_COMPONENTBASE_H
#define SST_JUCEGUI_COMPONENTBASE_H

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
        isEditingMod = true;
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
