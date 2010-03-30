/*
 * events.h - Events abstract class for events system
 * created date: 12/18/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <exception>

class Kernel;
class Formula;
class QuineMcCluskeyData;
class KMap;

class MinimizeEvent
{
public:
    static const int FORMULA = 1;  // Some formula is set
    static const int DEBUG   = 2;  // Minimized in debug mode
    static const int RUN     = 4;  // Minimizing was run

    MinimizeEvent(int f = 0) : flags(f) {}

    void enableFormula() { flags |= FORMULA; }
    void enableDebug() { flags |= DEBUG; }
    void enableRun() { flags |= RUN; }

    bool isFormula() { return flags & FORMULA; }
    bool isDebug() { return flags & DEBUG; }
    bool isRun() { return flags & RUN; }


    int flags;
protected:
};

class Events
{
public:
    Events();
    virtual ~Events();

protected:
    virtual void evtFormulaChanged(Formula *) {}
    virtual void evtFormulaRemoved() {}
    virtual void evtFormulaMinimized(Formula *, MinimizeEvent &) {}
    virtual void evtFormulasSet(unsigned) {}
    virtual void evtError(std::exception &) {}
    virtual void evtExit() {}
    virtual void evtHelp() {}
    virtual void evtShowQm(QuineMcCluskeyData *) {}
    virtual void evtShowKMap(KMap *) {}
    virtual void evtShowCube() {}
    virtual void evtShowFce(Formula *, Formula *) {}


    friend class Kernel;
};


#endif // EVENTS_H
