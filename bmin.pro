# Bmin project

QT += opengl
TARGET = bmin
TEMPLATE = app


SOURCES += main.cpp
OTHER_FILES += license-header.txt license.txt

include(kernel/kernel.pri)
include(shell/shell.pri)
include(qtgui/qtgui.pri)
