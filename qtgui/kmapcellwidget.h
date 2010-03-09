/*
 *
 * created date: 3/9/2010
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

#ifndef KMAPCELLWIDGET_H
#define KMAPCELLWIDGET_H

#include "kmap.h"

#include <QGraphicsWidget>

// Class for cell in k-map
class KMapCellWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static const qreal SIZE    = 35.0;
    static const qreal BORDER  = 2.0;
    static const int FONT_SIZE = 10;
    static const int PADDING   = 4;

    KMapCellWidget(QGraphicsItem *parent = 0);
    void setWalls(bool top, bool bottom, bool left, bool right);
    void setValue(char value);
    bool hasCoverPos(unsigned pos);
    unsigned addCover(KMapCell &cell, unsigned pos = 0);
    void setLastCoverPos(unsigned pos);
    void clearCover();
    inline void setIdx(int idx) { m_idx = idx; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    inline qreal getCellWidth() { return m_cellWidth; }
    inline qreal getCellHeight() { return m_cellHeight; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    // walls
    bool m_topWall;
    bool m_bottomWall;
    bool m_leftWall;
    bool m_rightWall;

    qreal m_cellWidth;
    qreal m_cellHeight;
    int m_penSize;
    int m_wallSize;
    int m_cellSize;

    std::list<KMapCell> m_covers;

    // value
    char m_value;
    // value string
    QString m_valueStr;
    // text font
    QFont m_font;
    // cell index
    int m_idx;

signals:
    void cellValueChanged(int, OutputValue &);
};

#endif // KMAPCELLWIDGET_H
