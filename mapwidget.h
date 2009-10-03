/*
 * mapwidget.h - K-map
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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <vector>

#include "modulewidget.h"
#include "term.h"

class MapCell;
class Formula;

class MapWidget : public ModuleWidget
{
	Q_OBJECT

public:
	static const int MAX_SIZE = 6;

	MapWidget(const QString &name, int pos);

	void setCellSize(int size) { cellSize = size; }
	void setBorderSize(qreal border) { cellBorder = border; }

	int getMapSize() { return mapSize; }
	int getCellSize() { return cellSize; }
	qreal getCellBorder() { return cellBorder; }

signals:
	void mapChanged(int idx, tval value);

public slots:
	void invalidateMap();
	void setMapData(Formula *formula);
	void setActivity(bool active);
	void setAllValues(int val);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);

private:
	static const int transX = 150;
	static const int transY = 100;

	enum {MSG_OVER, MSG_INVALID, MSG_NONE} msg;

	bool isActive;
	int mapSize;
	int cellSize;
	qreal cellBorder;
	std::vector<MapCell *> cells;
	std::vector<char> vars;

	void updateMap() { update(); }
	void setValues(std::vector<int> idx, int value, int size);
	void clearCells();
	void showMsg(QPainter &, QString &);
};
#endif // MAPWIDGET_H
