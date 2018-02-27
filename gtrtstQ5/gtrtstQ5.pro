include(../common.pri)
include(../config.pri)
include(../defines.pri)
include(../odbc.pri)

QT += widgets

TEMPLATE= lib
TARGET  = gtrtstQ5
DESTDIR	= ../lib
CONFIG	+= staticlib

HEADERS = 

SOURCES = \
	szLogPrintf.cpp \
	szMessageBox.cpp

#
# INSTALLS
#
!win32 {
        target.path = $$INSTALL_TARGET_LIB
        INSTALLS += target
}

