# shell include project file
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += konsole.h \
    lexicalanalyzer.h \
    parser.h \
    shellexc.h \
    options.h
SOURCES += konsole.cpp \
    lexicalanalyzer.cpp \
    parser.cpp \
    shellexc.cpp \
    options.cpp
