/*
 * kmapgridwidget.cpp - K-map picture - grid layout of cells and headers
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

#include "kmapgridwidget.h"
#include "kmapcellwidget.h"
#include "colors.h"
#include "guimanager.h"
// kernel
#include "kmap.h"
#include "constants.h"

#include <list>
#include <algorithm>
using namespace std;

#include <QGraphicsGridLayout>

KMapGridWidget::KMapGridWidget(KMap *kmap, bool showCovers, QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    m_kmap = kmap;
    m_varsCount = kmap->getVarsCount();
    m_rowsCount = kmap->getRowsCount();
    m_colsCount = kmap->getColsCount();
    m_showCovers = showCovers;
    if (Constants::KMAP_LINES_DEFAULT)
        m_mode = KMapHeadWidget::LINES;
    else
        m_mode = KMapHeadWidget::BINARY;

    m_colors = new Colors(m_kmap->getCellsCount() / 2);
    m_colorsAlpha = new Colors(m_kmap->getCellsCount() / 2, 50);


    m_map = new KMapCellWidget *[m_rowsCount];
    for (unsigned i = 0; i < m_rowsCount; i++)
        m_map[i] = new KMapCellWidget[m_colsCount];

    m_layout = new QGraphicsGridLayout;
    m_layout->setSpacing(0);

    // head desc
    m_hDesc = new KMapDescWidget *[m_rowsCount];
    m_vDesc = new KMapDescWidget *[m_colsCount];

    for (unsigned i = 0; i < m_rowsCount; i++) {
        for (unsigned j = 0; j < m_colsCount; j++) {
            m_map[i][j].setGrid(this);
            m_map[i][j].setWalls(i == 0 || i == 4, i == m_rowsCount - 1 || i == 3,
                                 j == 0 || j == 4, j == m_colsCount - 1 || j == 3);
            m_map[i][j].setIdx(kmap->getIdx(i, j));
            connect(&m_map[i][j], SIGNAL(cellValueChanged(int, OutputValue &)),
                    GUIManager::instance(), SLOT(setTerm(int, OutputValue &)));
            m_layout->addItem(&m_map[i][j], i + 1, j + 1);
        }
        m_hDesc[i] = new KMapDescWidget(kmap->getSideGC(i), kmap->getRowsVarsCount(),
                                          m_map[i][0].getCellHeight(), m_mode,
                                          KMapHeadWidget::VERTICAL);
        connect(m_hDesc[i], SIGNAL(modeChanged(KMapHeadWidget::Mode)),
                this, SLOT(setMode(KMapHeadWidget::Mode)));
        m_layout->addItem(m_hDesc[i], i + 1, 0);
    }

    for (unsigned j = 0; j < m_colsCount; j++) {
        m_vDesc[j] = new KMapDescWidget(kmap->getTopGC(j), kmap->getColsVarsCount(),
                                        m_map[0][j].getCellWidth(), m_mode,
                                        KMapHeadWidget::HORIZONTAL);
        connect(m_vDesc[j], SIGNAL(modeChanged(KMapHeadWidget::Mode)),
                this, SLOT(setMode(KMapHeadWidget::Mode)));
        m_layout->addItem(m_vDesc[j], 0, j + 1);
    }

    // head vars
    m_binVars = new KMapBinaryVarsWidget(kmap->getTopVars(), kmap->getSideVars());
    m_hVars = new KMapLineVarsWidget(kmap->getTopVars(), KMapHeadWidget::HORIZONTAL);
    m_vVars = new KMapLineVarsWidget(kmap->getSideVars(), KMapHeadWidget::VERTICAL);

    connect(m_binVars, SIGNAL(modeChanged(KMapHeadWidget::Mode)),
            this, SLOT(setMode(KMapHeadWidget::Mode)));
    connect(m_hVars, SIGNAL(modeChanged(KMapHeadWidget::Mode)),
            this, SLOT(setMode(KMapHeadWidget::Mode)));
    connect(m_vVars, SIGNAL(modeChanged(KMapHeadWidget::Mode)),
            this, SLOT(setMode(KMapHeadWidget::Mode)));

    m_layout->addItem(m_binVars, 0, 0);
    m_layout->addItem(m_hVars, 0, m_colsCount + 1);
    m_layout->addItem(m_vVars, m_rowsCount + 1, 0);

    setLayout(m_layout);
    setMode(m_mode);
}

KMapGridWidget::~KMapGridWidget()
{
    delete m_colors;
    delete m_colorsAlpha;

    for (unsigned i = 0; i < KMap::MAX_ROWS; i++)
        delete [] m_map[i];
    delete [] m_map;

    delete [] m_vDesc;
    delete [] m_hDesc;

    delete m_binVars;
    delete m_hVars;
    delete m_vVars;
}

QRectF KMapGridWidget::boundingRect() const
{
    qreal width = m_colsCount * KMapCellWidget::SIZE + KMapHeadWidget::SIZE + 10;
    qreal height = m_rowsCount * KMapCellWidget::SIZE + KMapHeadWidget::SIZE + 10;

    return QRectF(0, 0, width, height);
}


void KMapGridWidget::setMapData(KMap *kmap)
{
    if (kmap->getVarsCount() != m_varsCount)
        return;

    m_kmap = kmap;

    // cells
    for (unsigned i = 0; i < m_rowsCount; i++) {
        for (unsigned j = 0; j < m_colsCount; j++)
            m_map[i][j].setValue(kmap->getCellValue(i, j).toChar());
    }

    // vars
    m_hVars->setVars(kmap->getTopVars());
    m_vVars->setVars(kmap->getSideVars());
    m_binVars->setVars(kmap->getTopVars(), kmap->getSideVars());

    if (m_showCovers)
        showCovers();

    update();
}

void KMapGridWidget::showCovers()
{
    list<KMapCover> *covers = m_kmap->getMinCovers();
    if (!covers)
        return;

    int i = 0;
    for (list<KMapCover>::iterator itCover = covers->begin(); itCover != covers->end(); itCover++) {
        list<KMapCell> *cells = (*itCover).getCells();
        int pos = 0;
        for (list<KMapCell>::iterator itCell = cells->begin(); itCell != cells->end(); itCell++) {
            (*itCell).setColor(i);
            pos = m_map[(*itCell).getRow()][(*itCell).getCol()].addCover(*itCell, pos);
        }
        if (pos != 0) {
            for (list<KMapCell>::iterator itCell = cells->begin(); itCell != cells->end(); itCell++)
                m_map[(*itCell).getRow()][(*itCell).getCol()].setLastCoverPos(pos);
        }
        i++; // next color
    }

}

const QColor &KMapGridWidget::getColor(int pos, bool withAlpha)
{
    return withAlpha? (*m_colorsAlpha)[pos]: (*m_colors)[pos];
}

void KMapGridWidget::setMode(KMapHeadWidget::Mode mode)
{
    m_mode = mode;
    for (unsigned i = 0; i < m_rowsCount; i++)
        m_hDesc[i]->setMode(mode);
    for (unsigned i = 0; i < m_colsCount; i++)
        m_vDesc[i]->setMode(mode);

    m_hVars->setVisible(mode == KMapHeadWidget::LINES);
    m_vVars->setVisible(mode == KMapHeadWidget::LINES);
    m_binVars->setVisible(mode == KMapHeadWidget::BINARY);
}

void KMapGridWidget::enableCovers(bool show)
{
    m_showCovers = show;
    setMapData(m_kmap);
}

void KMapGridWidget::selectTerms(const QItemSelection &selected, const QItemSelection &deselected)
{
    unsigned row, col;
    QModelIndex index;

    QModelIndexList items = selected.indexes();
    foreach (index, items) {
        m_kmap->getRowCol(index.model()->data(index, Qt::UserRole).toInt(), row, col);
        m_map[row][col].setSelection(true);
    }

    items = deselected.indexes();
    foreach (index, items) {
        m_kmap->getRowCol(index.model()->data(index, Qt::UserRole).toInt(), row, col);
        m_map[row][col].setSelection(false);
    }
    update();
}

void KMapGridWidget::selectCover(const QModelIndexList &items, bool selected)
{
    list<KMapCover> *covers = m_kmap->getMinCovers();
    if (!covers)
        return;

    QModelIndex index;
    foreach (index, items) {
        KMapCover foundCover(index.model()->data(index, Qt::UserRole).toInt());
        list<KMapCover>::iterator it = find(covers->begin(), covers->end(), foundCover);
        if (it == covers->end())
            return;
        list<KMapCell> *cells = (*it).getCells();
        for (list<KMapCell>::iterator itCell = cells->begin(); itCell != cells->end(); itCell++) {
            unsigned row = (*itCell).getRow();
            unsigned col = (*itCell).getCol();
            m_map[row][col].selectCover(*itCell, selected);
        }
    }
}

void KMapGridWidget::selectCovers(const QItemSelection &selected, const QItemSelection &deselected)
{
    selectCover(selected.indexes(), true);
    selectCover(deselected.indexes(), false);

    update();
}
