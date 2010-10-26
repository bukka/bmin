/*
 * termssortlist.cpp - specila terms list for fast sorting of terms
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

#include "termssortinglist.h"

TermsSortingItem::TermsSortingItem(const Term &term) : Term(term)
{
}

TermsSortingList::TermsSortingList()
{
}

void TermsSortingList::push_back(const Term &term)
{
    std::list<TermsSortingItem>::push_back(TermsSortingItem(term));
}

void TermsSortingList::push_front(const Term &term)
{
    std::list<TermsSortingItem>::push_front(TermsSortingItem(term));
}
