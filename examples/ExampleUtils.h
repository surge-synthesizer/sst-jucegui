//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_EXAMPLEUTILS_H
#define SST_JUCEGUI_EXAMPLEUTILS_H

#include <juce_gui_basics/juce_gui_basics.h>

struct Solid : public juce::Component
{
    juce::Colour color;
    Solid(const juce::Colour &c) : color(c) {}
    void paint(juce::Graphics &g) override
    {
        if (isHover)
            g.fillAll(color);
    }
    void mouseEnter(const juce::MouseEvent &) override
    {
        isHover = true;
        repaint();
    }
    void mouseExit(const juce::MouseEvent &) override
    {
        isHover = false;
        repaint();
    }
    bool isHover{false};
};

#endif // SST_JUCEGUI_EXAMPLEUTILS_H
