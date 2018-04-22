TEMPLATE         = app 
TARGET           = OQGConsole5
DESTDIR          = ../../../bin
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )

QT += gui
QT += widgets

INCLUDEPATH     += ../include  ../../QtCore5/include

LIBS += -L../../../lib -lOQG5  -lOQ5

include( ../../../odbc.pri )

HEADERS		= \
                OQGConsole.h
SOURCES 	= \
                main.cpp \
                OQGConsole.cpp


