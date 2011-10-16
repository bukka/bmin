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

// gets next literal value of the value lv (0 -> 1 -> 2 -> 0 -> ...)
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

// constructor - value from character
LiteralValue::LiteralValue(char c) throw(InvalidValueExc)
{
    switch (c) {
    case '0':
        value = ZERO;
        break;
    case '1':
        value = ONE;
        break;
    case '-':
    case '2':
        value = MISSING;
        break;
    default:
        throw InvalidValueExc(value);
    }
}

// converts the current value to characte
char LiteralValue::toChar() const
{
    switch (value) {
    case ZERO:
        return '0';
    case ONE:
        return '1';
    default: // missing value
        return '-';
    }
}

// converts the current value to string
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
