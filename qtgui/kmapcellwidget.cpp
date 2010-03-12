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

#include "kmapcellwidget.h"
#include "kmapgridwidget.h"

#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

KMapCellWidget::KMapCellWidget(QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    m_cellWidth = m_cellHeight = SIZE;
    m_penSize = static_cast<int>(BORDER / 2);
    m_cellSize = static_cast<int>(SIZE);
    m_wallSize = static_cast<int>(BORDER);
    m_coverCorrection = (COVER_PEN % 2)? 0: 1;
    m_selection = false;
    m_selectionBrush = QBrush(QColor(220, 220, 220));
    m_font = QFont("Monospace", FONT_SIZE, QFont::Normal);
}

void KMapCellWidget::setWalls(bool top, bool bottom, bool left, bool right)
{
    if ((m_topWall = top))
        m_cellHeight += BORDER;
    if ((m_bottomWall = bottom))
        m_cellHeight += BORDER;
    if ((m_leftWall = left))
        m_cellWidth += BORDER;
    if ((m_rightWall = right))
        m_cellWidth += BORDER;

    setMinimumSize(m_cellWidth, m_cellHeight);
    setMaximumSize(minimumSize());
}

void KMapCellWidget::setValue(char value)
{
    m_value = value;
    m_valueStr = QString(QChar(value));

    m_covers.clear();
}

bool KMapCellWidget::hasCoverPos(unsigned pos)
{
    for (list<KMapCell>::iterator it = m_covers.begin(); it != m_covers.end(); it++) {
        if (pos == (*it).getPos())
            return true;
    }
    return false;
}

unsigned KMapCellWidget::addCover(KMapCell &cell, unsigned pos)
{
    unsigned position = pos;
    if (hasCoverPos(pos)) {
        // find new pos
        for (unsigned i = 0; i < static_cast<unsigned>(KMap::MAX_VARS); i++) {
            if (!hasCoverPos(i)) {
                position = i;
                break;
            }
        }
    }
    cell.setPos(position);
    m_covers.push_front(cell);
    return position;
}

void KMapCellWidget::setLastCoverPos(unsigned pos)
{
    m_covers.front().setPos(pos);
}

void KMapCellWidget::selectCover(const KMapCell &cell, bool selected)
{
    list<KMapCell>::iterator it = find(m_covers.begin(), m_covers.end(), cell);
    if (it != m_covers.end())
        (*it).setSelection(selected);
}

void KMapCellWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    unsigned x = (m_leftWall? m_wallSize: 0);
    unsigned y = (m_topWall? m_wallSize: 0);
    int width = m_cellSize - m_penSize;
    int height = m_cellSize - m_penSize;


    painter->setFont(m_font);
    painter->setPen(QPen(Qt::black, m_penSize));
    if (m_selection)
        painter->setBrush(m_selectionBrush);
    painter->drawRect(x, y, width, height);
    painter->setBrush(Qt::NoBrush);

    if (m_topWall)
        painter->drawLine(x - 1, y - m_penSize, x + 1 + width, y - m_penSize);
    if (m_bottomWall)
        painter->drawLine(x, y + height + m_penSize, x + 1 + width, y + height + m_penSize);
    if (m_leftWall)
        painter->drawLine(x - m_penSize, y - 1, x - m_penSize, y + 1 + height);
    if (m_rightWall)
        painter->drawLine(x + height + m_penSize, y - 1, x + height + m_penSize, y + 1 + height);


    if (!m_covers.empty()) {
        int x1, y1, x2, y2;
        int nextPos = 2 * m_penSize;

        for (list<KMapCell>::reverse_iterator it = m_covers.rbegin(); it != m_covers.rend(); it++) {
            KMapCell &cell = *it;
            int pos = cell.getPos();
            int padding = PADDING + pos * nextPos - 1;

            if (cell.isSelected()) {
                painter->setPen(QPen(Qt::black, m_penSize));
                painter->setBrush(QBrush(m_grid->getColor(cell.getColor(), true)));
                painter->drawRect(x, y, width, height);
                painter->setBrush(Qt::NoBrush);
            }

            painter->setPen(QPen(QBrush(m_grid->getColor(cell.getColor())), COVER_PEN));

            if (cell.hasTop()) {
                y1 = y2 = padding + y;
                x1 = x + (cell.hasLeft()? padding: 0);
                x2 = width + x - (cell.hasRight()? padding: 0) + m_coverCorrection;
                painter->drawLine(x1, y1, x2, y2);
            }
            if (cell.hasBottom()) {
                y1 = y2 = height + y - padding + m_coverCorrection;
                x1 = x + (cell.hasLeft()? padding: 0);
                x2 = width + x - (cell.hasRight()? padding: 0) + m_coverCorrection;
                painter->drawLine(x1, y1, x2, y2);
            }
            if (cell.hasLeft()) {
                x1 = x2 = padding + x;
                y1 = y + (cell.hasTop()? padding: 0);
                y2 = height + y - (cell.hasBottom()? padding: 0) + m_coverCorrection;
                painter->drawLine(x1, y1, x2, y2);
            }
            if (cell.hasRight()) {
                x1 = x2 = width + x - padding + m_coverCorrection;
                y1 = y + (cell.hasTop()? padding: 0);
                y2 = height + y - (cell.hasBottom()? padding: 0) + m_coverCorrection;
                painter->drawLine(x1, y1, x2, y2);
            }
        }
    }

    if (!m_valueStr.isEmpty()) {
        painter->setPen(QPen(Qt::black, m_penSize));
        painter->drawText(x, y, width, height, Qt::AlignCenter, m_valueStr);
    }
}

void KMapCellWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        OutputValue value = OutputValue::getNextValue(OutputValue(m_value));
        emit cellValueChanged(m_idx, value);
    }
}

