/*
 * modulewidget.h - module superclass
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

#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include <QWidget>

class ModuleWidget : public QWidget
{
public:
	ModuleWidget(const QString &name, int pos);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	// getter for module name
	QString name() const { return m_name; }
	// getter for position
	int position() const { return m_position; }
private:
	// module name
	QString m_name;
	// position
	int m_position;
};

#endif // MODULEWIDGET_H
