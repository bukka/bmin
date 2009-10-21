/*
 * mapcell.h - one cell in K-map
 * created date: 2007/12/29
 *
 * Copyright (C) 2007 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifndef MAPCELL_H
#define MAPCELL_H

class QPainter;
class QString;

// class for one cell in karnaugh map
class MapCell
{
public:
    static const int DEF_SIZE = 25;
    static const qreal DEF_BORDER = 2.0;

    static const int UNDEF = 0;
    static const int ZERO = 1;
    static const int ONE = 2;
    static const int DONT_CARE = 3;
    static const int DONT_CARE2 = 4;

    static int getNextValue(int val);

    MapCell(int xPos, int yPos, int s = DEF_SIZE,
        qreal b = DEF_BORDER, int val = UNDEF);

    void draw(QPainter & painter) const;

    int getValue() const { return value; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getSize() const { return size; }
    qreal getBorder() const { return border; }

    void setValue(int val);
    void setPosition(int xPos, int yPos) { x = xPos; y = yPos; }
    void setSize(int s) { size = s; }
    void setBorder(qreal b) { border = b; }

private:
    int x;	// X position
    int y;	// Y position
    int size;
    qreal border;
    int value;

    QString getValueText() const;
};
#endif // MAPCELL_H

