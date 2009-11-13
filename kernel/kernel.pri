# kernel include project file
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += formula.cpp \
    term.cpp \
    kernelexc.cpp \
    literalvalue.cpp \
    oldformula.cpp \
    termstree.cpp \
    minimizingalgorithm.cpp \
    quinemccluskey.cpp \
    kernel.cpp \
    termscontainer.cpp
HEADERS += formula.h \
    term.h \
    kernelexc.h \
    literalvalue.h \
    oldformula.h \
    termstree.h \
    minimizingalgorithm.h \
    quinemccluskey.h \
    kernel.h \
    constants.h \
    termscontainer.h
