/*
 *
 * created date: 4/10/2010
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

#include "espresso.h"
#include "formula.h"

Espresso::Espresso()
{

}

Espresso::~Espresso()
{
    delete of;
}

Formula *Espresso::minimize(Formula *f, bool dbg)
{
    debug = dbg;

    delete of;
    of = new Formula(*f, true);
    mf = new Formula(*of);


    mf->setMinimized(true);
    f->setMinimized(true);

    return mf;
}