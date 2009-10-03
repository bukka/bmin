/*
 * cubegldrawer.cpp - OpenGL drawer of the Boolean n-Cube
 * created date: 2008/03/27
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
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QGLWidget>
#include <QGLFormat>
#include <QDebug>

#include <math.h>

#include "cubegldrawer.h"
#include "cubeglconf.h"
#include "formula.h"
#include "term.h"
#include "cubewidget.h"

static inline void setArray(GLfloat *a,
	GLfloat r, GLfloat g, GLfloat b, GLfloat w = 1.0f)
{
	a[0] = r;
	a[1] = g;
	a[2] = b;
	a[3] = w;
}

static inline void setParam(GLdouble **pa, GLdouble x, GLdouble y)
{
	(*pa)[0] = x;
	(*pa)[1] = y;
	*pa += 2;
}

void CubeGLDrawer::setMin2()
{
	GLdouble *p = min2Points;
	GLdouble *d = min2Derivs;
	GLdouble h = 2.0 * getD(SPHERE_R);
	GLdouble w = getD(CUBE_A) / 2.0;
	GLdouble v = getD(CUBE_A);

	setParam(&p, -w, h);
	setParam(&d, +v, 0);

	setParam(&p, +w, h);
	setParam(&d, +v, 0);

	setParam(&p, +w, -h);
	setParam(&d, -v, 0);

	setParam(&p, -w, -h);
	setParam(&d, -v, 0);

	setParam(&p, -w, h);
	setParam(&d, +v, 0);

}

void CubeGLDrawer::setMin4()
{
	GLdouble *p = min4Points;
	GLdouble *d = min4Derivs;
	GLdouble i = getD(CUBE_A) / 2.0 - 1.3 * getD(SPHERE_R);
	GLdouble o = getD(CUBE_A) / 2.0 + 2.0 * getD(SPHERE_R);
	GLdouble v1 = getD(CUBE_A) / 2.0;
	GLdouble v2 = getD(CUBE_A) / 1.5;

	setParam(&p, -i, o);
	setParam(&d, v1, 0);

	setParam(&p, i, o);
	setParam(&d, v2, 0);

	setParam(&p, o, i);
	setParam(&d, 0, -v1);

	setParam(&p, o, -i);
	setParam(&d, 0, -v2);

	setParam(&p, i, -o);
	setParam(&d, -v1, 0);

	setParam(&p, -i, -o);
	setParam(&d, -v2, 0);

	setParam(&p, -o, -i);
	setParam(&d, 0, v1);

	setParam(&p, -o, i);
	setParam(&d, 0, v2);

	setParam(&p, -i, o);
	setParam(&d, v1, 0);

}

CubeGLDrawer::CubeGLDrawer(const QGLFormat &format,
	QWidget *parent) : QGLWidget(format, parent), CubeGLConf()
{
	// color definition
	bgColor = Qt::black;
	// material definition
	setArray(oneDiffuse,      1.0f, 0.0f, 0.0f);
	setArray(zeroDiffuse,     0.0f, 1.0f, 1.0f);
	setArray(dcDiffuse,       1.0f, 0.0f, 1.0f);
	setArray(cylinderDiffuse, 0.6f, 0.6f, 0.6f);
	// set min points
	setMin2();
	setMin4();

	// light definitions
	lightsAngle = 30.0;
	figureLights();
	isLight[0] = false; // Reflector
	isLight[1] = true; // Light 1
	isLight[2] = true; // Light 2
	isLight[3] = isLight[1] || isLight[2];

	camera = CAM_3D; // default camera
	background = BG_NONE; // default background
	paintedMsg = MSG_INVALID; // default message
	actualCube = -1;
	animateTime = 0;
	isMin = false;
	showAnimation = true;
	termTranslated = 0;

	wAngle = 270.0;
	hAngle = 0.0;
	zoom = 1.0;
	// 3D camera
	x3D = 0.0;
	y3D = 0.0;
	z3D = -1.8;
	wAngle3D = 0.0;
	hAngle3D = 0.0;

	overlap = sqrt(getD(SPHERE_R) * getD(SPHERE_R) - getD(CYLINDER_R)
		* getD(CYLINDER_R))	- getD(SPHERE_R) / 20.0; // it is in sphere

	// setting default activity
	isActive = true;

	// essential for key and mouse events
	setFocusPolicy(Qt::StrongFocus);

	makeActions();
	makeTimers();
}

// destructor - deletes all lists
CubeGLDrawer::~CubeGLDrawer()
{
	makeCurrent();
	glDeleteLists(cubeListId, MAX_N + 1);
	glDeleteLists(sphereListId, 1);
	glDeleteLists(cylinderListId, 1);
	glDeleteLists(displayListId, 1);
	glDeleteLists(bgListId, 1);

}


void CubeGLDrawer::setFormula(Formula *f)
{
	stopMin();
	if (f->get_var_count() > MAX_N) {
		actualCube = -1;
		paintedMsg = MSG_OVER;
	}
	else {
		makeCurrent();
		formula = f;
		actualCube = f->get_var_count();
		drawCube(actualCube);
		paintedMsg = MSG_NONE;
	}
	updateCube();
}

void CubeGLDrawer::minimizeCube()
{
	if (!formula->is_minimized())
		return;
	makeCurrent();
	terms = formula->get_minterms();
	bindTermsTextures();

	if (isMin) {
		minPos = 0;
		dynamicPos = 0;
		minTimer->start(getI(TIMER_CLOCK));
		updateCube();
	}
}

void CubeGLDrawer::invalidateCube()
{
	actualCube = -1;
	paintedMsg = MSG_INVALID;
	updateCube();
}

void CubeGLDrawer::setActivity(bool active)
{
	if ((isActive = active)) {
		if (timersActivity.cube)
			cubeTimer->start(getI(TIMER_CLOCK));
		if (timersActivity.min)
			minTimer->start(getI(TIMER_CLOCK));
		if (timersActivity.lights)
			lightsTimer->start(getI(TIMER_CLOCK));

		setFocus();
	}
	else {
		timersActivity.cube = cubeTimer->isActive();
		timersActivity.min = minTimer->isActive();
		timersActivity.lights = lightsTimer->isActive();

		cubeTimer->stop();
		minTimer->stop();
		lightsTimer->stop();
	}
}


GLuint CubeGLDrawer::bindTextTextures(QString text,
	int width, int height, int fontSize, const char *fontFamily,
	Qt::GlobalColor fontColor, Qt::GlobalColor bgColor)
{
	QImage img(width, height, QImage::Format_ARGB32);
	QPainter painter(&img);
	painter.setFont(QFont(fontFamily, fontSize));
	painter.setPen(QColor(fontColor));
	painter.fillRect(0, 0,
		width, height, QBrush(bgColor));
	painter.drawText(0, 0, width, height, Qt::AlignCenter, text);
	return bindTexture(img);
}

void CubeGLDrawer::bindTermsTextures()
{
	GLuint texId;
	termsTextures.clear();

	for (unsigned i = 0; i < terms.size(); i++) {
		texId = bindTextTextures(QString::fromStdString(
			terms[i].to_string(formula->get_vars())), getI(TERM_IMG_W),
			getI(TERM_IMG_H), getI(TERM_FONT_SIZE), "Arial");
		termsTextures.push_back(texId);
	}
}

// generating dynamic texture
void CubeGLDrawer::genDT()
{
	int wStep = getI(TERM_IMG_W) / DT_COUNT + 1;
	int hStep = getI(TERM_IMG_H) / DT_COUNT + 1;
	int w = wStep;
	int h = hStep;
	for (int i = 0; i < DT_COUNT; i++) {
		QImage img(getI(TERM_IMG_W), getI(TERM_IMG_H),
			QImage::Format_ARGB32);
		QPainter painter(&img);
		painter.fillRect(0, 0,
			getI(TERM_IMG_W), getI(TERM_IMG_H), QBrush(Qt::black));
		painter.setBrush(QBrush(Qt::white));
		painter.setPen(Qt::NoPen);
		painter.drawEllipse((getI(TERM_IMG_W) - w) / 2,
			(getI(TERM_IMG_H) - h) / 2, w, h);

		dynamicTexture[i] = bindTexture(img);
		w += wStep;
		h += hStep;
	}
}

// cube rotating
void CubeGLDrawer::rotateCube()
{
	wAngle += getD(ROTATE_ANGLE) / 2.0;
	hAngle += getD(ROTATE_ANGLE) / 2.0;

	updateCube();
}

// figures out lights' positions
void CubeGLDrawer::figureLights()
{
	GLfloat lightsAngleRad = lightsAngle * M_PI / 180;
	GLfloat y = sin(lightsAngleRad);
	GLfloat z = cos(lightsAngleRad);
	setArray(light1Pos, 0.0f,  y,  z, 0.0f);
	setArray(light2Pos,	0.0f, -y, -z, 0.0f);

	GLfloat color = fabs(sin(lightsAngleRad) / 6.);
	GLfloat ambient1[4]= {color, 0.0, 0.0, 1.0};
	GLfloat ambient2[4]= {0.0, 0.0, color, 1.0};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
}

// lights rotating
void CubeGLDrawer::rotateLights()
{
	lightsAngle += getF(LIGHT_ANGLE);
	figureLights();
	updateCube();
}

// animates minimization
void CubeGLDrawer::animateMin()
{
	animateTime += getD(ANIMATE_STEP);
	if (animateTime > 1.0)
		animateTime -= 1.0;
	updateCube();
}

void CubeGLDrawer::initializeGL()
{
	// set flags
	glEnable(GL_CULL_FACE); // line aliasing
	glEnable(GL_DEPTH_TEST); // show depth - on z
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	// object style
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_MODELVIEW);

	// lights
	GLfloat ambient0[4]= {0.0,0.0,0.0,1.0};
	GLfloat diffuse0[4] = {1.0,1.0,1.0,1.0};
	GLfloat specular0[4] = {1.0,1.0,1.0,1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, 20);
	glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, 5);
	if (isLight[0])
		glEnable(GL_LIGHT0);

	GLfloat ambient1[4]= {0.0,0.0,0.0,1.0};
	GLfloat diffuse1[4] = {1.0,1.0,1.0,1.0};
	GLfloat specular1[4] = {1.0,1.0,1.0,1.0};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	if (isLight[1])
		glEnable(GL_LIGHT1);

	GLfloat ambient2[4]= {0.0,0.0,0.0,1.0};
	GLfloat diffuse2[4] = {1.0,1.0,1.0,1.0};
	GLfloat specular2[4] = {1.0,1.0,1.0,1.0};
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	if (isLight[2])
		glEnable(GL_LIGHT2);


	// generating lists
	cubeListId = glGenLists(MAX_N + 1);
	sphereListId = glGenLists(1);
	minSphereListId = glGenLists(1);
	cylinderListId = glGenLists(2);
	displayListId = glGenLists(1);
	bgListId = glGenLists(1);

	// set background color
	qglClearColor(bgColor);

	// generate texture
	bgTexture[0] = bindTexture(QImage(IMG_BG1));
	bgTexture[1] = bindTexture(QImage(IMG_BG2));
	genDT();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	makeSpheres();
	makeCylinder(0);
	makeDisplay();
	makeMsg();
	if (background != BG_NONE)
		makeBackground();

	for (int i = 0; i <= MAX_N; i++) {
		drawCube(i);
	}

	// default focus on cube - only for testing
	setFocus(Qt::MouseFocusReason);
}

// called when window is resized - new proportion by camera type
void CubeGLDrawer::resizeWin(bool fullSetting)
{
	if (fullSetting) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}

	if (camera == CAM_ROTATE) {
		GLdouble side = (GLdouble) qMin(winWidth, winHeight);
		GLdouble x = winWidth / side;
		GLdouble y = winHeight / side;
		glOrtho(-x, x, -y, y, -20.0, 20.0);
	}
	else
		gluPerspective(60.0, (GLdouble) winWidth / winHeight,
			0.01, 20.0);

	if (!fullSetting)
		return;

	glMatrixMode(GL_MODELVIEW);
}

void CubeGLDrawer::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	winWidth = width;
	winHeight = height;
	resizeWin();
}


void CubeGLDrawer::paintGL()
{
	if (!isActive)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (actualCube < 0) {
		if (paintedMsg != MSG_NONE) { // show message
			glLoadIdentity();
			glTranslatef(0.0, 0.0, -2.0);
			glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

			glCallList(msgListId + paintedMsg);
		}
	}
	else {
		glPushMatrix();
		transformScene();
		if (background != BG_NONE && camera == CAM_3D) {
			glCallList(bgListId);
		}
		glCallList(cubeListId + actualCube);
		if (isMin && minPos >= 0)
			drawMin();
		glPopMatrix();
	}
}

void CubeGLDrawer::transformScene()
{
	static GLdouble dtor = M_PI / 180.0;
	GLdouble wAngleRad, hAngleRad;

	if (camera == CAM_ROTATE) {
		wAngleRad  = wAngle * dtor;
		hAngleRad  = hAngle * dtor;

		GLdouble eyeX, eyeY, eyeZ, upX, upY, upZ;

		eyeX = cos(wAngleRad) * cos(hAngleRad);
		eyeY = sin(hAngleRad);
		eyeZ = -(sin(wAngleRad) * cos(hAngleRad));

		upX = 0.0;
		upY = cos(hAngleRad);
		upZ = 0.0;

		glLoadIdentity();
		// light 1
		glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
		glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);

		glScaled(zoom, zoom, zoom);
		gluLookAt(
			eyeX, eyeY, eyeZ,
			0.0, 0.0, 0.0,
			upX, upY, upZ
		);
	}
	else if (camera == CAM_3D) {
		glLoadIdentity();
		GLfloat light0Pos[4] = {0, 0, -0.0f, 1.0f};
		GLfloat light0Direct[3] = {0, 0, -1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0Direct);

		glRotatef(hAngle3D, 1.0f, 0.0f, 0.0f);
		glRotatef(wAngle3D, 0.0f, 1.0f, 0.0f);
		glTranslatef(x3D, y3D, z3D);

		// light 1
		glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
		glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);

		//	cout << "x3d: " << x3D << ", y3d: "
		//			<< y3D << ", z3d, " << z3D << endl;
	}
}

// Hermit curve: t-polynomials
static inline GLdouble hcF1(GLdouble t)
{
	return 2.0 * t * t * t - 3.0 * t * t + 1.0;
}
static inline GLdouble hcF2(GLdouble t)
{
	return -2.0 * t * t * t + 3.0 * t * t ;
}
static inline GLdouble hcF3(GLdouble t)
{
	return t * t * t - 2.0 * t * t + t;
}
static inline GLdouble hcF4(GLdouble t)
{
	return t * t * t - t * t;
}

void CubeGLDrawer::drawMin()
{
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (dynamicPos < DT_COUNT) {
		glBindTexture(GL_TEXTURE_2D, dynamicTexture[dynamicPos++]);
		glCallList(displayListId);
		return;
	}

	// number of ones and zeros
	int ozSize = terms[minPos].get_size(false);
	if (ozSize == 0)
		return;

	GLdouble x, y, t;
	/*
	if (termTranslated == 22) {
		termTranslated = 0;
	}
	else if (termTranslated > 0) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		if (termTranslated > 10) {
			glTranslated(-0.1 * (20 - termTranslated) , 0.0, 0.0);
			glMatrixMode(GL_MODELVIEW);
			termTranslated++;
		}
		else {
			glTranslated(0.1 * (GLdouble) termTranslated, 0.0, 0.0);
			termTranslated++;
			glBindTexture(GL_TEXTURE_2D, termsTextures[minPos-1]);
			glCallList(displayListId);
			glMatrixMode(GL_MODELVIEW);
			return;
		}
	}
	*/

	glBindTexture(GL_TEXTURE_2D, termsTextures[minPos]);
	glCallList(displayListId);

	if (!showAnimation)
		return;

	if (ozSize == actualCube) {
		t = animateTime * 4 * M_PI;
		x = 2 * getD(SPHERE_R) * cos(t);
		y = 2 * getD(SPHERE_R) * sin(t);
	}
	else {
		GLdouble *p, *d;
		int size, pos;

		if (ozSize == actualCube - 1) {
			p = min2Points;
			d = min2Derivs;
			size = MIN2_POINTS;
		}
		else {
			p = min4Points;
			d = min4Derivs;
			size = MIN4_POINTS;
		}

		t = animateTime * size;
		pos = (int) t;
		t -= pos;

		GLdouble *p1 = &p[2 * pos];
		GLdouble *p2 = &p[2 * pos + 2];
		GLdouble *d1 = &d[2 * pos];
		GLdouble *d2 = &d[2 * pos + 2];
		x =  p1[0] * hcF1(t) + p2[0] * hcF2(t)
			+ d1[0] * hcF3(t) + d2[0] * hcF4(t);
		y =  p1[1] * hcF1(t) + p2[1] * hcF2(t)
			+ d1[1] * hcF3(t) + d2[1] * hcF4(t);
	}
	GLdouble shift[3];
	GLdouble a = getD(CUBE_A) / 2.0;
	for (int i = 0; i < actualCube; i++) {
		switch (terms[minPos][i]) {
			case Term::one:
				shift[i] = a;
				break;
			case Term::zero:
				shift[i] = -a;
				break;
			default:
				shift[i] = 0;
		}
	}

	glPushMatrix();
	if (actualCube == 3) {
		if (ozSize == 2 && terms[minPos][2] != Term::dont_care) {
			if (terms[minPos][0] != Term::dont_care) {
				glRotated(90.0, 0.0, 0.0, 1.0);
				if (terms[minPos][2] == terms[minPos][0])
					glTranslated(x, y - shift[0], -shift[2]);
				else
					glTranslated(x, y + shift[0], shift[2]);

			}
			else {
				glRotated(90.0, 0.0, 1.0, 0.0);
				if (terms[minPos][2] == terms[minPos][1])
					glTranslated(x, y + shift[2], shift[1]);
				else
					glTranslated(x, y - shift[2], -shift[1]);
			}
		}
		else if (ozSize == 1 && terms[minPos][1] != Term::dont_care) {
			glRotated(90.0, 1.0, 0.0, 0.0);
			glTranslated(x, y, -shift[1]);
		}
		else if (ozSize == 1 && terms[minPos][2] != Term::dont_care) {
			glRotated(90.0, 0.0, 1.0, 0.0);
			glTranslated(x, y, shift[2]);
		}
		else
			glTranslated(x + shift[2], y + shift[1], -shift[0]);
	}
	else if (actualCube == 2) {
		if (ozSize == 1 && terms[minPos][1] != Term::dont_care) {
			glRotated(90, 0.0, 0.0, 1.0);
			glTranslated(x + shift[0], y - shift[1], 0.0);
		}
		else
			glTranslated(x + shift[1], y + shift[0], 0.0);
	}

	glCallList(minSphereListId);
	glPopMatrix();
}

