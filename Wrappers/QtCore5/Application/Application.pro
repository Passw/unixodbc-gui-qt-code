TEMPLATE         = app 
TARGET           = OQConsole5
DESTDIR          = ../../../bin
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )
include( ../../../odbc.pri )

QT += core

CONFIG          += console

INCLUDEPATH     += ../include

LIBS += -L../../../lib -lOQ5

HEADERS		=
SOURCES 	= main.cpp


