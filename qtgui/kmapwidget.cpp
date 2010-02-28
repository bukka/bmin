/*
 *
 * created date: 2/20/2010
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

#include "kmapwidget.h"
#include "guimanager.h"
// kernel
#include "kernel.h"
#include "kmap.h"

#include <vector>
using namespace std;

#include <QPainter>
#include <QHBoxLayout>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsSceneContextMenuEvent>


// HEAD

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
    m_font = QFont("Times", FONT_SIZE, QFont::Normal);

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

// CELL

KMapCellWidget::KMapCellWidget(QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    m_cellWidth = m_cellHeight = SIZE;
    m_penSize = BORDER / 2;
    m_cellSize = static_cast<int>(SIZE);
    m_wallSize = static_cast<int>(BORDER);
    m_font = QFont("Times", FONT_SIZE, QFont::Normal);
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
}

void KMapCellWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(m_font);
    painter->setPen(QPen(Qt::black, m_penSize));

    unsigned x = (m_leftWall? m_wallSize: 0);
    unsigned y = (m_topWall? m_wallSize: 0);
    int width = m_cellSize - m_penSize;
    int height = m_cellSize - m_penSize;

    painter->drawRect(x, y, width, height);

    if (m_topWall)
        painter->drawLine(x - 1, y - m_penSize, x + 1 + width, y - m_penSize);
    if (m_bottomWall)
        painter->drawLine(x, y + height + m_penSize, x + 1 + width, y + height + m_penSize);
    if (m_leftWall)
        painter->drawLine(x - m_penSize, y - 1, x - m_penSize, y + 1 + height);
    if (m_rightWall)
        painter->drawLine(x + height + m_penSize, y - 1, x + height + m_penSize, y + 1 + height);

    if (!m_valueStr.isEmpty())
        painter->drawText(x, y, width, height, Qt::AlignCenter, m_valueStr);
}

void KMapCellWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        OutputValue value = OutputValue::getNextValue(OutputValue(m_value));
        emit cellValueChanged(m_idx, value);
    }
}

// GRID

KMapGridWidget::KMapGridWidget(KMap *kmap, KMapHeadWidget::Mode mode, QGraphicsItem *parent)
        : QGraphicsWidget(parent)
{
    m_varsCount = kmap->getVarsCount();
    m_rowsCount = kmap->getRowsCount();
    m_colsCount = kmap->getColsCount();

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
                                          m_map[i][0].getCellHeight(), mode,
                                          KMapHeadWidget::VERTICAL);
        connect(m_hDesc[i], SIGNAL(modeChanged(KMapHeadWidget::Mode)),
                this, SLOT(setMode(KMapHeadWidget::Mode)));
        m_layout->addItem(m_hDesc[i], i + 1, 0);
    }
    
    for (unsigned j = 0; j < m_colsCount; j++) {
        m_vDesc[j] = new KMapDescWidget(kmap->getTopGC(j), kmap->getColsVarsCount(),
                                        m_map[0][j].getCellWidth(), mode,
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
    setMode(mode);
}

KMapGridWidget::~KMapGridWidget()
{
    for (int i = 0; i < KMap::MAX_ROWS; i++)
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

    // cells
    for (unsigned i = 0; i < m_rowsCount; i++) {
        for (unsigned j = 0; j < m_colsCount; j++)
            m_map[i][j].setValue(kmap->getCellValue(i, j).toChar());
    }

    // vars
    m_hVars->setVars(kmap->getTopVars());
    m_vVars->setVars(kmap->getSideVars());
    m_binVars->setVars(kmap->getTopVars(), kmap->getSideVars());
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

// MAP

KMapWidget::KMapWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    m_kernel = Kernel::instance();
    m_gm = GUIManager::instance();
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));

    m_active = false;
    m_varsCount = 0;
    m_kmap = 0;
    m_mode = DEFAULT_MODE;

    // grid widget array
    m_gridWidgets = new KMapGridWidget *[KMap::MAX_VARS + 1];
    for (int i = 0; i <= KMap::MAX_VARS; i++)
        m_gridWidgets[i] = 0;
    m_mainItem = 0;

    // scene
    m_scene = new QGraphicsScene;
    // view
    m_view = new QGraphicsView(m_scene);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_view);

    setLayout(layout);
}

KMapWidget::~KMapWidget()
{
    delete [] m_gridWidgets;
}

void KMapWidget::setActivity(bool active)
{
    m_active = active;
    if (active)
        updateData();
}

void KMapWidget::updateData()
{
    if (m_active) {
        m_kmap = m_kernel->getKMap();
        if (m_varsCount != m_kmap->getVarsCount()) {
            m_varsCount = m_kmap->getVarsCount();
            if (!m_gridWidgets[m_varsCount]) {
                m_gridWidgets[m_varsCount] = new KMapGridWidget(m_kmap, m_mode);
            }
            m_gridWidgets[m_varsCount]->setMapData(m_kmap);

            if (m_mainItem)
                m_scene->removeItem(m_mainItem);
            m_mainItem = m_gridWidgets[m_varsCount];
            m_scene->addItem(m_mainItem);
            m_view->update();
        }
        else if (m_gridWidgets[m_varsCount]) {
            m_gridWidgets[m_varsCount]->setMapData(m_kmap);
            m_gridWidgets[m_varsCount]->update();
        }
    }

}
