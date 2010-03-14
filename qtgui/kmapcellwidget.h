/*
 * kmapcellwidget.h - K-map term cell
 * created date: 3/9/2010
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

#ifndef KMAPCELLWIDGET_H
#define KMAPCELLWIDGET_H

#include "kmap.h"

#include <QGraphicsWidget>

class KMapGridWidget;

// Class for cell in k-map
class KMapCellWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static const qreal SIZE    = 35.0;
    static const qreal BORDER  = 2.0;
    static const int FONT_SIZE = 10;
    static const int PADDING   = 4;
    static const int COVER_PEN = 2;

    KMapCellWidget(QGraphicsItem *parent = 0);
    void setWalls(bool top, bool bottom, bool left, bool right);
    void setValue(char value);
    void setGrid(KMapGridWidget *grid) { m_grid = grid; }
    void setSelection(bool selection) { m_selection = selection; }
    bool hasCoverPos(unsigned pos);
    unsigned addCover(KMapCell &cell, unsigned pos = 0);
    void setLastCoverPos(unsigned pos);
    void selectCover(const KMapCell &cell, bool selected);
    void clearCover();
    inline void setIdx(int idx) { m_idx = idx; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    inline qreal getCellWidth() { return m_cellWidth; }
    inline qreal getCellHeight() { return m_cellHeight; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    KMapGridWidget *m_grid;

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
    int m_coverCorrection;

    std::list<KMapCell> m_covers;

    // value
    char m_value;
    // value string
    QString m_valueStr;
    // text font
    QFont m_font;
    // cell index
    int m_idx;
    // whether term is selected
    bool m_selection;
    // selection color
    QBrush m_selectionBrush;

signals:
    void cellValueChanged(int, OutputValue &);
};

#endif // KMAPCELLWIDGET_H
