//
// Created by Paul Walker on 5/31/22.
//

#ifndef SST_JUCEGUI_LAYOUTS_H
#define SST_JUCEGUI_LAYOUTS_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

namespace sst::jucegui::style
{
template <typename T> struct EvenDivision
{
    enum Orientation
    {
        HORIZONTAL,
        VERTICAL
    };
    typedef std::vector<juce::Rectangle<T>> store_t;
    store_t store;
    EvenDivision(const juce::Rectangle<T> &divide, int into, const Orientation &thisway)
    {
        jassert(into > 0);
        if (thisway == HORIZONTAL)
        {
            auto w = divide.getWidth() / into;
            auto q = divide.withWidth(w);
            for (int i = 0; i < into; ++i)
            {
                store.push_back(q);
                q = q.translated(w, 0);
            }
        }
        else
        {
            auto h = divide.getHeight() / into;
            auto q = divide.withHeight(h);
            for (int i = 0; i < into; ++i)
            {
                store.push_back(q);
                q = q.translated(0, h);
            }
        }
    }

    typedef typename store_t::iterator iterator;
    typedef const typename store_t::const_iterator const_iterator;

    iterator begin() { return store.begin(); }
    const_iterator begin() const { return store.begin(); }
    iterator end() { return store.end(); }
    const_iterator end() const { return store.end(); }
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_LAYOUTS_H
