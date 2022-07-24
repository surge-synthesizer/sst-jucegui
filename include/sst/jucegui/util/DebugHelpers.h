/*
 * sst-jucegui - a set of widgets for ShortCircuit, SurgeXT2, and others
 * Copyright 2022 to authors per the github transaction log
 *
 * Released under the MIT license. See 'LICENSE.md' for details.
 */

#ifndef SST_JUCEGUI_DEBUGHELPERS_H
#define SST_JUCEGUI_DEBUGHELPERS_H

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
