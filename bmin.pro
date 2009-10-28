# Bmin project

QT += opengl
TARGET = bmin
TEMPLATE = app

SOURCES += main.cpp

include(kernel/kernel.pri)
include(shell/shell.pri)
include(qtgui/qtgui.pri)
