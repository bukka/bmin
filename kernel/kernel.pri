# kernel include project file
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += formula.cpp \
    term.cpp \
    exceptions.cpp \
    literalvalue.cpp \
    oldformula.cpp \
    termstree.cpp \
    minimizingalgorithm.cpp \
    quinemccluskey.cpp \
    kernel.cpp
HEADERS += formula.h \
    term.h \
    exceptions.h \
    literalvalue.h \
    oldformula.h \
    termstree.h \
    minimizingalgorithm.h \
    quinemccluskey.h \
    kernel.h