void CubeGLDrawer::drawSphere(int idx, GLenum mode,
	GLdouble x, GLdouble y, GLdouble z)
{
	if (mode == GL_SELECT)
		glLoadName(idx);

	glTranslated(x, y, z);

	GLfloat *diffuse;
	if (actualCube < 0)
		diffuse = zeroDiffuse;
	else {
		tval value = formula->get_term_value(idx);
		switch (value) {
			case Term::zero:
				diffuse = zeroDiffuse;
				break;
			case Term::one:
				diffuse = oneDiffuse;
				break;
			case Term::dont_care:
				diffuse = dcDiffuse;
				break;
			default:
				diffuse = 0;
		}
	}
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glCallList(sphereListId);
}

void CubeGLDrawer::drawCube(int n, GLenum mode)
{
	GLdouble half =  getD(CUBE_A) / 2.0;
	GLdouble a = getD(CUBE_A);

	if (mode != GL_SELECT) {
		glNewList(cubeListId + n, GL_COMPILE);
	}
	switch (n) {
		case 0:
			glPushMatrix();
			drawSphere(0, mode, 0.0f, 0.0f, 0.0f);
			glPopMatrix();
			break;
		case 1:
			glPushMatrix();
			drawSphere(0, mode, -half, 0.0, 0.0);
			drawSphere(1, mode, a, 0.0, 0.0);

			if (mode == GL_SELECT)
				break;

			glMaterialfv(GL_FRONT, GL_DIFFUSE, cylinderDiffuse);
			glRotated(-90.0, 0.0, 1.0, 0.0);
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);
			glPopMatrix();
			break;
		case 2:
			glPushMatrix();
			drawSphere(2, mode, -half, half, 0.0);
			drawSphere(3, mode, a, 0.0, 0.0);
			drawSphere(1, mode, 0.0, -a, 0.0);
			drawSphere(0, mode, -a, 0.0, 0.0);

			if (mode == GL_SELECT)
				break;

			glMaterialfv(GL_FRONT, GL_DIFFUSE, cylinderDiffuse);
			glPushMatrix();
			glRotated(90.0, 0.0, 1.0, 0.0);
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);

			glTranslated(0.0, a, 0.0);
			glCallList(cylinderListId);
			glPopMatrix();

			glRotated(-90.0, 1.0, 0.0, 0.0);
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);

			glTranslated(a, 0.0, 0.0);
			glCallList(cylinderListId);

			glPopMatrix();

			break;

		case 3:
		case 4:
			glPushMatrix();

			drawSphere(2, mode,  -half, half, half);
			drawSphere(3, mode, a, 0.0, 0.0);
			drawSphere(1, mode, 0.0, -a, 0.0);
			drawSphere(0, mode, -a, 0.0, 0.0);
			drawSphere(4, mode, 0.0, 0.0, -a);
			drawSphere(5, mode, a, 0.0, 0.0);
			drawSphere(7, mode, 0.0, a, 0.0);
			drawSphere(6, mode, -a, 0.0, 0.0);

			if (mode == GL_SELECT) {
				glPopMatrix();
				break;
			}

			glMaterialfv(GL_FRONT, GL_DIFFUSE, cylinderDiffuse);

			// 1. cylinders
			glPushMatrix();
			glRotated(90.0, 0.0, 1.0, 0.0);
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);

			glTranslated(0.0, -a, 0.0);
			glCallList(cylinderListId);

			glTranslated(-a, 0.0, 0.0);
			glCallList(cylinderListId);

			glTranslated(0.0, a, 0.0);
			glCallList(cylinderListId);
			glPopMatrix();

			// 2. cylinders
			glPushMatrix();
			glRotated(90.0, 1.0, 0.0, 0.0);
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);

			glTranslated(0.0, a, 0.0);
			glCallList(cylinderListId);

			glTranslated(a, 0.0, 0.0);
			glCallList(cylinderListId);

			glTranslated(0.0, -a, 0.0);
			glCallList(cylinderListId);
			glPopMatrix();

			// 3. cylinders
			glTranslated(0.0, 0.0, overlap);
			glCallList(cylinderListId);

			glTranslated(0.0, -a, 0.0);
			glCallList(cylinderListId);

			glTranslated(a, 0.0, 0.0);
			glCallList(cylinderListId);

			glTranslated(0.0, a, 0.0);
			glCallList(cylinderListId);

			glPopMatrix();

			if (n == 3)
				break;

			// prepapared for 4-cube
			break;

	}
	if (mode != GL_SELECT)
		glEndList();
}

