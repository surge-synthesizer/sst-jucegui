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
        delayInMs = delayUntilIdle;
        idleActive = false;
    }
    void endTimer()
    {
        registeredItems.erase(this);
        // Remember to end the hover if active when we end the timer
        if (idleActive && onIdleHoverEnd)
        {
            onIdleHoverEnd();
        }
        idleActive = false;
    }
    void resetTimer(const juce::MouseEvent &e)
    {
        if (e.x != lx || e.y != ly)
        {
            if ((idleActive || delayInMs <= 0) && onIdleHoverEnd)
            {
                onIdleHoverEnd();
            }
            idleActive = false;

            delayInMs = delayUntilIdle;
        }
        lx = e.x;
        ly = e.y;
    }

    void immediatelyInitiateIdleAction(int endAfter) // endAfter = -1 for no dismiss
    {
        delayInMs = endAfter;
        if (onIdleHover)
            onIdleHover();
        idleActive = true;
        delayToUnIdleInMs = endAfter;
    }
    int64_t delayInMs{1}, delayToUnIdleInMs{0};
    bool idleActive{false};

    int lx{0}, ly{0};

    std::function<void()> onIdleHover{nullptr}, onIdleHoverEnd{nullptr};

    static std::unordered_set<WithIdleTimer *> registeredItems;
    static std::unique_ptr<juce::Timer> theTimer;
    static uint64_t timerClients;

    static constexpr int idleTimeMS{50};
    int delayUntilIdle{1000};
};

template <typename T> struct EditableComponentBase : public WithIdleTimer
{
    EditableComponentBase() = default;

    std::function<void(void)> onBeginEdit = []() {};
    std::function<void(void)> onEndEdit = []() {};

    // Wheel calls begin and end but we may want to know it happened for tooltips etc
    std::function<void(void)> onWheelEditOccurred = []() {};

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

    // Some GCC12 issues with the template and this and we always have a parent with same
    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditableComponentBase<T>)

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
    data::ContinuousModulatable *continuousModulatable()
    {
        if (std::holds_alternative<data::ContinuousModulatable *>(source))
        {
            return std::get<data::ContinuousModulatable *>(source);
        }
        return nullptr;
    }

    template <typename S> void setSource(S *s)
    {
        if (continuous())
            continuous()->removeGUIDataListener(this);
        source = s;
        if (continuous())
        {
            continuous()->addGUIDataListener(this);
            asT()->setTitle(continuous()->getLabel());
        }
        asT()->repaint();
    }

    void clearSource()
    {
        if (continuous())
            continuous()->removeGUIDataListener(this);
        source = (data::ContinuousModulatable *)nullptr;
    }

    void dataChanged() override { asT()->repaint(); }
    void sourceVanished(data::Continuous *s) override
    {
        assert(s == continuous());
        clearSource();
    }

    // Some gcc12 problems with this and we always have a parent class with same
    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Modulatable<T>)

    std::variant<data::Continuous *, data::ContinuousModulatable *> source{
        (data::Continuous *)nullptr};
    bool isEditingMod{false};
    ModulationDisplay modulationDisplay{NONE};
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_COMPONENTBASE_H
