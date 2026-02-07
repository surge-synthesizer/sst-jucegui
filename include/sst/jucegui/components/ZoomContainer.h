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
#include "ToolTip.h"

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
    struct MagControl : juce::Component
    {
        MagControl(ZoomContainer *z, ScrollBar *v, ScrollBar *h) : z(z), v(v), h(h)
        {
            if (!(h || v))
                return;
            toolTip = std::make_unique<ToolTip>();
            z->addChildComponent(*toolTip);
        }

        void resized() override
        {
            if (toolTip)
            {
                if (h)
                    toolTip->setStyle(h->style());
                else
                    toolTip->setStyle(v->style());
                toolTip->setTooltipTitleAndData("Drag to zoom", std::vector<std::string>());

                auto sz = toolTip->getBounds().expanded(2);
                auto bc =
                    z->getLocalBounds().getBottomRight() -
                    juce::Point<int>(sz.getWidth() + getWidth(), sz.getHeight() + getHeight());
                toolTip->setBounds(bc.getX(), bc.getY(), sz.getWidth(), sz.getHeight());
            }
        }

        void paint(juce::Graphics &g) override
        {
            if (!(h || v) || !z)
                return;

            const auto &style = (h ? h->style() : v->style());
            auto c = style->getColour(ScrollBar::Styles::styleClass, ScrollBar::Styles::outline);
            g.setColour(c);
            auto b = getLocalBounds().withTrimmedRight(2).withTrimmedBottom(2).reduced(1);
            g.drawEllipse(b.toFloat(), 2);
            auto fp = getLocalBounds().getBottomRight();
            auto cp = fp - juce::Point<int>(3, 3);
            g.drawLine(fp.getX(), fp.getY(), cp.getX(), cp.getY(), 2);
        }

        void mouseEnter(const juce::MouseEvent &event) override
        {
            if (toolTip)
                toolTip->setVisible(true);
            z->repaint();
        }
        void mouseExit(const juce::MouseEvent &event) override
        {
            if (toolTip)
                toolTip->setVisible(false);
            z->repaint();
        }
        juce::Point<float> lmpos;
        void mouseDown(const juce::MouseEvent &event) override { lmpos = event.position; }
        void mouseDrag(const juce::MouseEvent &event) override
        {
            if (toolTip && toolTip->isVisible())
                toolTip->setVisible(false);
            auto d = event.position - lmpos;
            lmpos = event.position;

            auto cp = z->getLocalBounds().getCentre().toFloat();
            static constexpr float magFac{0.005f};
            if (fabs(d.x) > fabs(d.y))
            {
                z->adjustHorizontalZoom(cp, 1 + magFac * d.x);
            }
            else
            {
                z->adjustVerticalZoom(cp, 1 - magFac * d.y);
            }
        }
        ZoomContainer *z;
        ScrollBar *v, *h;
        std::unique_ptr<ToolTip> toolTip;
    };

    std::unique_ptr<ScrollBar> vScroll, hScroll;
    std::unique_ptr<MagControl> magControl;
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

        if (hScroll || vScroll)
        {
            magControl = std::make_unique<MagControl>(this, vScroll.get(), hScroll.get());
            addAndMakeVisible(*magControl);
        }
    }

    static constexpr int scrollBarWidth{6}, scrollBarMargin{2};

    void resized() override
    {
        auto bx = getLocalBounds();

        auto trim = scrollBarWidth + scrollBarMargin;
        if (hScroll && vScroll)
        {
            auto hb = bx.withTrimmedTop(bx.getHeight() - scrollBarWidth).withTrimmedRight(trim);
            auto vb = bx.withTrimmedLeft(bx.getWidth() - scrollBarWidth).withTrimmedBottom(trim);
            auto mc = bx.withTrimmedTop(bx.getHeight() - scrollBarWidth - 3)
                          .withTrimmedLeft(bx.getWidth() - scrollBarWidth - 3);
            bx = bx.withTrimmedBottom(trim).withTrimmedRight(trim);
            hScroll->setBounds(hb);
            vScroll->setBounds(vb);
            magControl->setBounds(mc);
        }
        else if (hScroll)
        {
            auto hb = bx.withTrimmedTop(bx.getHeight() - scrollBarWidth);
            auto mc = bx.withTrimmedTop(bx.getHeight() - scrollBarWidth - 3)
                          .withTrimmedLeft(bx.getWidth() - scrollBarWidth - 3);
            bx = bx.withTrimmedBottom(trim);
            hScroll->setBounds(hb);
            magControl->setBounds(mc);
        }
        else if (vScroll)
        {
            auto vb = bx.withTrimmedLeft(bx.getWidth() - scrollBarWidth);
            auto mc = bx.withTrimmedTop(bx.getHeight() - scrollBarWidth - 3)
                          .withTrimmedLeft(bx.getWidth() - scrollBarWidth - 3);
            bx = bx.withTrimmedRight(trim);
            vScroll->setBounds(vb);
            magControl->setBounds(mc);
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
        /* Evil Dragon spec for windows:
         *
         * Mousewheel - scroll up-down (even though one could argue in Sample view it would be far
         * more useful if it did zoom in/out, straight up)
         * Shift+Mousewheel - scroll left-right
         * Alt+Mousewheel - vertical zoom in-out
         * Shift+Alt+Mousewheel - horizontal zoom in-out
         *
         * HMousewheel - scroll left-right (this is backwards with mac code)
         * Alt+HMousewheel - zoom horizontally
         */

#if JUCE_MAC
        static constexpr float wheelFac{1.f}; // adjust zoom by this much on win
#else
        static constexpr float wheelFac{0.2f}; // adjust zoom by this much on win
#endif

        if (fabs(wheel.deltaX) < fabs(wheel.deltaY))
        {
            // OK so we have a vertical-style motion
            bool doHZoom{false}, doVZoom{false}, doHScroll{false};
#if JUCE_MAC
            auto sd = event.mods.isCommandDown();
            auto cd = event.mods.isAltDown();
            doHZoom = sd && cd;
            doVZoom = cd && !doHZoom;
            doHScroll = sd && !doHZoom && !doVZoom;
#else
            auto ad = event.mods.isAltDown();
            auto sd = event.mods.isShiftDown();
            doHZoom = sd && ad;
            doVZoom = ad && !doHZoom;
            doHScroll = sd && !doHZoom && !doVZoom;
#endif


            if (doHZoom)
            {
                // HZOOM
                if (contents->supportsHorizontalZoom())
                {
                    adjustHorizontalZoom(event.position, 1.0 + wheelFac * wheel.deltaY);
                }
            }
            else if (doVZoom)
            {
                // VZoom by delta Y
                if (contents->supportsVerticalZoom())
                {
                    adjustVerticalZoom(event.position, 1.0 + wheelFac * wheel.deltaY);
                }
            }
            else if (doHScroll)
            {
                // HSCROLL
                if (hScroll)
                {
                    auto dy = -wheel.deltaY;
                    auto rs = hScroll->getCurrentRangeStart();
                    auto rw = hScroll->getCurrentRangeSize();

                    // You want translation to be relative to the size to make
                    // it sort of "uniform speed"
                    rs = std::clamp(rs - wheelFac * dy * rw * 2, 0., 1.);
                    hScroll->setCurrentRangeStart(rs);
                }
            }
            else
            {
                // VSCROLL
                if (vScroll)
                {
                    auto dy = wheel.deltaY;
                    auto rs = vScroll->getCurrentRangeStart();
                    auto rw = vScroll->getCurrentRangeSize();

                    rs = std::clamp(rs - wheelFac * dy * rw * 2, 0., 1.);
                    vScroll->setCurrentRangeStart(rs);
                }
            }
        }
        else
        {
#if JUCE_MAC
            if (hScroll)
            {
                auto dy = wheel.deltaX;
                auto rs = hScroll->getCurrentRangeStart();
                auto rw = hScroll->getCurrentRangeSize();

                // You want translation to be relative to the size to make
                // it sort of "uniform speed"
                rs = std::clamp(rs - dy * rw * 2, 0., 1.);
                hScroll->setCurrentRangeStart(rs);
            }
#else
            auto sd = event.mods.isShiftDown();
            if (sd)
            {
                if (contents->supportsHorizontalZoom())
                {
                    adjustHorizontalZoom(event.position, 1.0 + wheelFac * wheel.deltaX);
                }
            }
            else
            {
                if (hScroll)
                {
                    auto dy = -wheel.deltaX; // win apparently reversed
                    auto rs = hScroll->getCurrentRangeStart();
                    auto rw = hScroll->getCurrentRangeSize();

                    // You want translation to be relative to the size to make
                    // it sort of "uniform speed"
                    rs = std::clamp(rs - wheelFac * dy * rw * 2, 0., 1.);
                    hScroll->setCurrentRangeStart(rs);
                }
            }
#endif
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

    double hZoomFloor{0.0}, vZoomFloor{0};
    /**
     * Set the smallest zoom as a percentage of the total window. So 0.1
     * means you cap out at 10x zoom. And (24+2)/128 means you cap out at
     * 2 octaves and 2 notes if your span is 128 notes.
     *
     * @param f The zoom
     */
    void setHZoomFloor(float f) { hZoomFloor = std::clamp(f, 0.f, 1.f); }
    void setVZoomFloor(float f) { vZoomFloor = std::clamp(f, 0.f, 1.f); }

    void adjustVerticalZoom(const juce::Point<float> &p, float scaleFactor)
    {
        if (!vScroll)
            return;

        auto rs = vScroll->getCurrentRangeStart();
        auto ors = rs;
        auto re = vScroll->getCurrentRangeSize();

        auto nre = re / scaleFactor;
        auto dre = re - nre;
        re = nre;

        auto mfac = std::clamp(p.getY() / (getHeight() - (hScroll ? scrollBarWidth : 0)), 0.f, 1.f);

        rs += dre * mfac;

        rs = std::clamp(rs, 0., 1.);
        re = std::clamp(re, vZoomFloor, 1.);
        if (re <= vZoomFloor)
            rs = ors;

        contents->setVerticalZoom(rs, 1.0 / re);

        vScroll->setCurrentRange(rs, re, juce::NotificationType::dontSendNotification);
        vScroll->repaint();
    }
    void adjustHorizontalZoom(const juce::Point<float> &p, float scaleFactor)
    {
        if (!hScroll)
            return;

        auto rs = hScroll->getCurrentRangeStart();
        auto ors = rs;
        auto re = hScroll->getCurrentRangeSize();

        auto nre = re / scaleFactor;
        auto dre = re - nre;
        re = nre;

        auto mfac = std::clamp(p.getX() / (getWidth() - (vScroll ? scrollBarWidth : 0)), 0.f, 1.f);

        rs += dre * mfac;

        rs = std::clamp(rs, 0., 1.);
        re = std::clamp(re, hZoomFloor, 1.);
        if (re <= hZoomFloor)
            rs = ors;

        contents->setHorizontalZoom(rs, 1.0 / re);

        hScroll->setCurrentRange(rs, re, juce::NotificationType::dontSendNotification);
        hScroll->repaint();
    }
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_ZOOMCONTAINER_H
