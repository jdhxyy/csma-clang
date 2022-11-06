TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    ../../csma.c \
    ../../clib/tzrandom/tzrandom.c \
    ../../clib/scunit-clang/scunit.c \
    ../../clib/tztime/tztime.c \

INCLUDEPATH += ../../ \
    ../../clib/tzrandom \
    ../../clib/scunit-clang \
    ../../clib/tztime

HEADERS += \
    ../../csma.h \
    ../../clib/tzrandom/tzrandom.h \
    ../../clib/scunit-clang/scunit.h \
    ../../clib/tztime/tztime.h
