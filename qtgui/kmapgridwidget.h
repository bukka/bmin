/*
 * kmapgridwidget.h - K-map picture - grid layout of cells and headers
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

#ifndef KMAPGRIDWIDGET_H
#define KMAPGRIDWIDGET_H

#include "kmapheadwidget.h"

#include <QGraphicsWidget>
#include <QItemSelection>

class KMap;
class KMapCellWidget;
class Colors;
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
    void deselectAllTerms();
    
    const QColor &getColor(int pos, bool withAplpha = false);
    QRectF boundingRect() const;

private:
    void selectCover(const QModelIndexList &items, bool selected);

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

    Colors *m_colors;
    Colors *m_colorsAlpha;

public slots:
    void setMode(KMapHeadWidget::Mode mode);
    void enableCovers(bool show);
    void selectTerms(const QItemSelection &selected, const QItemSelection &deselected);
    void selectCovers(const QItemSelection &selected, const QItemSelection &deselected);

};


#endif // KMAPGRIDWIDGET_H
