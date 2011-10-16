/*
 * literalvalue.h - class representing literal value
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

#ifndef LITERALVALUE_H
#define LITERALVALUE_H

#include "kernelexc.h"

#include <string>

// class for term literal value
class LiteralValue
{
public:
    // value 0
    static const int ZERO = 0;
    // value 1
    static const int ONE = 1;
    // missing value - can be 1 or 0
    static const int MISSING = 2;

    // gets next literal value of the value lv (0 -> 1 -> 2 -> 0 -> ...)
    static LiteralValue getNextValue(const LiteralValue &lv);

    // constructor - value from integer
    LiteralValue(int v) : value(v) {}
    // constructor - value from character
    LiteralValue(char c) throw(InvalidValueExc);

    // returns integer representation of value
    inline int getValue() const { return value; }
    // returns class representation of value
    LiteralValue getNextValue() const;

    // equality
    inline bool equal(const LiteralValue & val) const { return val.getValue() == value; }
    // equality
    inline bool equal(int val) const { return val == value; }
    // if the value is 0, returns true otherwise false
    inline bool isZero() const { return value == ZERO; }
    // if the value is 1, returns true otherwise false
    inline bool isOne() const { return value == ONE; }
    // if the value is missing, returns true otherwise false
    inline bool isMissing() const { return value == MISSING; }

    // equality operator
    bool operator==(const LiteralValue & val) const { return val.getValue() == value; }

    // converts the current value to character
    char toChar() const;
    // converts the current value to string
    std::string toString() const;

 private:
    // the current value
    int value;

};

#endif // LITERALVALUE_H