void CubeGLDrawer::makeSpheres()
{
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
//	gluQuadricTexture(quadric, GLU_TRUE);

	// vertex sphere
	glNewList(sphereListId, GL_COMPILE);
	glPushMatrix();
	gluSphere(quadric, getD(SPHERE_R), getI(SPHERE_STRIPS),
		getI(SPHERE_STACKS));
	glPopMatrix();
	glEndList();

	// animate's sphere
	glNewList(minSphereListId, GL_COMPILE);
	GLfloat diffuse[4] = {1.0, 1.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	gluSphere(quadric, getD(MIN_SPHERE_R), getI(MIN_SPHERE_STRIPS),
		getI(MIN_SPHERE_STACKS));
	glEndList();
}

void CubeGLDrawer::makeCylinder(int list)
{
	GLdouble a, x1, y1, x2, y2, z, normX, normY, step, length;

	a = 0.0;
	x1 = getD(CYLINDER_R);
	y1 = 0.0;
	step = M_PI / getI(CYLINDER_STRIPS);

	if (list == 0) {
		length = (getD(CUBE_A) - 2 * overlap) / getI(CYLINDER_STACKS);
	}
	else {
		length = 0.5; // for 4-Cube - not implemnted
	}

	glNewList(cylinderListId + list, GL_COMPILE);
	for (int i = 0; i <  getI(CYLINDER_STRIPS); i++) {
		z = 0.0;
		a += step;
		normX = cos(a);
		normY = sin(a);
		glNormal3d(normX, normY, z);
		a += step;

		x2 = getD(CYLINDER_R) * cos(a);
		y2 = getD(CYLINDER_R) * sin(a);


		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= getI(CYLINDER_STACKS); j++, z += length) {
			glVertex3d(x1, y1, z);
			glVertex3d(x2, y2, z);
		}
		glEnd();

		x1 = x2;
		y1 = y2;

	}
	glEndList();
}

