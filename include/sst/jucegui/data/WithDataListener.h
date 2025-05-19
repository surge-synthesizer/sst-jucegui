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

#ifndef INCLUDE_SST_JUCEGUI_DATA_WITHDATALISTENER_H
#define INCLUDE_SST_JUCEGUI_DATA_WITHDATALISTENER_H

#include <cassert>
#include <unordered_set>

namespace sst::jucegui::data
{
template <typename T> struct WithDataListener
{

    struct DataListener
    {
        virtual ~DataListener() = default;
        // FIXME - in the future we may want this more fine grained
        virtual void dataChanged() = 0;
        virtual void sourceVanished(T *) = 0;
    };
    bool supressListenerModification{false};
    void addGUIDataListener(DataListener *l)
    {
        assert(!supressListenerModification);
        if (!supressListenerModification)
            guilisteners.insert(l);
    }
    void removeGUIDataListener(DataListener *l)
    {
        if (!supressListenerModification)
            guilisteners.erase(l);
    }
    void addModelDataListener(DataListener *l) { modellisteners.insert(l); }
    void removeModelDataListener(DataListener *l) { modellisteners.erase(l); }

  protected:
    std::unordered_set<DataListener *> guilisteners, modellisteners;
};
} // namespace sst::jucegui::data
#endif // WITHDATALISTENER_H
