/*
 * outputvalue.cpp - class representing literal value
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

#include "outputvalue.h"

#include <string>

// constructor - value from character
OutputValue::OutputValue(char ch) throw(InvalidValueExc)
{
    switch (ch) {
    case '0':
        value = ZERO;
        break;
    case '1':
        value = ONE;
        break;
    case 'X':
    case '-':
    case '2':
        value = DC;
        break;
    default:
        throw InvalidValueExc(value);
    }
}

// get next output value of the value ov (0 -> 1 -> 2 -> 0 -> ...)
OutputValue OutputValue::getNextValue(const OutputValue &ov)
{
    switch (ov.getValue()) {
    case ZERO:
        return OutputValue(ONE);
    case ONE:
        return OutputValue(DC);
    default: // missing value
        return OutputValue(ZERO);
    }
}

// converts the current value to char
char OutputValue::toChar(bool crossDC) const
{
    switch (value) {
    case ZERO:
        return '0';
    case ONE:
        return '1';
    default: // dont care value
        return crossDC? 'X': '-';
    }
}

// converts the current value to string
std::string OutputValue::toString(bool crossDC) const
{
    switch (value) {
    case ZERO:
        return "0";
    case ONE:
        return "1";
    default: // dont care value
        return  crossDC? "X": "-";
    }
}