void CubeGLDrawer::makeDisplay()
{
	GLdouble a = getD(DISPLAY_W) / 2.0;
	GLdouble b = getD(DISPLAY_H) / 2.0;
	GLdouble c = getD(DISPLAY_D) / 2.0;

	glNewList(displayListId, GL_COMPILE);

	GLfloat diffuse[4] = {0.8, 0.8, 0.8};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

	// front
	glEnable(GL_TEXTURE_2D);
	glNormal3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(-a, -b, c);
	glTexCoord2d(1.0, 0.0); glVertex3d(+a, -b, c);
	glTexCoord2d(1.0, 1.0); glVertex3d(+a, +b, c);
	glTexCoord2d(0.0, 1.0); glVertex3d(-a, +b, c);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// back
	glNormal3d(0.0, 0.0, -1.0);
	glBegin(GL_QUADS);
	glVertex3d(-a, -b, -c);
	glVertex3d(-a, +b, -c);
	glVertex3d(+a, +b, -c);
	glVertex3d(+a, -b, -c);
	glEnd();

	// top
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3d(-a, +b, -c);
	glVertex3d(-a, +b, +c);
	glVertex3d(+a, +b, +c);
	glVertex3d(+a, +b, -c);
	glEnd();

	// bottom
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3d(-a, -b, +c);
	glVertex3d(-a, -b, -c);
	glVertex3d(+a, -b, -c);
	glVertex3d(+a, -b, +c);
	glEnd();

	// right
	glNormal3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3d(+a, -b, +c);
	glVertex3d(+a, -b, -c);
	glVertex3d(+a, +b, -c);
	glVertex3d(+a, +b, +c);
	glEnd();

	// left
	glNormal3d(0.0, 0.0, -1.0);
	glBegin(GL_QUADS);
	glVertex3d(-a, -b, -c);
	glVertex3d(-a, -b, +c);
	glVertex3d(-a, +b, +c);
	glVertex3d(-a, +b, -c);
	glEnd();

	glEndList();
}

