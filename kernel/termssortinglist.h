/*
 * termssortinglist.h - specila terms list for fast sorting of terms
 * created date: 10/25/2010
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

#ifndef TERMSSORTINGLIST_H
#define TERMSSORTINGLIST_H

#include "term.h"

#include <list>
#include <vector>

class TermsSortingItem : public Term
{
public:
    TermsSortingItem();
    TermsSortingItem(const Term *term);
    TermsSortingItem(const TermsSortingItem &tsi);
    virtual ~TermsSortingItem();

    // comparing
    bool operator<(const TermsSortingItem &tsi) const;
    bool operator>(const TermsSortingItem &tsi) const;

private:
    unsigned isize;
    int *indexes;
};


class TermsSortingList : public std::list<TermsSortingItem>
{
public:
    TermsSortingList() : std::list<TermsSortingItem>() {}

    void push_back(const Term &term);
    void push_front(const Term &term);

    void sort();
};

#endif // TERMSSORTINGLIST_H
