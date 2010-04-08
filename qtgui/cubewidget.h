/*
 * cubewidget.h - Boolean n-Cube superstructure for cube drawer
 * created date: 2008/03/26
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
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

#ifndef CUBEWIDGET_H
#define CUBEWIDGET_H

#include "modulewidget.h"

class CubeGLDrawer;
class GUIManager;
class KMap;
class KMapGridWidget;
class TermsModel;
class CoversModel;
class QLabel;
class QCheckBox;
class QTableView;

// BooleanCube class - superstructure for cube drawer
class CubeWidget : public ModuleWidget
{
    Q_OBJECT

public:
    static const int SIDEBAR_SIZE = 150;

    // CubeWidget constructor
    CubeWidget(const QString &name, int pos);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    CubeGLDrawer *m_drawer;
    GUIManager *m_gm;

    TermsModel *m_termsModel;
    CoversModel *m_coversModel;
    QTableView *m_termsView;
    QTableView *m_coversView;
    QCheckBox *m_coversCheckBox;
    QLabel *m_termsLabel;
    QString m_termsStr;
    QString m_mintermsStr;
    QString m_maxtermsStr;

signals:
    void activated(bool); // activity of cube was changed

};

#endif // CUBEWIDGET_H

