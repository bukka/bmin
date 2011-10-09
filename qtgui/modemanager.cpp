/*
 * modemanager.cpp - modes manager (selecting mode...)
 * created date: 8/4/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#include "modemanager.h"
#include "guimanager.h"
#include "welcomewidget.h"
#include "kmapwidget.h"
#include "cubewidget.h"
#include "qmwidget.h"
#include "qmwebkitwidget.h"

#include <QList>

// preinicialize static instance
ModeManager *ModeManager::s_instance = 0;

// constructor - init modes
ModeManager::ModeManager()
{
    GUIManager *gm = GUIManager::instance();
    connect(gm, SIGNAL(modeChanged(int)), this, SLOT(setMode(int)));

    // welcome
    m_welcome = new WelcomeWidget(tr("Welcome"), ID_WELCOME);
    m_modules << m_welcome;

    // K-map
    m_kmap = new KMapWidget(tr("K-Map"), ID_KMAP);
    m_modules << m_kmap;
    connect(this, SIGNAL(kmapActivated(bool)), m_kmap, SLOT(setActivity(bool)));

#if CUBE3D
    // Boolean n-cube
    m_cube = new CubeWidget(tr("n-Cube"), ID_CUBE);
    m_modules << m_cube;
    connect(this, SIGNAL(cubeActivated(bool)), m_cube, SLOT(setActivity(bool)));
#endif

    // Quine-McCluskey - webkit
    m_qmw = new QmWebkitWidget(tr("Quine-McCluskey"), ID_QM_WEBKIT);
    m_modules << m_qmw;
    connect(this, SIGNAL(qmwActivated(bool)), m_qmw, SLOT(setActivity(bool)));

    // Quine-McCluskey - text widget
    // m_qm = new QmWidget(tr("QM"), ID_QM);
    // m_modules << m_qm;
    // connect(this, SIGNAL(qmActivated(bool)), m_qm, SLOT(setActivity(bool)));
}

// return instance of ModeManager
ModeManager *ModeManager::instance()
{
    if (s_instance == 0)
        s_instance = new ModeManager();

    return s_instance;
}

// delete and clear static instance of ModeManager
void ModeManager::destroy()
{
    delete s_instance;
    s_instance = 0;
}

// returns all modes
QList<Mode> ModeManager::modes()
{
    QList<Mode> modesList;
    modesList << Mode(ID_WELCOME, tr("Welcome"));
    modesList << Mode(ID_KMAP, tr("Karnaugh Map"));
#if CUBE3D
    modesList << Mode(ID_CUBE, tr("Boolean n-Cube"));
#endif
    modesList << Mode(ID_QM_WEBKIT, tr("QM Webkit"));
    // modesList << Mode(ID_QM, tr("Quine-McCluskey"));
    return modesList;
}

// set mode
void ModeManager::setMode(int modeId)
{
    emit kmapActivated(modeId == ID_KMAP);
    // emit qmActivated(modeId == ID_QM);
    emit qmwActivated(modeId == ID_QM_WEBKIT);
#if CUBE3D
    emit cubeActivated(modeId == ID_CUBE);
#endif
    emit modeChanged(modeId);
}
