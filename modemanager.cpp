/*
 * modemanager.cpp - modes manager (selecting mode...)
 * created date: 2009/08/04
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

#include "modemanager.h"
#include "welcomewidget.h"
#include "mapwidget.h"
#include "cubewidget.h"

#include <QList>

// preinicialize static instance
ModeManager *ModeManager::s_instance = 0;

// constructor - init modes
ModeManager::ModeManager()
{
	// welcome
	m_welcome = new WelcomeWidget(tr("Welcome"), 0);
	m_modules << m_welcome;

	// map
	m_map = new MapWidget(tr("K-map"), 1);
	m_modules << m_map;
	connect(this, SIGNAL(mapActivated(bool)), m_map, SLOT(setActivity(bool)));

#ifdef Q_WS_WIN
	// cube
	m_cube = new CubeWidget(tr("Boolean n-Cube"), 2);
	m_modules << m_cube;
	connect(this, SIGNAL(cubeActivated(bool)), m_cube, SIGNAL(activated(bool)));
#endif
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

// set mode
void ModeManager::setMode(int modeId)
{
	emit mapActivated(modeId == MAP_ID);
#ifdef Q_WS_WIN
	emit cubeActivated(modeId == CUBE_ID);
#endif
	emit modeChanged(modeId);
}
