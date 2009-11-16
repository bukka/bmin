/*
 * cubegldrawer.h - OpenGL drawer of the Boolean n-Cube
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

#ifndef CUBEGLDRAWER_H
#define CUBEGLDRAWER_H

#include <QGLWidget>
#include <QColor>
#include <QTimer>
#include <QMap>
#include <Qt>

#include <vector>

#include "cubeglconf.h"

class QImage;
class QKeyEvent;
class QMouseEvent;
class QContextMenuEvent;
class QFocusEvent;
class QGLFormat;
class QMenu;
class QAction;
class QActionGroup;
class QString;
class Formula;
class Term;
class OutputValue;

// widget class for opengl cube
class CubeGLDrawer : public QGLWidget, public CubeGLConf
{
    Q_OBJECT

public:
    // maximal number of variables
    static const int MAX_N = 3;
    // maximal number of terms
    static const int MAX_T = 1 << MAX_N; // 2 ^ MAX_N

    CubeGLDrawer(const QGLFormat &format, QWidget *parent = 0);
    ~CubeGLDrawer();

signals:
    // emmited when key M is pushed - minimizing
    void minRequested();
    // emited when it is clicked on sphere
    void cubeChanged(int idx, OutputValue &value);
    // emmited when cube is focused
    void cubeFocused(bool cubeFocus);

public slots:
    // gets formula
    void setFormula(Formula *f);
    // when formula is minimized
    void minimizeCube();
    // when formula is invalid
    void invalidateCube();
    // setting activity
    void setActivity(bool active);

private slots:
    // slots for menu actions
    void rotateCube();
    void rotateLights();
    void animateMin();
    void setCamera3D();
    void setCameraRotate();
    void switchLight0();
    void switchLight1();
    void switchLight2();
    void setBgNone();
    void setBgPattern();
    void setBgComplete();
    void setAnimation();
    void keyClock();
    void keyTimerStop();

protected:
    // OpenGL methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // Event's methods
    void keyEvent(QKeyEvent *event, bool start);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);

private:
    static const int MIN4_POINTS = 8;
    static const int MIN2_POINTS = 4;
    static const int DT_COUNT    = 10;

    // direction types
    enum direction {DIRECT_X, DIRECT_Y, DIRECT_Z};
    // actual camera
    enum {CAM_ROTATE, CAM_3D} camera;
    // actual background
    enum {BG_NONE, BG_COMPLETE, BG_PATTERN} background;
    // paint message - when cube is not drawn
    enum {MSG_OVER, MSG_INVALID, MSG_NONE} paintedMsg;
    // key timers flags
    enum keyTimerFlag {KTS_FREE, KTS_ACTIVE, KTS_ENQ};


    // changes viewport
    void resizeWin(bool fullSetting = true);
    // makes all transformations
    void transformScene();
    // draws min box
    void drawMin();
    // draws cube - makes list
    void drawCube(int n, GLenum mode = GL_RENDER);
    // draws sphere
    void drawSphere(int idx, GLenum mode,
            GLdouble x, GLdouble y, GLdouble z);
    // makes sphere list
    void makeSpheres();
    // makes cylinder list
    void makeCylinder(int list);
    // makes dysplay list
    void makeDisplay();
    // makes msg list
    void makeMsg();
    // makes background list
    void makeBackground();
    // makes menu actions
    void makeActions();
    // mekes timers for movement
    void makeTimers();
    // called by movement in cube
    void go3D(direction d, GLdouble orient = 1.0);
    // switches lights positions
    void switchPosLights();
    // figures position of lights
    void figureLights();
    // makes text images and binds them to the texture
    GLuint bindTextTextures(QString text, int width, int height,
        int fontSize, const char *fontFamily,
        Qt::GlobalColor fontColor = Qt::black,
        Qt::GlobalColor bgColor = Qt::white);
    // binds msg to the the texture
    GLuint bindMsgTexture(QString text)
    {
        return bindTextTextures(text, getI(MSG_IMG_W), getI(MSG_IMG_H),
            getI(MSG_FONT_SIZE), "Monospace", Qt::red, Qt::black);
    }
    // binds terms to the textures
    void bindTermsTextures();
    // set animation points around 4 spheres
    void setMin4();
    // set animation points around 2 spheres
    void setMin2();
    // generates dynamic's texture images
    void genDT();

    void stopMin() { minTimer->stop(); isMin = false; }
    bool isInverted3D() { return hAngle3D > 90.0 && hAngle3D < 270.0; }

    // updates Cube
    void updateCube() { updateGL(); }

    // main formula
    Formula *formula;
    std::vector<Term> terms;
    std::vector<GLuint> termsTextures;

    // FLAGS
    // cube N
    int actualCube;
    // whether cube tab is showed
    bool isActive;
    // show minimization panel and animation
    bool isMin;
    // animation effect
    bool showAnimation;
    // actual visible term
    int minPos;
    // for dynamic effect in the begining of the minimization
    int dynamicPos;
    // switch on lights
    bool isLight[4];
    // windows width
    int winWidth;
    // windows height
    int winHeight;
    // for term texture translation - not actived
    int termTranslated;

    // Timers setting - for correct pausing in inactive mode
    struct 	{
        bool cube;
        bool min;
        bool lights;
    } timersActivity;

    struct KeyTimer {
        QTimer *main;
        QTimer *stop;
        keyTimerFlag flag;
    } keyTimers[KT_COUNT];


    GLfloat oneDiffuse[4];
    GLfloat zeroDiffuse[4];
    GLfloat dcDiffuse[4];
    GLfloat cylinderDiffuse[4];
    GLfloat light1Pos[4];
    GLfloat light2Pos[4];
    GLdouble min4Points[(MIN4_POINTS + 1) * 2];
    GLdouble min2Points[(MIN2_POINTS + 1) * 2];
    GLdouble min4Derivs[(MIN4_POINTS + 1) * 2];
    GLdouble min2Derivs[(MIN2_POINTS + 1) * 2];
    GLdouble wAngle;
    GLdouble hAngle;
    GLdouble zoom;
    GLfloat x3D;
    GLfloat y3D;
    GLfloat z3D;
    GLfloat wAngle3D;
    GLfloat hAngle3D;
    GLdouble overlap; // sphere - cylinder overlap
    GLdouble lightsAngle;
    GLdouble animateTime;

    GLuint cubeListId;
    GLuint sphereListId;
    GLuint minSphereListId;
    GLuint cylinderListId;
    GLuint displayListId;
    GLuint bgListId;
    GLuint msgListId;
    GLuint bgTexture[2];
    GLuint msgTexture[2];
    GLuint dynamicTexture[DT_COUNT];
    GLUquadricObj *quadric;

    QColor bgColor;
    QMenu *cameraMenu;
    QMenu *lightMenu;
    QMenu *bgMenu;
    QMenu *effectsMenu;
    QAction *camRotateAct;
    QAction *cam3DAct;
    QAction *lightAct[3];
    QAction *bgNoneAct;
    QAction *bgCompleteAct;
    QAction *bgPatternAct;
    QAction *animationAct;
    QActionGroup *camGroup;
    QActionGroup *bgGroup;
    QMap<int, KeyTimer *> keyMapRotate;
    QMap<int, KeyTimer *> keyMap3D;
    QTimer *cubeTimer;
    QTimer *lightsTimer;
    QTimer *minTimer;

};

#define IMG_BG1 ":/bits.png"
#define IMG_BG2 ":/bits_bg.png"

#endif // CUBEGLDRAWER_H

