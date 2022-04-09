TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    ../../csma.c \
    ../../clib/tzrandom/tzrandom.c \
    ../../clib/scunit-clang/scunit.c \

INCLUDEPATH += ../../ \
    ../../clib/tzrandom \
    ../../clib/scunit-clang

HEADERS += \
    ../../csma.h \
    ../../clib/tzrandom/tzrandom.h \
    ../../clib/scunit-clang/scunit.h
