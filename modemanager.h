/*
 * modemanager.h - modes manager (selecting mode...)
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

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <QObject>
#include <QList>

class ModuleWidget;
class WelcomeWidget;
class MapWidget;
class CubeWidget;

// mode manager class
class ModeManager : public QObject
{
	Q_OBJECT

public:
	// singleton method for returning instance of ModeManager
	static ModeManager *instance();
	// destroy instance method
	static void destroy();

	// modes' identifiers
	static const int WELCOME_ID = 0;
	static const int MAP_ID     = 1;
#ifdef Q_WS_WIN
	static const int CUBE_ID    = 2;
#endif

	// return modules
	QList<ModuleWidget *> getModules() const { return m_modules; }

	// static instance of ModeManager
	static ModeManager *s_instance;

private:
	ModeManager();

	WelcomeWidget *m_welcome;
	MapWidget *m_map;
#ifdef Q_WS_WIN
	CubeWidget *m_cube;
#endif
	QList<ModuleWidget *> m_modules;

public slots:
	void setMode(int);

signals:
	// emitted when mode is changed
	void modeChanged(int);
	void mapActivated(bool);
#ifdef Q_WS_WIN
	void cubeActivated(bool);
#endif
};

#endif // MODEMANAGER_H
