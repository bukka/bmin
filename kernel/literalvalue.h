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

#include <string>

class LiteralValue
{
public:
    static const int ZERO = 0;
    static const int ONE = 1;
    static const int MISSING = 2;

    static LiteralValue getNextValue(const LiteralValue &lv);

    LiteralValue(int v) : value(v) {}

    inline int getValue() const { return value; }
    LiteralValue getNextValue() const;

    inline bool equal(const LiteralValue & val) const { return val.getValue() == value; }
    inline bool equal(int val) const { return val == value; }
    inline bool isZero() const { return value == ZERO; }
    inline bool isOne() const { return value == ONE; }
    inline bool isMissing() const { return value == MISSING; }

    bool operator==(const LiteralValue & val) const { return val.getValue() == value; }

    std::string toString() const;

 private:
    int value;

};

#endif // LITERALVALUE_H
