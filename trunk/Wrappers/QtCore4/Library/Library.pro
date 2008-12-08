TEMPLATE        = lib
VERSION         = 1.0.1
TARGET          = odbcq4 
DESTDIR         = ../../../lib
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )
include( ../../../odbc.pri )
CONFIG          += staticlib
CONFIG          += console

INCLUDEPATH     += ../include ../../CPP/include

HEADERS =       ../include/ODBCQConnection.h \
		../include/ODBCQConsole.h \
		../include/ODBCQEnvironment.h \
		../include/ODBCQStatement.h \
		../include/ODBCQSystem.h
SOURCES =       ODBCQConnection.cpp \
                ODBCQConsole.cpp \
		ODBCQEnvironment.cpp \
		ODBCQStatement.cpp \
		ODBCQSystem.cpp

