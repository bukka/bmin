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

#ifndef KMAPWIDGET_H
#define KMAPWIDGET_H

#include "modulewidget.h"

#include <QString>

class GUIManager;
class Kernel;
class KMap;
class KMapGridWidget;
class TermsModel;
class CoversModel;
class QLabel;
class QCheckBox;
class QListView;
class QTableView;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsGridLayout;
class QGraphicsItem;


// Class for Karnaugh Map
class KMapWidget : public ModuleWidget
{
    Q_OBJECT

public:
    KMapWidget(const QString &name, int pos);
    ~KMapWidget();

private:
    GUIManager *m_gm;
    Kernel *m_kernel;
    KMap *m_kmap;
    // array with grid widget (for one variable count one widget)
    KMapGridWidget **m_gridWidgets;

    // SIDE
    TermsModel *m_termsModel;
    CoversModel *m_coversModel;
    QTableView *m_termsView;
    QTableView *m_coversView;
    QCheckBox *m_coversCheckBox;
    QLabel *m_termsLabel;
    QString m_termsStr;
    QString m_mintermsStr;
    QString m_maxtermsStr;

    // MAIN
    // actual widget in scene
    QGraphicsItem *m_mainItem;
    // scene
    QGraphicsScene *m_scene;
    // view
    QGraphicsView *m_view;

    bool m_active;
    unsigned m_varsCount;

public slots:
    void setActivity(bool active);
    void updateData();

private slots:
    void enableCovers(bool show);
};

#endif // KMAPWIDGET_H
