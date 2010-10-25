/*
 * termssortlist.h - specila terms list for fast sorting of terms
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

#ifndef TERMSSORTLIST_H
#define TERMSSORTLIST_H

#include "term.h"

#include <list>

class TermsSortItem : public Term
{
    TermsSortItem() {}
};


class TermsSortList : public std::list<TermsSortItem>
{
public:
    TermsSortList();


};

#endif // TERMSSORTLIST_H
