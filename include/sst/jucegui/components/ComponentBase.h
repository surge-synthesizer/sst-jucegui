//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_COMPONENTBASE_H
#define SST_JUCEGUI_COMPONENTBASE_H

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::components
{
template <typename T> struct EditableComponentBase
{
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

  protected:
    bool isHovered{false};
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_COMPONENTBASE_H
