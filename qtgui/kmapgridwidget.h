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

#ifndef KMAPGRIDWIDGET_H
#define KMAPGRIDWIDGET_H

#include "kmapheadwidget.h"

#include <QGraphicsWidget>
#include <QItemSelection>

class KMap;
class KMapCellWidget;
class QGraphicsItem;
class QGraphicsGridLayout;

// Class for k-map grid
class KMapGridWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    KMapGridWidget(KMap *kmap, bool showCovers, QGraphicsItem *parent = 0);
    ~KMapGridWidget();

    void setMapData(KMap *data);
    void showCovers();

private:
    bool m_showCovers;
    unsigned m_varsCount;
    unsigned m_rowsCount;
    unsigned m_colsCount;
    KMap *m_kmap;

    std::vector<char> m_vars;
    // selecting mode
    KMapHeadWidget::Mode m_mode;
    // main layout
    QGraphicsGridLayout *m_layout;
    // map of cell widgets
    KMapCellWidget **m_map;
    // head desc widgets
    KMapDescWidget **m_vDesc; // vertical
    KMapDescWidget **m_hDesc; // horizontal
    // head vars description widget
    KMapLineVarsWidget *m_vVars;
    KMapLineVarsWidget *m_hVars;
    KMapBinaryVarsWidget *m_binVars;

public slots:
    //void selectTerms(const QItemSelection &selected, const QItemSelection &deselected);
    //void selectCovers(const QItemSelection &selected, const QItemSelection &deselected);
    void setMode(KMapHeadWidget::Mode mode);
    void enableCovers(bool show);

};


#endif // KMAPGRIDWIDGET_H
