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

#include "kmapgridwidget.h"
#include "kmapcellwidget.h"
#include "guimanager.h"
// kernel
#include "kmap.h"
#include "constants.h"

#include <list>
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
            m_map[i][j].setWalls(i == 0 || i == 4, i == m_rowsCount - 1 || i == 3,
                                 j == 0 || j == 4, j == m_colsCount - 1 || j == 3);
            m_map[i][j].setIdx(kmap->getIdx(i, j));
            connect(&m_map[i][j], SIGNAL(cellValueChanged(int, OutputValue &)),
                    GUIManager::instance(), SLOT(setTerm(int, OutputValue&)));
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
    for (unsigned i = 0; i < KMap::MAX_ROWS; i++)
        delete [] m_map[i];
    delete [] m_map;

    delete [] m_vDesc;
    delete [] m_hDesc;

    delete m_binVars;
    delete m_hVars;
    delete m_vVars;
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
    else {
        for (list<KMapCover>::iterator itCover = covers->begin(); itCover != covers->end(); itCover++) {
            list<KMapCell> *cells = (*itCover).getCells();
            int pos = 0;
            for (list<KMapCell>::iterator itCell = cells->begin(); itCell != cells->end(); itCell++)
                pos = m_map[(*itCell).getRow()][(*itCell).getCol()].addCover(*itCell, pos);
            if (pos != 0) {
                for (list<KMapCell>::iterator itCell = cells->begin(); itCell != cells->end(); itCell++)
                    m_map[(*itCell).getRow()][(*itCell).getCol()].setLastCoverPos(pos);
            }
        }
    }
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
