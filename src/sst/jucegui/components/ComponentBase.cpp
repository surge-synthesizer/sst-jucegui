/*
 * sst-juce-guil - an open source library of juce widgets
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

#include "sst/jucegui/components/ComponentBase.h"
#include <cassert>

namespace sst::jucegui::components
{

struct wiTimer : juce::Timer
{
    void timerCallback() override
    {
        assert(juce::MessageManager::getInstance()->isThisTheMessageThread());
        for (auto *w : WithIdleTimer::registeredItems)
        {
            if (w->delayInMs > 0)
            {
                w->delayInMs -= WithIdleTimer::idleTimeMS;
                if (w->delayInMs <= 0)
                {
                    if (w->onIdleHover)
                    {
                        w->onIdleHover();
                    }
                }
            }
        }
    }
};

WithIdleTimer::WithIdleTimer()
{
    if (!theTimer)
    {
        theTimer = std::make_unique<wiTimer>();
        theTimer->startTimer(idleTimeMS);
    }
    timerClients++;
}
WithIdleTimer::~WithIdleTimer()
{
    timerClients--;
    if (timerClients == 0)
    {
        if (theTimer)
        {
            theTimer->stopTimer();
        }
        theTimer.reset();
    }
}
std::unique_ptr<juce::Timer> WithIdleTimer::theTimer;
std::unordered_set<WithIdleTimer *> WithIdleTimer::registeredItems;
uint64_t WithIdleTimer::timerClients{0};
} // namespace sst::jucegui::components