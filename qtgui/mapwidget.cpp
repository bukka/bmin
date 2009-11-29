/*
 * mapwidget.cpp - K-map
 * created date: 2007/12/29
 *
 * Copyright (C) 2007 Jakub Zelenka.
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

#include "mapwidget.h"
#include "mapcell.h"
#include "guimanager.h"

#include "kernel/formula.h"
#include "kernel/term.h"
#include "kernel/outputvalue.h"

#include <QPalette>
#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QString>
#include <QChar>
#include <QPoint>

#include <vector>

using namespace std;

// maping cell index in karnaugh map
// for number of variables greater 2
static int karnaughG2[] = {
    0,4,1,5,3,7,2,6,
    12,8,13,9,15,11,14,10,
    24,28,25,29,27,31,26,30,
    20,16,21,17,23,19,22,18,
    34,38,35,39,33,37,32,36,
    46,42,47,43,45,41,44,40,
    58,62,59,63,57,61,56,60,
    54,50,55,51,53,49,52,48
};

// maping cell index in karnaugh map
// for number of variables equal 2
static int karnaughE2[] = {0,2,1,3};

// maping position of mouse in karnaugh map
static int mouseMap[] = {
    0,1,3,2,34,35,33,32,
    4,5,7,6,38,39,37,36,
    12,13,15,14,46,47,45,44,
    8,9,11,10,42,43,41,40,
    24,25,27,26,58,59,57,56,
    28,29,31,30,62,63,61,60,
    20,21,23,22,54,55,53,52,
    16,17,19,18,50,51,49,48
};

// karnaugh2G inverse
int karG2ToPos(int kar)
{
    for (int i = 0; i < 64; i++)
        if (karnaughG2[i] == kar)
            return i;
    return -1;
}

// karnaugh2E inverse
int karE2ToPos(int kar)
{
    for (int i = 0; i < 4; i++)
        if (karnaughE2[i] == kar)
            return i;
    return -1;
}

// maping of karnaugh map position and cell index
// for number of variables equal 1
int karE1ToPos(int kar)
{
    if (kar < 0 || kar > 1)
        return -1;
    return kar;
}


MapWidget::MapWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    isActive = true;
    msg = MSG_INVALID;

    mapSize = 0;
    cellSize = MapCell::DEF_SIZE;
    cellBorder = MapCell::DEF_BORDER;

    setPalette(QPalette(QColor(Qt::white)));
    setAutoFillBackground(true);

    GUIManager *gm = GUIManager::instance();
    // changing term's value in map
    connect(this, SIGNAL(mapChanged(int, OutputValue &)),
        gm, SLOT(setTerm(int, OutputValue &)));
    // changing formula in parent
    connect(gm, SIGNAL(formulaChanged(Formula *)),
        this, SLOT(setMapData(Formula *)));
    // info from parent that formula is invalid
    connect(gm, SIGNAL(formulaInvalidated()),
        this, SLOT(invalidateMap()));
}

void drawNegLine(QPainter & painter, int startX, int startY,
        int endX, int endY, Qt::GlobalColor color)  {
    painter.setPen(QPen(QBrush(color),2.0));
    painter.drawLine(startX,startY,endX,endY);
}

void MapWidget::showMsg(QPainter &painter, QString &text)
{
    int size = 14;
    bool isCorrect;
    QFont font;

    do {
        font = QFont("Monospace", size);
        QFontMetrics fm(font);

        isCorrect = fm.width(text) < (width() - 50);
        size--;
    } while (!isCorrect && size >= 4);


    painter.setFont(font);
    painter.setPen(QColor(Qt::red));
    painter.drawText(0, 0, width(), height(),
        Qt::AlignCenter, text);

}

void MapWidget::paintEvent(QPaintEvent * /* event */)
{
    if (!isActive)
        return;

    QPainter painter(this);

    if (msg != MSG_NONE) {
        QString msgText = ((msg == MSG_INVALID)?
            tr("The formula is unknown."):
            QString(tr("The maximum number of variables in Karnaugh "
                "map is ")) + QString::number(MAX_SIZE) + '.');
        showMsg(painter, msgText);
        return;
    }

    int x,y,endX,endY,legX,legY;
    int spac = 10;
    QString var;

    // drawing variables lines - example: f(f,e,d,c,b,a)
    if (mapSize == 1) {
        painter.translate(transX,0);
        y = transY - spac;
        drawNegLine(painter,cellSize,y,2*cellSize,y,Qt::darkRed);

        // show legend
        painter.drawText(2*cellSize+5,y+3,QString(QChar(vars[0])));

        painter.translate(-transX,transY);
    }
    else if (mapSize == 2) {
        painter.translate(transX,0);
        // a
        y = transY - spac;
        drawNegLine(painter,cellSize,y,2*cellSize,y,Qt::darkRed);

        // show legend
        painter.drawText(2*cellSize+5,y+3,QString(QChar(vars[1])));

        // b
        x = transX - spac;
        painter.translate(-transX,transY);
        drawNegLine(painter,x,cellSize,x,2*cellSize,Qt::darkGreen);

        // show legend
        painter.drawText(x-3,2*cellSize+15,QString(QChar(vars[0])));
    }
    else if (mapSize > 2) {
        // horizontal negations
        painter.translate(transX,0);

        // a
        y = transY - spac;
        drawNegLine(painter,cellSize,y,3*cellSize,y,Qt::darkRed);
        if (mapSize == 6) {
            endX = 6 * cellSize; // end of b line
            legX = 8 * cellSize + 5;

            drawNegLine(painter,5*cellSize,y,7*cellSize,y,Qt::darkRed);
            // legend a
            painter.drawText(legX,y+5, QString(QChar(vars[vars.size()-1])));

            // f
            y -= 2 * spac;
            drawNegLine(painter,4*cellSize,y,8*cellSize,y,Qt::darkBlue);

            // legend f
            painter.drawText(legX,y+1,QString(QChar(vars[0])));
        }
        else {
            endX = 4 * cellSize;
            legX = endX + 5;
            // legend a
            painter.drawText(legX,y+5,
                QString(QChar(vars[vars.size()-1])));
        }

        // b
        y = transY - 2 * spac;
        drawNegLine(painter,2*cellSize,y,endX,y,Qt::darkGreen);
        // legend b
        painter.drawText(legX,y+3,QString(QChar(vars[vars.size()-2])));



        // vertical negations
        painter.translate(-transX,transY);

        // c
        x = transX - spac;
        drawNegLine(painter,x,cellSize,x,
            ((mapSize == 3)? 2:3)*cellSize,Qt::darkYellow);
        if (mapSize >= 5) {
            endY = 6 * cellSize; // end of d line
            legY = 8 * cellSize + 15;

            // c
            drawNegLine(painter,x,5*cellSize,x,7*cellSize,
                Qt::darkYellow);
            // legend c
            painter.drawText(x-3,legY,
                QString(QChar(vars[vars.size()-3])));

            // e
            x -= 2 * spac;
            drawNegLine(painter,x,4*cellSize,x,8*cellSize,Qt::darkCyan);

            // legend e
            painter.drawText(x-3,legY, QString(QChar(vars[vars.size()-5])));
        }
        else {
            endY = 4 * cellSize;
            legY = endY + 15;
        }

        if (mapSize > 3) {
            if (mapSize == 4)
                painter.drawText(x-3,legY, QString(QChar(vars[vars.size()-3])));

            // d
            x = transX - 2 * spac;
            drawNegLine(painter,x,2*cellSize,x,endY,Qt::darkMagenta);
            painter.drawText(x-3,legY,
                QString(QChar(vars[vars.size()-4])));

        }
        else
            painter.drawText(x-3,2*cellSize+15,
                QString(QChar(vars[vars.size()-3])));
    }

    // drawing karnaugh map
    painter.translate(transX, 0);
    painter.setBrush(Qt::NoBrush);
    for (unsigned i = 0; i < cells.size(); i++)
        cells[i]->draw(painter);

}


