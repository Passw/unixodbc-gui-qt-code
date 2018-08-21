TEMPLATE         = app 
TARGET           = ODBCDataManagerQ5
DESTDIR          = ../bin
include( ../common.pri )
include( ../config.pri )
include( ../defines.pri )

#
# Lets switch Qt implicit char conversion back on as we are no longer
# directly interacting with ODBC (and all that WCHAR stuff) - we have
# our wrappers for that.
#
DEFINES -= QT_NO_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

QT += gui
QT += widgets

INCLUDEPATH     += ../Wrappers/QtCore5/include  ../Wrappers/QtGui5/include

LIBS += -L../lib -lOQG5 -lOQ5 -lodbcinstQ5

include( ../odbc.pri )

HEADERS		= \
#	DBrowser.h \
	DDataManager.h \
#	DDiagnostic.h \
#	DEdit.h \
#	DEditStatement.h \
#	DNode.h \
#	DNodeAttribute.h \
#	DNodeConnection.h \
#	DNodeDataSourceNameFile.h \
#	DNodeDataSourceNames.h \
#	DNodeDataSourceNameSystem.h \
#	DNodeDataSourceNameUser.h \
#	DNodeDriver.h \
#	DNodeDrivers.h \
#	DNodeOQ.h \
#	DNodeSchema.h \
#	DNodeSchemas.h \
#	DNodeStatement.h \
#	DNodeTable.h \
#	DNodeTableColumn.h \
#	DNodeTableColumns.h \
#	DNodeTables.h \
#	DNodeWidget.h \
	DOutputLog.h \
	DOutputMessages.h \
#	DPropWidget.h \
	ODBCDelegateCell.h \
	ODBCMetaInfo.h \
	ODBCModel.h \
	ODBCModelBrowser.h \
    ODBCModelConnection.h \
    ODBCModelDataSource.h \
    ODBCModelDataSourcesFile.h \
    ODBCModelDataSourcesSystem.h \
    ODBCModelDataSourcesUser.h \
	ODBCModelDriver.h \
	ODBCModelDrivers.h \
	ODBCModelEnvironment.h \
	ODBCModelSystem.h \
	ODBCValueCell.h \
	ODBCViewBrowser.h
SOURCES 	= \
	main.cpp \
#	DBrowser.cpp \
	DDataManager.cpp \
#	DDiagnostic.cpp \
#	DEdit.cpp \
#	DEditStatement.cpp \
#	DNode.cpp \
#	DNodeAttribute.cpp \
#	DNodeConnection.cpp \
#	DNodeDataSourceNameFile.cpp \
#	DNodeDataSourceNames.cpp \
#	DNodeDataSourceNameSystem.cpp \
#	DNodeDataSourceNameUser.cpp \
#	DNodeDriver.cpp \
#	DNodeDrivers.cpp \
#	DNodeODBC.cpp \
#	DNodeSchema.cpp \
#	DNodeSchemas.cpp \
#	DNodeStatement.cpp \
#	DNodeTable.cpp \
#	DNodeTableColumn.cpp \
#	DNodeTableColumns.cpp \
#	DNodeTables.cpp \
#	DNodeWidget.cpp \
	DOutputLog.cpp \
	DOutputMessages.cpp \
#	DPropWidget.cpp \
	ODBCDelegateCell.cpp \
	ODBCMetaInfo.cpp \
	ODBCModel.cpp \
	ODBCModelBrowser.cpp \
    ODBCModelConnection.cpp \
    ODBCModelDataSource.cpp \
    ODBCModelDataSourcesFile.cpp \
    ODBCModelDataSourcesSystem.cpp \
    ODBCModelDataSourcesUser.cpp \
	ODBCModelDriver.cpp \
	ODBCModelDrivers.cpp \
	ODBCModelEnvironment.cpp \
	ODBCModelSystem.cpp \
	ODBCValueCell.cpp \
	ODBCViewBrowser.cpp

#
# INSTALLS
#
target.path = $$INSTALL_TARGET_BIN
INSTALLS += target

