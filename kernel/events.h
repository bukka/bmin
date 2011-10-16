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

#include "kernel.h"

#include <exception>

class Formula;
class QuineMcCluskeyData;
class KMap;
class Cube;

// Minimize event class
class MinimizeEvent
{
public:
    // Some formula is set
    static const int FORMULA  = 1;
    // Minimized in debug mode
    static const int DEBUG    = 2;
    // Minimizing was run
    static const int RUN      = 4;
    // Espresso algorithm
    static const int ESPRESSO = 8;

    // constructor
    MinimizeEvent(int f = 0) : flags(f) {}

    // enables FORMULA flag
    void enableFormula() { flags |= FORMULA; }
    // enables DEBUG flag
    void enableDebug() { flags |= DEBUG; }
    // enables RUN flag
    void enableRun() { flags |= RUN; }
    // enables ESPRESSO flag
    void enableEspresso() { flags |= ESPRESSO; }

    // whether FORMULA flag is set
    bool isFormula() { return flags & FORMULA; }
    // whether DEBUG flag is set
    bool isDebug() { return flags & DEBUG; }
    // whether RUN flag is set
    bool isRun() { return flags & RUN; }
    // whether ESPRESSO flag is set
    bool isEspresso() { return flags & ESPRESSO; }

    // event flags
    int flags;
};

// Events class - superclass
class Events
{
public:
    // event constructor
    Events()
    {
        Kernel::instance()->registerEvents(this);
    }

    // event destructor
    virtual ~Events()
    {
        Kernel::instance()->unregisterEvents(this);
    }

protected:
    // actual formula is changed
    virtual void evtFormulaChanged(Formula *) {}
    // actual formula is removed
    virtual void evtFormulaRemoved() {}
    // actual formula is minimized
    virtual void evtFormulaMinimized(Formula *, MinimizeEvent &) {}
    // actual minimized formula is changed
    virtual void evtMinimalFormulaChanged(Formula *) {}
    // new formulas are set
    virtual void evtFormulasSet(unsigned) {}
    // minimizing algorithm is changed
    virtual void evtAlgorithmChanged(Kernel::Algorithm) {}
    // error invoked
    virtual void evtError(std::exception &) {}
    // bmin exit
    virtual void evtExit() {}
    // help request
    virtual void evtHelp() {}
    // Espresso algorithm data required
    virtual void evtShowEspresso(EspressoData *) {}
    // Quine-McCluskey algorithm data required
    virtual void evtShowQm(QuineMcCluskeyData *) {}
    // Karnaugh map data requiered
    virtual void evtShowKMap(KMap *) {}
    // Boolean cobe data required
    virtual void evtShowCube(Cube *) {}
    // Formula print request
    virtual void evtShowFce(Formula *, Formula *) {}


    friend class Kernel;
};


#endif // EVENTS_H