int power(int x, int y)
{
    int ret = 1;
    for (int i = 0; i < y; i++)
        ret *= x;
    return ret;
}

inline int min(int a, int b)
{
    return (a < b)? a: b;
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint trans(transX,transY);
    QPoint pos = event->pos();
    pos -= trans;

    if (pos.x() < 0 || pos.x() > cellSize*8 ||
            pos.y() < 0 || pos.y() > cellSize*8)
        return;
    int idx = mouseMap[(pos.y() / cellSize) * 8 + pos.x() / cellSize];

    //cout << "X: " << pos.x() << "; Y: " << pos.y() << endl;

    if (mapSize == 2 && (idx == 4 || idx == 5))
        idx -= 2;
    else if (idx >= power(2,mapSize))
        return;


    int cellIdx;
    if (mapSize == 1)
        cellIdx = idx;
    else if (mapSize == 2)
        cellIdx = karE2ToPos(idx);
    else
        cellIdx = karG2ToPos(idx);

    int oldValue = cells[cellIdx]->getValue();
    int newValue = MapCell::getNextValue(oldValue);
    cells[cellIdx]->setValue(newValue);
    updateMap();

    OutputValue outVal;
    switch (newValue) {
        case MapCell::ZERO:
            outVal = OutputValue::ZERO;
            break;
        case MapCell::ONE:
            outVal = OutputValue::ONE;
            break;
        case MapCell::DONT_CARE:
            outVal = OutputValue::DC;
            break;
        default:
            return;
    }
    emit mapChanged(idx, outVal);

}


