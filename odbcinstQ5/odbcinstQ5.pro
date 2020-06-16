include(../common.pri)
include(../config.pri)
include(../defines.pri)
include(../odbc.pri)

TEMPLATE= lib
TARGET  = odbcinstQ5


# We do not want a version for a plugin file name (although gnu auto tools will use one but it at least sym links to a name without)...
# VERSION = %V_MAJOUR%.%V_MINOR%.%V_RELEASE%

DESTDIR	= ../lib
CONFIG	+= plugin

QT += gui
QT += widgets

# One way to get more access to unixODBC internals.
# But lets just define extern the few things we need. 
# DEFINES += UNIXODBC_SOURCE

#
# Turn the function mapping off.
# We can not include odbcinstext.h (internals) with function mapping on... it
# will cause an error (duplicate declaration of the functions). But, we need access to
# HODBCINSTPROPERTY... among other things. So we turn function mapping off and
# make wide-char calls explicitly.
#
DEFINES += SQL_NOUNICODEMAP

INCLUDEPATH	+= ../ini
LIBS		+= -L../lib -lini

HEADERS = \
	CAbout.h \
	CAdvanced.h \
	CDataSourceNameList.h \
	CDataSourceNames.h \
	CDataSourceNamesFile.h \
	CDataSourceNamesFileModel.h \
	CDriverConnectPrompt.h \
	CDriverList.h \
	CDriverPrompt.h \
	CDSNWizardProperties.h \
	CDSNWizardDriver.h \
	CDSNWizardEntre.h \
	CDSNWizardFini.h \
	CDSNWizard.h \
	CDSNWizardType.h \
	CFileSelector.h \
	CHelp.h \
	CManageDataSourceNames.h \
	CManageDrivers.h \
	CMonitor.h \
	CMonitorHandleCounts.h \
	CMonitorProcesses.h \
	CODBCConfig.h \
	CODBCInst.h \
	CPage.h \
	CPooling.h \
	CPropertiesDelegate.h \
	CPropertiesDialog.h \
	CPropertiesModel.h \
	CThreading.h \
	CTracing.h \
	About48.xpm \
	Advanced48.xpm \
	Canada.xpm \
	DataSourceName48.xpm \
	DataSourceNameFile48.xpm \
	DataSourceNameSystem48.xpm \
	DataSourceNameUser48.xpm \
	Driver128.xpm \
	Driver48.xpm \
	GreatBritain.xpm \
	Info16.xpm \
	InfoArrow16.xpm \
	NoFlag.xpm \
	ODBC.xpm \
	Pooling48.xpm \
	Set.xpm \
	Stats48.xpm \
	Threading48.xpm \
	Tracing48.xpm \
	ODBCManageDataSources64.xpm \
	WizardCreateDataSourceName.xpm

SOURCES = CAbout.cpp \
	CAdvanced.cpp \
	CDataSourceNameList.cpp \
	CDataSourceNames.cpp \
	CDataSourceNamesFile.cpp \
	CDataSourceNamesFileModel.cpp \
	CDriverConnectPrompt.cpp \
	CDriverList.cpp \
	CDriverPrompt.cpp \
	CDSNWizardProperties.cpp \
	CDSNWizardDriver.cpp \
	CDSNWizardEntre.cpp \
	CDSNWizardFini.cpp \
	CDSNWizard.cpp \
	CDSNWizardType.cpp \
	CFileSelector.cpp \
	CHelp.cpp \
	CManageDataSourceNames.cpp \
	CManageDrivers.cpp \
	CODBCConfig.cpp \
	CODBCInst.cpp \
	CPage.cpp \
	CPooling.cpp \
	CPropertiesDelegate.cpp \
	CPropertiesDialog.cpp \
	CPropertiesModel.cpp \
	CMonitorHandleCounts.cpp \
	CMonitor.cpp \
	CMonitorProcesses.cpp \
	CThreading.cpp \
	CTracing.cpp \
	ODBCDriverConnectPrompt.cpp \
	SQLManageDataSources.cpp

#
# INSTALLS
#
target.path = $$INSTALL_TARGET_LIB
INSTALLS += target

