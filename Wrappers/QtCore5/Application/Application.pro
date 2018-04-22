TEMPLATE         = app 
TARGET           = OQConsole5
DESTDIR          = ../../../bin
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )

QT += core

CONFIG          += console

INCLUDEPATH     += ../include

LIBS += -L../../../lib -lOQ5

include( ../../../odbc.pri )

HEADERS		=
SOURCES 	= main.cpp


