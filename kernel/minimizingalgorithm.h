/*
 * minimizingalgorithm.h - abstract class for minimizing algorithms
 * created date: 10/25/2009
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

#ifndef MINIMIZINGALGORITHM_H
#define MINIMIZINGALGORITHM_H

class Formula;

class MinimizingAlgorithm
{
public:
    MinimizingAlgorithm() : of(0), mf(0), debug(false) {}

    virtual Formula *minimize(Formula *f, bool debug) = 0;

    virtual inline Formula *getMinimizedFormula() { return mf; }
    virtual inline Formula *getOriginalFormula() { return of; }

    // debugging
    inline void setDebug(bool value) { debug = value; }
    void enableDebug() { setDebug(true); }
    void disableDebug() { setDebug(false); }
    bool isDebug() { return debug; }


protected:
    Formula *of; // original formula
    Formula *mf; // minimized formula
    bool debug;
};

#endif // MINIMIZINGALGORITHM_H
