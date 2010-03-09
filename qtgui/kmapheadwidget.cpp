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

#include "kmapheadwidget.h"
#include "kmapcellwidget.h"

#include <QPainter>
#include <QMenu>
#include <QActionGroup>
#include <QGraphicsSceneContextMenuEvent>

#include <vector>
using namespace std;

KMapHeadWidget::KMapHeadWidget(Mode m, Direction d, QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    m_direction = d;
    setOwnedByLayout(false);

    // make actions
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    m_linesAction = new QAction(tr("Lines Mode"), group);
    m_linesAction->setCheckable(true);
    connect(m_linesAction, SIGNAL(triggered()), this, SLOT(setLinesMode()));
    m_binaryAction = new QAction(tr("Binary Mode"), group);
    m_binaryAction->setCheckable(true);
    connect(m_binaryAction, SIGNAL(triggered()), this, SLOT(setBinaryMode()));

    setMode(m);
}

void KMapHeadWidget::setMode(Mode m)
{
    m_mode = m;
    if (m == LINES)
        m_linesAction->setChecked(true);
    else // binary
        m_binaryAction->setChecked(true);
    update();
}

void KMapHeadWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(m_linesAction);
    menu.addAction(m_binaryAction);
    menu.exec(event->screenPos());
}

void KMapHeadWidget::setLinesMode()
{
    emit modeChanged(LINES);
}

void KMapHeadWidget::setBinaryMode()
{
    emit modeChanged(BINARY);
}


// Vars - lines

KMapLineVarsWidget::KMapLineVarsWidget(vector<char> *vars, Direction d, QGraphicsItem *parent)
        : KMapHeadWidget(LINES, d, parent)
{
    m_vars = vars;
    m_varsCount = vars->size();
    m_font = QFont("Monospace", FONT_SIZE, QFont::Normal);

    if (d == HORIZONTAL) {
        m_width = KMapCellWidget::SIZE;
        m_height = SIZE;
    }
    else {
        m_width = SIZE;
        m_height = KMapCellWidget::SIZE;
    }

    setMinimumSize(m_width, m_height);
    setMaximumSize(minimumSize());
}

void KMapLineVarsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(m_font);

    int charShift = (FONT_SIZE / 2);
    int linePos = SIZE - LINE_SPACING;
    for (unsigned i = 0; i < m_varsCount; i++, linePos -= LINE_SPACING) {
        if (m_direction == HORIZONTAL)
            painter->drawText(charShift, linePos + charShift, QString(m_vars->at(i)));
        else
            painter->drawText(linePos - charShift, FONT_SIZE + charShift, QString(m_vars->at(i)));
    }
}

// Vars - binary

KMapBinaryVarsWidget::KMapBinaryVarsWidget(vector<char> *hVars, vector<char> *vVars, QGraphicsItem *parent)
        : KMapHeadWidget(BINARY, HORIZONTAL, parent)
{
    setVars(hVars, vVars);
    m_hVarsPoint = QPoint(2 * FONT_SIZE, FONT_SIZE);
    m_vVarsPoint = QPoint(0, SIZE);

    m_font = QFont("Monospace", FONT_SIZE, QFont::Bold);

    setMinimumSize(SIZE, SIZE);
    setMaximumSize(minimumSize());
}

void KMapBinaryVarsWidget::setVars(std::vector<char> *hVars, std::vector<char> *vVars)
{
    m_hVars = hVars;
    m_hVars = vVars;
    m_varsCount = hVars->size() + vVars->size();

    m_hVarsStr.clear();
    m_vVarsStr.clear();
    for (unsigned i = 0; i < hVars->size(); i++)
        m_hVarsStr.insert(0, hVars->at(i));
    for (unsigned i = 0; i < vVars->size(); i++)
        m_vVarsStr.insert(0, vVars->at(i));
}

void KMapBinaryVarsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(m_font);
    painter->setPen(QPen(Qt::black, LINE_SIZE));
    painter->drawLine(0, 0, SIZE, SIZE);

    painter->drawText(m_hVarsPoint, m_hVarsStr);
    painter->drawText(m_vVarsPoint, m_vVarsStr);
}



// Desc

KMapDescWidget::KMapDescWidget(int code, unsigned vc, qreal size, Mode m, Direction d, QGraphicsItem *parent)
        : KMapHeadWidget(m, d, parent)
{
    m_varsCount = vc;
    m_code = code;
    m_font = QFont("Monospace", FONT_SIZE, QFont::Normal);

    if (d == HORIZONTAL) {
        m_width = size;
        m_height = SIZE;
    }
    else {
        m_width = SIZE;
        m_height = size;
    }

    m_codeStr = QString("%1").arg(code, vc, 2, QChar('0'));

    setMinimumSize(m_width, m_height);
    setMaximumSize(minimumSize());
}

void KMapDescWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    if (m_mode == LINES) {
        painter->setPen(QPen(Qt::black, LINE_SIZE));

        int linePos = SIZE - LINE_SPACING;
        for (unsigned i = 0; i < m_varsCount; i++, linePos -= LINE_SPACING) {
            if (m_code & (1 << i)) {
                if (m_direction == HORIZONTAL)
                    painter->drawLine(0, linePos, m_width, linePos);
                else
                    painter->drawLine(linePos, 0, linePos, m_height);
            }
        }
    }
    else { // BINARY
        painter->setFont(m_font);
        if (m_direction == HORIZONTAL) {
            painter->drawText(QRectF(0, 0, m_width, m_height),
                              Qt::AlignHCenter | Qt::AlignBottom, m_codeStr);
        }
        else {
            painter->drawText(QRectF(0, 0, m_width - 4, m_height),
                              Qt::AlignVCenter | Qt::AlignRight, m_codeStr);
        }
    }
}

