TEMPLATE         = app 
TARGET           = OQGConsole5
DESTDIR          = ../../../bin
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )
include( ../../../odbc.pri )

INCLUDEPATH     += ../include  ../../QtCore5/include

LIBS += -L../../../lib -lOQG5  -lOQ

HEADERS		= \
                OQGConsole5.h
SOURCES 	= \
                main.cpp \
                OQGConsole5.cpp


