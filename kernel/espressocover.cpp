/*
 *
 * created date: 4/17/2010
 *
 * Copyright (C) 2010 Jakub Zelenka.
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

#include "espressocover.h"

#include <list>
using namespace std;


unsigned EspressoCover::cost() const
{
    return cover.size();
}

int EspressoCover::count() const
{
    return cover.size();
}

bool EspressoCover::isEmpty() const
{
    return cover.empty();
}

void EspressoCover::add(const Term &t)
{
    cover.push_back(t);
}

void EspressoCover::clear()
{
    cover.clear();
}

void EspressoCover::sort()
{
    cover.sort(DecreasingOrder());
}

void EspressoCover::removeInactived()
{
    cover.remove_if(InactiveEql());
}

void EspressoCover::setActived(bool value)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setActive(value);
}

bool EspressoCover::isCovered()
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++) {
        if (!(*it).isCovered())
            return false;
    }
    return true;
}

void EspressoCover::setCovered(bool value)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setCovered(value);
}

void EspressoCover::setCovering(EspressoCover &c)
{
    list<Term>::iterator itT = cover.begin();
    list<Term>::iterator itC = c.cover.begin();
    for (; itT != cover.end() && itC != c.cover.end(); itT++, itC++) {
        if ((*itC).isCovered())
            (*itT).setCovered(true);
    }
}

void EspressoCover::setTautology()
{
    unsigned size = cover.front().getSize();
    cover.clear();
    cover.push_back(Term(Term::MISSING_ALL, size));
}

void EspressoCover::appendDC(EspressoCover &d)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setDC(false);

    for (list<Term>::iterator it = d.cover.begin(); it != d.cover.end(); it++) {
        (*it).setDC(true);
        cover.push_back(*it);
    }
}

void EspressoCover::removeDC()
{
    cover.remove_if(DCEql());
}

void EspressoCover::removeRedundant()
{
    cover.remove_if(RedundantEql());
}
