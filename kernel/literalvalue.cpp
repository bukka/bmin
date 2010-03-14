/*
 * literalvalue.cpp - class representing literal value
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

#include "literalvalue.h"

#include <string>

LiteralValue LiteralValue::getNextValue(const LiteralValue &lv)
{
    switch (lv.getValue()) {
    case ZERO:
        return LiteralValue(ONE);
    case ONE:
        return LiteralValue(MISSING);
    default: // missing value
        return LiteralValue(ZERO);
    }
}

std::string LiteralValue::toString() const
{
    switch (value) {
    case ZERO:
        return "0";
    case ONE:
        return "1";
    default: // missing value
        return "-";
    }
}
