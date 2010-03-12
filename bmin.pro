# Bmin project

TEMPLATE = app
CONFIG += qt warn_on
#QT += opengl

TARGET = bmin
SOURCES += main.cpp
OTHER_FILES += license-header.txt license.txt doc/TODO.txt

include(kernel/kernel.pri)
include(shell/shell.pri)
include(qtgui/qtgui.pri)
