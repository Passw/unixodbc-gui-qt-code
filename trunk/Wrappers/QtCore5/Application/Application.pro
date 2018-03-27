TEMPLATE         = app 
TARGET           = OQConsole5
DESTDIR          = ../../../bin
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )
include( ../../../odbc.pri )
CONFIG          += console
QT += core
# $CONFIG          -= qt
INCLUDEPATH     += ../include

LIBS += -L../../../lib -lOQ5

HEADERS		=
SOURCES 	= main.cpp


