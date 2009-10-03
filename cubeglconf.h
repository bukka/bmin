/*
 * cubeglconf.h - Configuration and constants for CubeGLDrawer
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

#ifndef CUBEGLCONF_H
#define CUBEGLCONF_H

#include <QGLWidget>
#include <map>
#include <string.h>

#define CONF_FILE "conf/gl_cube.txt"
#define CONF_FILE_MAX_ROW 50

#define CF_CUBE_A       "cube-size"
#define CF_SPHERE_R     "sphere-radius"
#define CF_MIN_SPHERE_R "animated-sphere-radius"
#define CF_CYLINDER_R   "line-radius"
#define CF_DISPLAY_W    "display-width"
#define CF_DISPLAY_H    "display-height"
#define CF_DISPLAY_D    "display-deep"

#define cfm_def(_map, _value) (_map[CF_ ## _value] = _value)

class CubeGLConf
{
public:
	CubeGLConf();

	int getI(int type) const { return data[type].i; }
	GLdouble getD(int type) const { return data[type].d; }
	GLfloat getF(int type) const { return data[type].f; }

	static const int CUBE_A            = 0;
	static const int CUBE_2ND_DIFF     = 1;
	static const int SPHERE_R          = 2;
	static const int SPHERE_STRIPS     = 3;
	static const int SPHERE_STACKS     = 4;
	static const int MIN_SPHERE_R      = 5;
	static const int MIN_SPHERE_STRIPS = 6;
	static const int MIN_SPHERE_STACKS = 7;
	static const int CYLINDER_R        = 8;
	static const int CYLINDER_STRIPS   = 9;
	static const int CYLINDER_STACKS   = 10;
	static const int ROTATE_ANGLE      = 11;
	static const int ZOOM_STEP         = 12;
	static const int ZOOM_MIN 	       = 14;
	static const int ZOOM_MAX          = 15;
	static const int WALK_STEP         = 16;
	static const int TURN_ANGLE        = 17;
	static const int LIGHT_ANGLE       = 18;
	static const int TIMER_CLOCK       = 19;
	static const int PICKBOX_A         = 20;
	static const int ANIMATE_STEP      = 21;
	static const int DISPLAY_W         = 22;
	static const int DISPLAY_H         = 23;
	static const int DISPLAY_D         = 24;
	static const int TERM_IMG_W        = 25;
	static const int TERM_IMG_H        = 26;
	static const int TERM_FONT_SIZE    = 27; // font size
	static const int MSG_IMG_W         = 28;
	static const int MSG_IMG_H         = 29;
	static const int MSG_FONT_SIZE     = 30; // font size
	static const int KT_CLOCK          = 31; // Key Timers clock
	static const int KT_STOP_AFTER     = 32; // stop interval

	static const int CONSTANTS_COUNT   = 33;

	// key timers
	static const int KTR_UP       = 0;
	static const int KTR_DOWN     = 1;
	static const int KTR_LEFT     = 2;
	static const int KTR_RIGHT    = 3;
	static const int KTM_UP       = 4;
	static const int KTM_DOWN     = 5;
	static const int KTM_LEFT     = 6;
	static const int KTM_RIGHT    = 7;
	static const int KTM_STRAIGHT = 8;
	static const int KTM_BACK     = 9;
	static const int KT_COUNT     = 10;
	

private:
	void loadConf();

	struct ltstr {
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	std::map<const char *, int, ltstr> cfm; // config file map

	union value {
		int i;
		GLdouble d;
		GLfloat f;
	};

	value data[CONSTANTS_COUNT];

};

#endif // CUBEGLCONF_H

