/*
 * cubegldrawer.cpp - Configuration and constants of CubeGLDrawer
 * created date: 2008/05/18
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


#include "cubeglconf.h"

#include <QGLWidget>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

#define def_cf(_const)

CubeGLConf::CubeGLConf()
{
	// default constants
	data[CUBE_A].d            = 1.0;
	data[CUBE_2ND_DIFF].d     = 0.1;
	data[SPHERE_R].d          = 0.1;
	data[SPHERE_STRIPS].i     = 18;
	data[SPHERE_STACKS].i     = 18;
	data[MIN_SPHERE_R].d      = 0.02;
	data[MIN_SPHERE_STRIPS].i = 10;
	data[MIN_SPHERE_STACKS].i = 10;
	data[CYLINDER_R].d        = 0.02;
	data[CYLINDER_STRIPS].i   = 12;
	data[CYLINDER_STACKS].i   = 12;
	data[ROTATE_ANGLE].d      = 2.0;
	data[ZOOM_STEP].d         = 0.01;
	data[ZOOM_MIN].d          = 0.05;
	data[ZOOM_MAX].d          = 4.00;
	data[WALK_STEP].d         = 0.02;
	data[TURN_ANGLE].d        = 2.0;
	data[LIGHT_ANGLE].f       = 2.0;
	data[TIMER_CLOCK].i       = 25;
	data[PICKBOX_A].d         = 2.0;
	data[ANIMATE_STEP].d      = 0.01;
	data[DISPLAY_W].d         = 0.36;
	data[DISPLAY_H].d         = 0.12;
	data[DISPLAY_D].d         = 0.08;
	data[TERM_IMG_W].i        = 90;
	data[TERM_IMG_H].i        = 30;
	data[TERM_FONT_SIZE].i    = 18; // font size
	data[MSG_IMG_W].i         = 300;
	data[MSG_IMG_H].i         = 40;
	data[MSG_FONT_SIZE].i     = 20; // font size
	data[KT_CLOCK].i          = 30;
	data[KT_STOP_AFTER].i     = 50;

	cfm_def(cfm, CUBE_A);
	cfm_def(cfm, SPHERE_R);
	cfm_def(cfm, MIN_SPHERE_R);
	cfm_def(cfm, CYLINDER_R);
	cfm_def(cfm, DISPLAY_W);
	cfm_def(cfm, DISPLAY_H);
	cfm_def(cfm, DISPLAY_D);

	loadConf();
}

void CubeGLConf::loadConf()
{
	char buff[CONF_FILE_MAX_ROW], *p, *d;
	ifstream(fin);
	fin.open(CONF_FILE);
	std::map<const char *, int, ltstr>::iterator it;
	if (fin.is_open()) {
		while (fin.getline(buff, CONF_FILE_MAX_ROW)) {
			p = buff;
			while (*p != ' ') p++;
			*p = '\0';
			p++;
			if (*p != '=' || *(p + 1) != ' ') {
				continue;
			}
			d = ++p;
			if ((it = cfm.find(buff)) != cfm.end()) {
				while (!*p || *p != ' ') p++;
				data[(*it).second].d = strtod(d, &p);
			}

		}
	}

}