void CubeGLDrawer::makeMsg()
{
	msgListId = glGenLists(2);

	// MSG_OVER = 0
	msgTexture[MSG_OVER] = bindMsgTexture(QString(tr("Maximal "))
		+ QString::number(MAX_N) + QString(tr("-Cube")));
	// MSG_INVALID = 1
	msgTexture[MSG_INVALID] = bindMsgTexture(tr("Unknown formula"));


	GLdouble x = 0.75;
	GLdouble y = 0.1;

	for (int i = 0; i < 2; i++) {
		glNewList(msgListId + i, GL_COMPILE);
		glEnable(GL_TEXTURE_2D);
//		glDisable(GL_LIGHTING);

		glTexParameterf(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, msgTexture[i]);

		GLfloat diffuse[4] = {0.8, 0.8, 0.8};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

		glNormal3d(0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0); glVertex3d(-x, -y, 0.0);
		glTexCoord2d(1.0, 0.0); glVertex3d(+x, -y, 0.0);
		glTexCoord2d(1.0, 1.0); glVertex3d(+x, +y, 0.0);
		glTexCoord2d(0.0, 1.0); glVertex3d(-x, +y, 0.0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
//		glEnable(GL_LIGHTING);

		glEndList();
	}
}

void CubeGLDrawer::makeBackground()
{
	glNewList(bgListId, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D,
		bgTexture[(background == BG_COMPLETE)? 0: 1]);

	double rp1 = 10.0;
	double rp2 = 2 * rp1;
	// front side
	glNormal3d(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0, -2.0);
	glTexCoord2d(rp1, 0.0); glVertex3d(+1.0, -1.0, -2.0);
	glTexCoord2d(rp1, rp1); glVertex3d(+1.0, +1.0, -2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(-1.0, +1.0, -2.0);
	glEnd();

	// right side
	glNormal3d(-1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(+1.0, -1.0, -2.0);
	glTexCoord2d(rp2, 0.0); glVertex3d(+1.0, -1.0, +2.0);
	glTexCoord2d(rp2, rp1); glVertex3d(+1.0, +1.0, +2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(+1.0, +1.0, -2.0);
	glEnd();

	// back side
	glNormal3d(0.0, 0.0, -1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(+1.0, -1.0, +2.0);
	glTexCoord2d(rp1, 0.0); glVertex3d(-1.0, -1.0, +2.0);
	glTexCoord2d(rp1, rp1); glVertex3d(-1.0, +1.0, +2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(+1.0, +1.0, +2.0);
	glEnd();

	// left side
	glNormal3d(+1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0, +2.0);
	glTexCoord2d(rp2, 0.0); glVertex3d(-1.0, -1.0, -2.0);
	glTexCoord2d(rp2, rp1); glVertex3d(-1.0, +1.0, -2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(-1.0, +1.0, +2.0);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, bgTexture[1]);

	// top side
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, +1.0, +2.0);
	glTexCoord2d(rp2, 0.0); glVertex3d(-1.0, +1.0, -2.0);
	glTexCoord2d(rp2, rp1); glVertex3d(+1.0, +1.0, -2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(+1.0, +1.0, +2.0);
	glEnd();

	// bottom side
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3d(+1.0, -1.0, +2.0);
	glTexCoord2d(rp2, 0.0); glVertex3d(+1.0, -1.0, -2.0);
	glTexCoord2d(rp2, rp1); glVertex3d(-1.0, -1.0, -2.0);
	glTexCoord2d(0.0, rp1); glVertex3d(-1.0, -1.0, +2.0);
	glEnd();

	// setting for other textures
	glDisable(GL_TEXTURE_2D);

	glEndList();
}

void CubeGLDrawer::makeTimers()
{
	// dynamic timer - rotates with cube, lights
	// and animates minimization
	cubeTimer = new QTimer(this);
	connect(cubeTimer, SIGNAL(timeout()), this, SLOT(rotateCube()));
	lightsTimer = new QTimer(this);
	connect(lightsTimer, SIGNAL(timeout()), this, SLOT(rotateLights()));
	minTimer = new QTimer(this);
	connect(minTimer, SIGNAL(timeout()), this, SLOT(animateMin()));

	// setting default timers' values
	timersActivity.cube = timersActivity.min = timersActivity.lights = false;

	// timers for key's actions
	for (int i = 0; i < KT_COUNT; i++) {
		keyTimers[i].main = new QTimer(this);
		keyTimers[i].main->setInterval(getI(KT_CLOCK));
		keyTimers[i].main->setProperty("timer", i);
		keyTimers[i].stop = new QTimer(this);
		keyTimers[i].stop->setSingleShot(true);
		keyTimers[i].stop->setInterval(getI(KT_STOP_AFTER));
		keyTimers[i].stop->setProperty("timer", i);
		keyTimers[i].flag = KTS_FREE;
		connect(keyTimers[i].main, SIGNAL(timeout()), SLOT(keyClock()));
		connect(keyTimers[i].stop, SIGNAL(timeout()), SLOT(keyTimerStop()));
	}
	
	// maping timers to keys in rotate camera mode
	keyMapRotate[Qt::Key_Left]  = &keyTimers[KTR_LEFT];
	keyMapRotate[Qt::Key_Right] = &keyTimers[KTR_RIGHT];
	keyMapRotate[Qt::Key_Up]    = &keyTimers[KTR_UP];
	keyMapRotate[Qt::Key_Down]  = &keyTimers[KTR_DOWN];
	keyMapRotate[Qt::Key_Plus]  = &keyTimers[KTM_UP];
	keyMapRotate[Qt::Key_Minus] = &keyTimers[KTM_DOWN];

	// maping timers to keys in 3D camera mode
	// rotation
	keyMap3D[Qt::Key_Left]  = keyMap3D[Qt::Key_A] = &keyTimers[KTR_LEFT];
	keyMap3D[Qt::Key_Right] = keyMap3D[Qt::Key_D] = &keyTimers[KTR_RIGHT];
	keyMap3D[Qt::Key_E] = &keyTimers[KTR_UP];
	keyMap3D[Qt::Key_C] = &keyTimers[KTR_DOWN];
	// movement
	keyMap3D[Qt::Key_Up] = keyMap3D[Qt::Key_W] = &keyTimers[KTM_STRAIGHT];
	keyMap3D[Qt::Key_Down] = keyMap3D[Qt::Key_S] = &keyTimers[KTM_BACK];
	keyMap3D[Qt::Key_H] = &keyTimers[KTM_RIGHT];
	keyMap3D[Qt::Key_F] = &keyTimers[KTM_LEFT];
	keyMap3D[Qt::Key_G] = &keyTimers[KTM_DOWN];
	keyMap3D[Qt::Key_T] = &keyTimers[KTM_UP];
}

void CubeGLDrawer::go3D(direction d, GLdouble orient)
{
	GLfloat stepX, stepY, stepZ;
	GLfloat dtor = M_PI / 180.0;
	GLfloat wAngleRad  = wAngle3D * dtor;
	GLfloat hAngleRad  = hAngle3D * dtor;

	switch (d) {
		case DIRECT_X:
			stepX = orient * getD(WALK_STEP) * -cos(wAngleRad);
			stepY = 0.0;
			stepZ = orient * getD(WALK_STEP) * sin(-wAngleRad);
			break;
		case DIRECT_Y:
			stepX = 0.0; //orient * getD(WALK_STEP) * -sin(wAngleRad);
			stepY = orient * getD(WALK_STEP) * -cos(hAngleRad);
			stepZ = orient * getD(WALK_STEP) * sin(hAngleRad);
						// * cos(wAngleRad);
			break;
		case DIRECT_Z:
			stepX = orient * getD(WALK_STEP) *
				sin((isInverted3D()? 1.0: -1.0) * wAngleRad);
			stepY = orient * getD(WALK_STEP) * sin(hAngleRad);
			stepZ = orient * getD(WALK_STEP) * cos(wAngleRad) *
				cos(hAngleRad);
			break;
		default:
			stepX = stepY = stepZ = 0;
	}

	if (x3D + stepX < 1.0 && x3D + stepX > -1.0)
		x3D += stepX;
	if (y3D + stepY < 1.0 && y3D + stepY > -1.0)
		y3D += stepY;
	if (z3D + stepZ < 2.0 && z3D + stepZ > -2.0)
		z3D += stepZ;
}

void CubeGLDrawer::keyClock()
{
	int timerId = sender()->property("timer").toInt();
	if (camera == CAM_ROTATE) {
		switch (timerId) {
			case KTR_LEFT:
				if (!cubeTimer->isActive())
					wAngle -= getD(ROTATE_ANGLE);
				break;
			case KTR_RIGHT:
				wAngle += getD(ROTATE_ANGLE);
				break;
			case KTR_DOWN:
				if (!cubeTimer->isActive())
					hAngle -= getD(ROTATE_ANGLE);
				break;
			case KTR_UP:
				hAngle += getD(ROTATE_ANGLE);
				break;
			case KTM_UP:
				if (zoom < getD(ZOOM_MAX))
					zoom += getD(ZOOM_STEP);
				break;
			case KTM_DOWN:
				if (zoom > getD(ZOOM_MIN))
					zoom -= getD(ZOOM_STEP);
				break;
		}
	}
	else { // 3D
		switch (timerId) {
			// rotate move
			case KTR_LEFT:
				if (isInverted3D())
					wAngle3D += getD(TURN_ANGLE);
				else
					wAngle3D -= getD(TURN_ANGLE);
				break;
			case KTR_RIGHT:
				if (isInverted3D())
					wAngle3D -= getD(TURN_ANGLE);
				else
					wAngle3D += getD(TURN_ANGLE);
				break;
			case KTR_UP:
				hAngle3D -= getD(TURN_ANGLE);
				if (hAngle3D < 0)
					hAngle3D += 360;
				break;
			case KTR_DOWN:
				hAngle3D += getD(TURN_ANGLE);
				if (hAngle3D > 0)
					hAngle3D -= 360;
				break;

			// transform move
			case KTM_STRAIGHT:
				go3D(DIRECT_Z);
				break;
			case KTM_BACK:
				go3D(DIRECT_Z, -1.0);
				break;
			case KTM_RIGHT:
				go3D(DIRECT_X);
				break;
			case KTM_LEFT:
				go3D(DIRECT_X, -1.0);
				break;
			case KTM_UP:
				go3D(DIRECT_Y);
				break;
			case KTM_DOWN:
				go3D(DIRECT_Y, -1.0);
				break;
		}
	}
	updateCube();
}

void CubeGLDrawer::keyTimerStop()
{
	KeyTimer *kt = &keyTimers[sender()->property("timer").toInt()];
	if (kt->flag == KTS_ENQ) {
		kt->flag = KTS_ACTIVE;
		kt->stop->start();
	}
	else {
		kt->flag = KTS_FREE;
		kt->main->stop();
	}
}

void CubeGLDrawer::keyEvent(QKeyEvent *event, bool start)
{
	KeyTimer *kt;
	if (camera == CAM_ROTATE && keyMapRotate.contains(event->key()))
		kt = keyMapRotate[event->key()];
	else if (camera == CAM_3D && keyMap3D.contains(event->key()))
		kt = keyMap3D[event->key()];
	else
		return;

	if (start && !kt->main->isActive())
		kt->main->start();
	else if (!start) {
		if (kt->flag == KTS_FREE)
			kt->stop->start();
		else if (kt->flag == KTS_ACTIVE)
			kt->flag = KTS_ENQ;
	}
}

void CubeGLDrawer::keyPressEvent(QKeyEvent *event)
{
	if (actualCube < 0)
		return;

	makeCurrent();
	switch (event->key()) {
		case Qt::Key_1:
			setCamera3D();
			return;
		case Qt::Key_2:
			setCameraRotate();
			return;
		case Qt::Key_L:
			switchPosLights();
			return;
		case Qt::Key_I:
			lightsTimer->isActive()? lightsTimer->stop():
				lightsTimer->start(getI(TIMER_CLOCK));
			return;
		case Qt::Key_M:
			if (isMin) {
				minTimer->stop();
				isMin = false;
			}
			else {
				isMin = true;
				if (formula->is_minimized()) {
					minTimer->start(getI(TIMER_CLOCK));
					minPos = 0;
					dynamicPos = 0;
				}
				else {
					minPos = -1;
					emit minRequested();
					return;
				}
			}
			break;
		case Qt::Key_N:
			if (isMin && (int) termsTextures.size() > (minPos + 1)) {
				termTranslated = 1;
				minPos++;
				break;
			}
			else
				return;
		case Qt::Key_B:
			if (isMin && minPos > 0)
				minPos--;
			break;
		case Qt::Key_R:
			if (camera == CAM_ROTATE) {
				if (cubeTimer->isActive())
					cubeTimer->stop();
				else
					cubeTimer->start(getI(TIMER_CLOCK));
				return;
			}
			else
				switchLight0();
			break;
		default:
			keyEvent(event, true);
			break;
	}

	updateCube();
}

void CubeGLDrawer::keyReleaseEvent(QKeyEvent *event)
{
	keyEvent(event, false);
}

void CubeGLDrawer::setCamera3D()
{
	camera = CAM_3D;
	cubeTimer->stop();
	if (isLight[0])
		glEnable(GL_LIGHT0);
	resizeWin();
	updateCube();
}

void CubeGLDrawer::setCameraRotate()
{
	camera = CAM_ROTATE;
	glDisable(GL_LIGHT0);
	resizeWin();
	updateCube();
}

void CubeGLDrawer::switchLight0()
{
	if (camera == CAM_3D)
		isLight[0]? glDisable(GL_LIGHT0): glEnable(GL_LIGHT0);

	isLight[0] = !isLight[0];
	updateCube();
}

void CubeGLDrawer::switchLight1()
{
	isLight[1]? glDisable(GL_LIGHT1): glEnable(GL_LIGHT1);
	isLight[1] = !isLight[1];
	updateCube();
}

void CubeGLDrawer::switchLight2()
{
	isLight[2]? glDisable(GL_LIGHT2): glEnable(GL_LIGHT2);
	isLight[2] = !isLight[2];
	updateCube();
}

void CubeGLDrawer::switchPosLights()
{
	isLight[3] = !(isLight[3] && (isLight[1] || isLight[2]));
	if (isLight[3]) {
		if (isLight[1])
			glEnable(GL_LIGHT1);
		if (isLight[2])
			glEnable(GL_LIGHT2);
	}
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	updateCube();
}

void CubeGLDrawer::setBgNone()
{
	background = BG_NONE;
	updateCube();
}

void CubeGLDrawer::setBgPattern()
{
	background = BG_PATTERN;
	makeBackground();
	updateCube();
}

void CubeGLDrawer::setBgComplete()
{
	background = BG_COMPLETE;
	makeBackground();
	updateCube();
}

void CubeGLDrawer::setAnimation()
{
	showAnimation = !showAnimation;
}

void CubeGLDrawer::makeActions()
{
	cam3DAct = new QAction(tr("&3D"), this);
	cam3DAct->setCheckable(true);
	connect(cam3DAct, SIGNAL(triggered()), this, SLOT(setCamera3D()));

	camRotateAct = new QAction(tr("&Rotated"), this);
	camRotateAct->setCheckable(true);
	connect(camRotateAct, SIGNAL(triggered()),
		this, SLOT(setCameraRotate()));

	camGroup = new QActionGroup(this);
	camGroup->addAction(camRotateAct);
	camGroup->addAction(cam3DAct);

	lightAct[0] = new QAction(tr("&Reflector"), this);
	lightAct[0]->setCheckable(true);
	connect(lightAct[0], SIGNAL(triggered()),
		this, SLOT(switchLight0()));

	lightAct[1] = new QAction(tr("Light &1"), this);
	lightAct[1]->setCheckable(true);
	connect(lightAct[1], SIGNAL(triggered()),
		this, SLOT(switchLight1()));

	lightAct[2] = new QAction(tr("Light &2"), this);
	lightAct[2]->setCheckable(true);
	connect(lightAct[2], SIGNAL(triggered()),
		this, SLOT(switchLight2()));

	bgNoneAct  = new QAction(tr("&None"), this);
	bgNoneAct->setCheckable(true);
	connect(bgNoneAct, SIGNAL(triggered()),
		this, SLOT(setBgNone()));

	bgPatternAct  = new QAction(tr("&Pattern"), this);
	bgPatternAct->setCheckable(true);
	connect(bgPatternAct, SIGNAL(triggered()),
		this, SLOT(setBgPattern()));

	bgCompleteAct  = new QAction(tr("&Complete"), this);
	bgCompleteAct->setCheckable(true);
	connect(bgCompleteAct, SIGNAL(triggered()),
		this, SLOT(setBgComplete()));

	bgGroup = new QActionGroup(this);
	bgGroup->addAction(bgNoneAct);
	bgGroup->addAction(bgPatternAct);
	bgGroup->addAction(bgCompleteAct);

	animationAct  = new QAction(tr("&Animation"), this);
	animationAct->setCheckable(true);
	connect(animationAct, SIGNAL(triggered()),
		this, SLOT(setAnimation()));
}

void CubeGLDrawer::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);

	cam3DAct->setChecked(camera == CAM_3D);
	camRotateAct->setChecked(camera == CAM_ROTATE);
	cameraMenu = menu.addMenu(tr("&Camera"));
	cameraMenu->addAction(cam3DAct);
	cameraMenu->addAction(camRotateAct);

	lightMenu = menu.addMenu(tr("&Lights"));
	for (int i = 0; i < 3; i++) {
		lightAct[i]->setChecked(isLight[i]);
		lightMenu->addAction(lightAct[i]);
		if (i == 0)
			lightMenu->addSeparator();
	}

	if (camera == CAM_3D) {
		bgNoneAct->setChecked(background == BG_NONE);
		bgPatternAct->setChecked(background == BG_PATTERN);
		bgCompleteAct->setChecked(background == BG_COMPLETE);

		bgMenu = menu.addMenu(tr("&Background"));
		bgMenu->addAction(bgNoneAct);
		bgMenu->addAction(bgPatternAct);
		bgMenu->addAction(bgCompleteAct);
	}

	animationAct->setChecked(showAnimation);
	effectsMenu = menu.addMenu(tr("&Effects"));
	effectsMenu->addAction(animationAct);

	menu.exec(event->globalPos());
}


void CubeGLDrawer::mousePressEvent(QMouseEvent *event)
{
	makeCurrent();

	GLuint sBuff[MAX_T];
	GLint hits, viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(MAX_T, sBuff);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(MAX_T);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble) event->x(), (GLdouble)
		(viewport[3] - event->y()), getD(PICKBOX_A),
			getD(PICKBOX_A), viewport);
	resizeWin(false);

	glMatrixMode(GL_MODELVIEW);
	transformScene();
	drawCube(actualCube, GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	hits = glRenderMode(GL_RENDER);
	if (hits)
		emit cubeChanged(sBuff[3],
			Term::get_next_value(formula->get_term_value(sBuff[3])));

}

void CubeGLDrawer::focusInEvent(QFocusEvent * /* event */)
{
	emit cubeFocused(true);
}

void CubeGLDrawer::focusOutEvent(QFocusEvent * /* event */)
{
	emit cubeFocused(false);
}