void MapWidget::invalidateMap()
{
    msg = MSG_INVALID;
    updateMap();
}

void MapWidget::setMapData(Formula *formula)
{
    int size;
    if (!formula ||	(size = formula->getVarsCount()) > MAX_SIZE) {
        mapSize = 0;
        msg = MSG_OVER;
        updateMap();
        return;
    }

    msg = MSG_NONE;
    vector<char> revVars = formula->getVars();
    vars.clear();
    for (int i = int(revVars.size()) - 1; i >= 0; i--)
        vars.push_back(revVars[i]);

    int zero = MapCell::ZERO;

    if (size != mapSize) {
        if (!cells.empty())
            clearCells();
        mapSize = size;
        int x,y,n,i,j,k;
        x = y = 0;

        if (size == 1) {
            cells.push_back(new MapCell(x,y,cellSize,cellBorder,zero));
            cells.push_back(new MapCell(x+cellSize,y,cellSize,cellBorder,zero));
        }
        else if (size > 1) {
            n = power(2,size);
            for (i = 0; i < min(n,64); i += 32) {
                for (j = 0; j < min(n,32); j += 8) {
                    for (k = 0; k < min(n,8); k += 2) {
                        cells.push_back(new MapCell(x,y,cellSize,cellBorder,zero));
                        y += cellSize;
                        cells.push_back(new MapCell(x,y,cellSize,cellBorder,zero));
                        x += cellSize;
                        y -= cellSize;
                    }
                    x -= 4 * cellSize;
                    y += 2 * cellSize;
                }
                x += 4 * cellSize;
                y -= 8 * cellSize;
            }
        }
    }
    else
        setAllValues(zero);

    std::vector<int> idx;
    setValues(formula->getTermsIdx(idx, OutputValue::ONE), MapCell::ONE,size);
    setValues(formula->getTermsIdx(idx, OutputValue::DC), MapCell::DONT_CARE, size);


    updateMap();
}

void MapWidget::setActivity(bool active)
{
    isActive = active;
}

void MapWidget::setAllValues(int value)
{
    for (unsigned i = 0; i < cells.size(); i++)
        cells[i]->setValue(value);
}

void MapWidget::setValues(std::vector<int> idx, int value, int size)
{
    int (* kar)(int);
    if (size == 1)
        kar = karE1ToPos;
    else if (size == 2)
        kar = karE2ToPos;
    else /* if (size > 2) */
        kar = karG2ToPos;

    for (unsigned i = 0; i < idx.size(); i++)
        cells[kar(idx[i])]->setValue(value);
}

void MapWidget::clearCells()
{
    for (unsigned i = 0; i < cells.size(); i++)
        delete cells[i];
    cells.clear();
}

