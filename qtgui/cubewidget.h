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
class Cube;
class TermsModel;
class CoversModel;
class QLabel;
class QPushButton;
class QCheckBox;
class QTableView;
class QAbstractItemModel;
class QItemSelection;

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
    void deselectAll(QTableView *view, const QAbstractItemModel *model);

    bool m_active;
    unsigned m_varsCount;
    int m_tourSteps;
    int m_tourPos;
    CubeGLDrawer *m_drawer;
    GUIManager *m_gm;
    Cube *m_cube;

    TermsModel *m_termsModel;
    CoversModel *m_coversModel;
    QTableView *m_termsView;
    QTableView *m_coversView;
    QCheckBox *m_coversCheckBox;
#if CUBE_TEXTURES
    QString m_tourStartStr;
    QString m_tourStopStr;
    QPushButton *m_tourBtn;
    QPushButton *m_tourPrevBtn;
    QPushButton *m_tourNextBtn;
#endif
    QLabel *m_termsLabel;
    QString m_termsStr;
    QString m_mintermsStr;
    QString m_maxtermsStr;

public slots:
    void setActivity(bool active);
    void updateData();
    void invalidateData();
    void setRepre(bool isSoP);
    void enableCovers(bool show);
#if CUBE_TEXTURES
    // tour slots
    void startTour(int covers);
    void stopTour();
    void shiftTour(int steps);
#endif
private slots:
    void selectTerms(const QItemSelection &selected, const QItemSelection &deselected);
    void selectCovers(const QItemSelection &selected, const QItemSelection &deselected);

signals:
    void activated(bool); // activity of cube was changed

};

#endif // CUBEWIDGET_H

