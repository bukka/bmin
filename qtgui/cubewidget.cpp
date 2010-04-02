/*
 * cubewidget.h - Boolean n-Cube wrapper
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

#include "cubewidget.h"
#include "cubegldrawer.h"
#include "borderwidget.h"
#include "guimanager.h"

#include <QHBoxLayout>


CubeWidget::CubeWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    // OpenGL format
    QGLFormat fmt;
    fmt.setDepth(true);
    fmt.setAlpha(true);

    // drawer
    m_drawer = new CubeGLDrawer(fmt);
    GUIManager *gm = GUIManager::instance();
    // changing one term in drawer
    connect(m_drawer, SIGNAL(cubeChanged(int, OutputValue &)),
            gm, SLOT(setTerm(int, OutputValue &)));
    // minimizing formula from drawer
    connect(m_drawer, SIGNAL(minRequested()), gm, SLOT(minimizeFormula()));

    // creating new formula
    connect(gm, SIGNAL(formulaChanged()), m_drawer, SLOT(reloadCube()));
    // request for minimizing cube
    connect(gm, SIGNAL(formulaMinimized()), m_drawer, SLOT(minimizeCube()));
    // request for invalidating cube
    connect(gm, SIGNAL(formulaInvalidated()), m_drawer, SLOT(invalidateCube()));
    // setting drawer activity
    connect(this, SIGNAL(activated(bool)), m_drawer, SLOT(setActivity(bool)));

    QHBoxLayout *glLayout = new QHBoxLayout;
    glLayout->setMargin(0);
    glLayout->addWidget(m_drawer);

    BorderWidget *borderWidget = new BorderWidget;
    borderWidget->setLayout(glLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(borderWidget);

    setLayout(mainLayout);

    setFocusPolicy(Qt::StrongFocus);
}

void CubeWidget::keyPressEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyPressEvent(event);
}

void CubeWidget::keyReleaseEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyReleaseEvent(event);
}

