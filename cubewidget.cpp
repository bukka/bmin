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

#include <QWidget>
#include <QVBoxLayout>
#include <QGLFormat>

#include "cubewidget.h"
#include "cubegldrawer.h"
#include "term.h"
#include "guimanager.h"


// BooleanCube constructor - makes CubeGLDrawer, which draws - too much
// Boolean n-Cube. It connects all required signals and slots
CubeWidget::CubeWidget(const QString &name, int pos)
		: ModuleWidget(name, pos)
{
	// OpenGL format
	QGLFormat fmt;
	fmt.setDepth(true);
	fmt.setAlpha(true);
	CubeGLDrawer *drawer = new CubeGLDrawer(fmt);

	GUIManager *gm = GUIManager::instance();
	// changing one term in drawer
	connect(drawer, SIGNAL(cubeChanged(int, tval)),
			gm, SLOT(setTerm(int, tval)));
	// formula is minimized
	connect(drawer, SIGNAL(minRequested()),
			gm, SLOT(minimizeFormula()));
	// cube is focused
	connect(drawer, SIGNAL(cubeFocused(bool)),
			this, SLOT(showFocus(bool)));

	// creating new formula
	connect(gm, SIGNAL(formulaChanged(Formula *)),
			drawer, SLOT(setFormula(Formula *)));
	// request for minimizing cube
	connect(gm, SIGNAL(formulaMinimized()),
			drawer, SLOT(minimizeCube()));
	// request for invalidating cube
	connect(gm, SIGNAL(formulaInvalidated()),
			drawer, SLOT(invalidateCube()));
	// setting drawer activity
	connect(this, SIGNAL(activated(bool)),
			drawer, SLOT(setActivity(bool)));

	// for changing background color
	setPalette(QPalette(Qt::black));
	setAutoFillBackground(true);

	// place drawer to this widget
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(2, 2, 2, 2);
	mainLayout->addWidget(drawer);
	setLayout(mainLayout);

}

// make or remove border symbolized focusing of the drawer widget
void CubeWidget::showFocus(bool isFocused)
{
	setBackgroundRole(isFocused? QPalette::Highlight: QPalette::NoRole);
}

