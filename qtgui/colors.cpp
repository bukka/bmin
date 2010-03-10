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

#include "colors.h"

Colors::Colors(int n, int alpha, int saturation, int lightness)
{
    double step = 360 / n;
    int generator = getGenerator(n);

    m_size = n;
    m_colors = new QColor[n + 1];

    for (int i = 0; i < n; i++) {
        m_colors[(i * generator) % n] = QColor::fromHsv(
                static_cast<int>(i * step), saturation, lightness, alpha);
    }
    m_colors[m_size] = QColor(Qt::black);
}

Colors::~Colors()
{
    delete [] m_colors;
}

const QColor &Colors::operator[](int pos)
{
    if (pos < 0 || pos > m_size)
        return m_colors[m_size];
    else
        return m_colors[pos];
}

bool Colors::isGenerator(int gen, int n)
{
    for (int i = 2; i < (n + 1); i++) {
        if ((gen * i) % n == 1)
            return true;
    }
    return false;
}

int Colors::getGenerator(int n)
{
    for (int i = n / 2; i > 1; i--) {
        if (isGenerator(i, n))
            return i;
    }
    return 1;
}
