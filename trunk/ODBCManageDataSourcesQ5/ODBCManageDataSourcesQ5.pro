include(../common.pri)
include(../config.pri)
include(../defines.pri)
include(../odbc.pri)

TEMPLATE= app
TARGET  = ODBCManageDataSourcesQ5
VERSION = %V_MAJOUR%.%V_MINOR%.%V_RELEASE%
DESTDIR	= ../bin

QT += gui
QT += widgets

SOURCES = main.cpp

#
# INSTALLS
#
target.path = $$INSTALL_TARGET_BIN
INSTALLS += target

