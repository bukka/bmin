/*
 * modulewidget.cpp - module superclass
 * created date: 2009/08/04
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

#include "modulewidget.h"

ModuleWidget::ModuleWidget(const QString &name, int pos)
        : m_name(name), m_position(pos) {}

QSize ModuleWidget::minimumSizeHint() const
{
    return QSize(500, 400);
}

QSize ModuleWidget::sizeHint() const
{
    return QSize(600, 500);
}
