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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_COMPONENTBASE_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_COMPONENTBASE_H

#include <functional>
#include <cassert>
#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/data/Continuous.h>

namespace sst::jucegui::components
{
struct WithIdleTimer
{
    WithIdleTimer();
    ~WithIdleTimer();
    void beginTimer()
    {
        registeredItems.insert(this);
        delayInMs = 1000;
    }
    void endTimer() { registeredItems.erase(this); }
    void resetTimer(const juce::MouseEvent &e)
    {
        if (e.x != lx || e.y != ly)
        {
            if (delayInMs <= 0 && onIdleHoverEnd)
            {
                onIdleHoverEnd();
            }
            delayInMs = 1000;
        }
        lx = e.x;
        ly = e.y;
    }
    uint64_t delayInMs{1};

    int lx{0}, ly{0};

    std::function<void()> onIdleHover{nullptr}, onIdleHoverEnd{nullptr};

    static std::unordered_set<WithIdleTimer *> registeredItems;
    static std::unique_ptr<juce::Timer> theTimer;
    static uint64_t timerClients;

    static constexpr int idleTimeMS{100};
};

template <typename T> struct EditableComponentBase : public WithIdleTimer
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
        beginTimer();
        asT()->repaint();
    }
    virtual void endHover()
    {
        isHovered = false;
        endTimer();
        asT()->repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditableComponentBase<T>)

    bool isHovered{false};
};

template <typename T> struct Modulatable : public data::Continuous::DataListener
{
    Modulatable() = default;

    virtual ~Modulatable()
    {
        if (continuous())
        {
            continuous()->removeGUIDataListener(this);
        }
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

    data::Continuous *continuous()
    {
        switch (source.index())
        {
        case 0:
            return std::get<0>(source);
        case 1:
            return std::get<1>(source);
        }
        assert(false);
        return nullptr;
    }
    data::ContinunousModulatable *continuousModulatable()
    {
        if (std::holds_alternative<data::ContinunousModulatable *>(source))
        {
            return std::get<data::ContinunousModulatable *>(source);
        }
        return nullptr;
    }

    template <typename S> void setSource(S *s)
    {
        if (continuous())
            continuous()->removeGUIDataListener(this);
        source = s;
        if (continuous())
            continuous()->addGUIDataListener(this);
        asT()->repaint();
    }

    void clearSource()
    {
        if (continuous())
            continuous()->removeGUIDataListener(this);
        source = (data::ContinunousModulatable *)nullptr;
    }

    void dataChanged() override { asT()->repaint(); }
    void sourceVanished(data::Continuous *s) override
    {
        assert(s == continuous());
        clearSource();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Modulatable<T>)

    std::variant<data::Continuous *, data::ContinunousModulatable *> source{
        (data::Continuous *)nullptr};
    bool isEditingMod{false};
    ModulationDisplay modulationDisplay{NONE};
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_COMPONENTBASE_H
