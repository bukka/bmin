# Bmin project
lessThan(QT_VERSION, 4.5):error("Bmin requires Qt 4.5 or greater")
TEMPLATE = app
CONFIG += qt \
    warn_on
QT += opengl \
    webkit
TARGET = bmin
SOURCES += main.cpp
OTHER_FILES += license-header.txt \
    license.txt \
    doc/TODO.txt
unix { 
    RCC_DIR = $$PWD/.rcc
    UI_DIR = $$PWD/.ui
    MOC_DIR = $$PWD/.moc
    OBJECTS_DIR = $$PWD/.obj
    isEmpty(PREFIX):PREFIX = /usr
    isEmpty(BINDIR):BINDIR = $$PREFIX/bin
    target.path = $$BINDIR
    INSTALLS += target
}
include(kernel/kernel.pri)
include(shell/shell.pri)
include(qtgui/qtgui.pri)
