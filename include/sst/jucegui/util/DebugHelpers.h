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

#ifndef INCLUDE_SST_JUCEGUI_UTIL_DEBUGHELPERS_H
#define INCLUDE_SST_JUCEGUI_UTIL_DEBUGHELPERS_H

#if DEBUG
#include <iostream>

#define DBGOUT(x)                                                                                  \
    std::cout << __FILE__ << ":" << __LINE__ << "|" << __func__ << "| " << x << std::endl;

#define DBGMARK DBGOUT("")
#define DBGVAL(x) " " << (#x) << "=" << x
#define DBGTYPE(x) " typeof:" << (#x) << "=" << typeid(x).name()
#else
#define DBGOUT(x) (void *)0;
#define DBGMARK DBGOUT("")
#define DBGVAL(x) (void *)0;
#define DBGTYPE(x) (void *)0;
#endif

#endif // SST_JUCEGUI_DEBUGHELPERS_H
