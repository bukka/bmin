/*
 * errorwidget.cpp - widget with error message
 * created date: 5/4/2010
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

#include "errorwidget.h"

#include <QPainter>

ErrorWidget::ErrorWidget(const QString &msg)
{
    message = msg;
}

void ErrorWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(QRect(0, 0, width(), height()), QColor(Qt::black));

    painter.setPen(QColor(200, 0, 0));
    painter.setFont(QFont("monspace", 18));
    painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, message);
}
