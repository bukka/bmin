/*
 *
 * created date: 3/10/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#ifndef COLORS_H
#define COLORS_H

#include <QColor>

// colors generator and container
class Colors
{
public:
    static const int DEFAULT_SATURATION = 255;
    static const int DEFAULT_LIGHTNESS  = 255;
    static const int DEFAULT_ALPHA      = 255;

    Colors(int n,
           int alpha      = DEFAULT_ALPHA,
           int saturation = DEFAULT_SATURATION,
           int lightness  = DEFAULT_LIGHTNESS);

    ~Colors();

    const QColor &operator[](int pos);

private:
    int getGenerator(int n);
    bool isGenerator(int gen, int n);
    int m_size;
    QColor *m_colors;
};

#endif // COLORS_H
