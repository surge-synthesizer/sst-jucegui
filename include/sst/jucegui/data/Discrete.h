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

#ifndef INCLUDE_SST_JUCEGUI_DATA_DISCRETE_H
#define INCLUDE_SST_JUCEGUI_DATA_DISCRETE_H

#include <string>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include "Labeled.h"

namespace sst::jucegui::data
{
struct Discrete : public Labeled
{
    virtual ~Discrete()
    {
        supressListenerModification = true;
        for (auto *l : guilisteners)
        {
            l->sourceVanished(this);
        }
        supressListenerModification = false;
    }

    struct DataListener
    {
        virtual ~DataListener() = default;
        // FIXME - in the future we may want this more fine grained
        virtual void dataChanged() = 0;
        virtual void sourceVanished(Discrete *) = 0;
    };
    bool supressListenerModification{false};
    void addGUIDataListener(DataListener *l) { guilisteners.insert(l); }
    void removeGUIDataListener(DataListener *l)
    {
        if (!supressListenerModification)
            guilisteners.erase(l);
    }
    void addModelDataListener(DataListener *l) { modellisteners.insert(l); }
    void removeModelDataListener(DataListener *l) { modellisteners.erase(l); }

    virtual int getValue() const = 0;
    virtual int getDefaultValue() const { return getMin(); }
    virtual void setValueFromGUI(const int &f) = 0;
    virtual void setValueFromModel(const int &f) = 0;

    virtual std::string getValueAsStringFor(int i) const { return std::to_string(i); }
    virtual std::string getValueAsString() const { return getValueAsStringFor(getValue()); }
    virtual void setValueAsString(const std::string &s) { setValueFromGUI(std::atof(s.c_str())); }

    virtual int getMin() const { return 0; }
    virtual int getMax() const { return 1; }

    bool jogWrapsAtEnd{true};
    virtual void setJogWrapsAtEnd(bool b) { jogWrapsAtEnd = b; }

    virtual void jog(int dir)
    {
        int v = getValue() + dir;
        if (jogWrapsAtEnd)
        {
            if (v < getMin())
                v = getMax();
            if (v > getMax())
                v = getMin();
        }
        else
        {
            v = std::clamp(v, getMin(), getMax());
        }
        setValueFromGUI(v);
    }

  protected:
    std::unordered_set<DataListener *> guilisteners, modellisteners;
};

struct BinaryDiscrete : public Discrete
{
    int getMin() const override { return 0; }
    int getMax() const override { return 1; }
};

struct NamedOptionsDiscrete : public Discrete
{
    std::vector<std::string> options;

    NamedOptionsDiscrete() {}

    void setOptions(const std::vector<std::string> &o)
    {
        options = o;

        for (auto *l : modellisteners)
            l->dataChanged();
        for (auto *l : guilisteners)
            l->dataChanged();
    }

    int getMin() const override { return 0; }
    int getMax() const override { return (int)options.size(); }

    std::string getValueAsStringFor(int i) const override
    {
        auto idx = std::clamp(i, 0, (int)(options.size() - 1));
        return options[idx];
    }
    void setValueAsString(const std::string &s) override
    {
        for (auto idx = 0U; idx < options.size(); idx++)
        {
            if (options[idx] == s)
            {
                setValueFromGUI(idx);
                return;
            }
        }
        return;
    }
};
} // namespace sst::jucegui::data
#endif // SST_JUCEGUI_DISCRETE_H
