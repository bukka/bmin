# Bmin project
QT += opengl
TARGET = bmin
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    guimanager.cpp \
    formula.cpp \
    term.cpp \
    controlwidget.cpp \
    cubewidget.cpp \
    mapwidget.cpp \
    mapcell.cpp \
    cubegldrawer.cpp \
    cubeglconf.cpp \
    welcomewidget.cpp \
    modewidget.cpp \
    modulewidget.cpp \
    modemanager.cpp \
    aboutdialog.cpp
HEADERS += mainwindow.h \
    guimanager.h \
    formula.h \
    term.h \
    controlwidget.h \
    cubewidget.h \
    mapwidget.h \
    mapcell.h \
    cubegldrawer.h \
    cubeglconf.h \
    welcomewidget.h \
    modewidget.h \
    constants.h \
    modulewidget.h \
    modemanager.h \
    aboutdialog.h
RESOURCES += resources.qrc
