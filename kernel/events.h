/*
 *
 * created date: 12/18/2009
 *
 * Copyright (C) 2009 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <exception>

class Kernel;
class Formula;

class Events
{
public:
    Events();
    virtual ~Events();

protected:
    virtual void evtFormulaChanged(Formula *) {}
    virtual void evtFormulaMinimized(bool) {}
    virtual void evtError(std::exception &) {}
    virtual void evtExit() {}
    virtual void evtHelp() {}
    virtual void evtShowQm() {}
    virtual void evtShowMap() {}
    virtual void evtShowCube() {}
    virtual void evtShowFce(Formula *, Formula *) {}


    friend class Kernel;
};

#endif // EVENTS_H
