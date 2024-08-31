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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_ZOOMCONTAINER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_ZOOMCONTAINER_H

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ScrollBar.h"

namespace sst::jucegui::components
{
struct ZoomContainerClient
{
    virtual ~ZoomContainerClient() = default;
    virtual juce::Component *associatedComponent() = 0;
    virtual bool supportsVerticalZoom() const = 0;
    virtual bool supportsHorizontalZoom() const = 0;
    virtual void setHorizontalZoom(float pctStart, float zoomFactor) {}
    virtual void setVerticalZoom(float pctStart, float zoomFactor) {}
};
struct ZoomContainer : juce::Component, juce::ScrollBar::Listener
{
    std::unique_ptr<ScrollBar> vScroll, hScroll;
    std::unique_ptr<ZoomContainerClient> contents;
    ZoomContainer(std::unique_ptr<ZoomContainerClient> &&c) : contents(std::move(c))
    {
        if (contents->supportsHorizontalZoom())
        {
            hScroll = std::make_unique<ScrollBar>(false);
            hScroll->setAutoHide(false);
            hScroll->setRangeLimits(0.0, 1.0, juce::NotificationType::dontSendNotification);
            hScroll->addListener(this);
            addAndMakeVisible(*hScroll);
        }
        if (contents->supportsVerticalZoom())
        {
            vScroll = std::make_unique<ScrollBar>(true);
            vScroll->setAutoHide(false);
            vScroll->setRangeLimits(0.0, 1.0, juce::NotificationType::dontSendNotification);
            vScroll->addListener(this);
            addAndMakeVisible(*vScroll);
        }
        addAndMakeVisible(*(contents->associatedComponent()));
    }

    static constexpr int sbw{6};

    void resized() override
    {
        auto bx = getLocalBounds();

        if (hScroll && vScroll)
        {
            auto hb = bx.withTrimmedTop(bx.getHeight() - sbw).withTrimmedRight(sbw);
            auto vb = bx.withTrimmedLeft(bx.getWidth() - sbw).withTrimmedBottom(sbw);
            bx = bx.withTrimmedBottom(sbw).withTrimmedRight(sbw);
            hScroll->setBounds(hb);
            vScroll->setBounds(vb);
        }
        else if (hScroll)
        {
            auto hb = bx.withTrimmedTop(bx.getHeight() - sbw);
            bx = bx.withTrimmedBottom(sbw);
            hScroll->setBounds(hb);
        }
        else if (vScroll)
        {
            auto vb = bx.withTrimmedLeft(bx.getWidth() - sbw);
            bx = bx.withTrimmedRight(sbw);
            vScroll->setBounds(vb);
        }
        contents->associatedComponent()->setBounds(bx);
    }

    void mouseMagnify(const juce::MouseEvent &event, float scaleFactor) override
    {
        if (event.mods.isShiftDown())
        {
            adjustVerticalZoom(event.position, scaleFactor);
        }
        else
        {
            adjustHorizontalZoom(event.position, scaleFactor);
        }
        Component::mouseMagnify(event, scaleFactor);
    }

    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override
    {
        if (event.mods.isShiftDown() && event.mods.isAltDown())
        {
            // HZOM by -Delta Y
            if (contents->supportsHorizontalZoom())
            {
                adjustHorizontalZoom(event.position, 1.0 - wheel.deltaY);
            }
            return;
        }
        if (event.mods.isShiftDown())
        {
            // VZoom by delta Y
            if (contents->supportsVerticalZoom())
            {
                adjustVerticalZoom(event.position, 1.0 - wheel.deltaY);
            }
            return;
        }

        // Assume a 2D mouse so do what you think. This is just a joy on
        // a mac trackpad!
        if (fabs(wheel.deltaX) > fabs(wheel.deltaY))
        {
            if (hScroll)
            {
                auto dy = wheel.deltaX;
                auto rs = hScroll->getCurrentRangeStart();
                rs = std::clamp(rs - dy, 0., 1.);
                hScroll->setCurrentRangeStart(rs);
            }
        }
        else
        {
            if (vScroll)
            {
                auto dy = wheel.deltaY;
                auto rs = vScroll->getCurrentRangeStart();
                rs = std::clamp(rs - dy, 0., 1.);
                vScroll->setCurrentRangeStart(rs);
            }
        }
    }

    void scrollBarMoved(juce::ScrollBar *scrollBarThatHasMoved, double newRangeStart) override
    {
        if (scrollBarThatHasMoved == hScroll.get())
        {
            contents->setHorizontalZoom(newRangeStart, 1.0 / hScroll->getCurrentRangeSize());
        }
        if (scrollBarThatHasMoved == vScroll.get())
        {
            contents->setVerticalZoom(newRangeStart, 1.0 / vScroll->getCurrentRangeSize());
        }
    }
    void adjustVerticalZoom(const juce::Point<float> &p, float scaleFactor)
    {
        if (!vScroll)
            return;

        auto rs = vScroll->getCurrentRangeStart();
        auto re = vScroll->getCurrentRangeSize();

        auto nre = re / scaleFactor;
        auto dre = re - nre;
        re = nre;

        auto mfac = std::clamp(p.getY() / (getHeight() - (hScroll ? sbw : 0)), 0.f, 1.f);

        rs += dre * mfac;

        rs = std::clamp(rs, 0., 1.);
        re = std::clamp(re, 0., 1.);

        contents->setVerticalZoom(rs, 1.0 / re);

        vScroll->setCurrentRange(rs, re, juce::NotificationType::dontSendNotification);
        vScroll->repaint();
    }
    void adjustHorizontalZoom(const juce::Point<float> &p, float scaleFactor)
    {
        if (!hScroll)
            return;

        auto rs = hScroll->getCurrentRangeStart();
        auto re = hScroll->getCurrentRangeSize();

        auto nre = re / scaleFactor;
        auto dre = re - nre;
        re = nre;

        auto mfac = std::clamp(p.getX() / (getWidth() - (vScroll ? sbw : 0)), 0.f, 1.f);

        rs += dre * mfac;

        rs = std::clamp(rs, 0., 1.);
        re = std::clamp(re, 0., 1.);

        contents->setHorizontalZoom(rs, 1.0 / re);

        hScroll->setCurrentRange(rs, re, juce::NotificationType::dontSendNotification);
        hScroll->repaint();
    }
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_ZOOMCONTAINER_H
