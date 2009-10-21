/*
 * mapcell.cpp - one cell in K-map
 * created date: 2007/12/29
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
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

#include <QWidget>
#include <QString>
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QPen>
#include <QBrush>

#include "mapcell.h"

// cell constructor
MapCell::MapCell(int xPos, int yPos, int s, qreal b, int val)
{
    x = xPos;
    y = yPos;
    size = s;
    border = b;
    setValue(val);
}

void MapCell::draw(QPainter & painter) const
{
    painter.setPen(QPen(QBrush(Qt::SolidPattern),border));

    QRect rect(x,y,size,size);
    painter.drawRect(rect);

    if (value != UNDEF)
        painter.drawText(rect,Qt::AlignCenter,getValueText());
}

void MapCell::setValue(int val)
{
    if (val > UNDEF && val <= DONT_CARE)
        value = val;
    else
        value = UNDEF;
}

QString MapCell::getValueText() const
{
    switch (value) {
        case ZERO:
            return "0";
        case ONE:
            return "1";
        case DONT_CARE:
            return "X";
        default:
            return "";
    }
}

int MapCell::getNextValue(int val)
{
    if (val < 0 || val > 3)
        return UNDEF;
    val++;
    if (val == 4)
        val = 1;
    return val;
}

