#
# \brief	These bring in the core and dev stuff.
#
#
# \sa		*.pri *.pro
#

#
# Lets build for this version of the ODBC specification.
#
# DEFINES += ODBCVER=0x0300
# DEFINES += ODBCVER=0x0350
DEFINES += ODBCVER=0x0380

#
# Turn UNICODE on. 
# This will, among other things, will cause functions to be mapped to the wide-char version.
# For example; SQLCreateDataSource -> SQLCreateDataSourceW
#
DEFINES += UNICODE

# unixODBC-Test sources must be available so we can access 
# autotest.h. This is needed for, at least, ODBCTestQx.

INCLUDEPATH	+= $(UNIXODBC_TEST_SRC)/AutoTests/include

# #########################################################
# WIN32
# #########################################################
win32 {
	# We use MS's Driver Manager and Installer library.
        LIBS += odbc32.lib odbccp32.lib
}

# #########################################################
# MAC/UNIX common
# #########################################################
unix {
}

# #########################################################
# MAC/UNIX specific
# #########################################################
mac {
	# Use iodbc on Mac?
        LIBS	        += -liodbc -liodbcinst
} else:unix {
	# unixODBC-Core and unixODBC-Dev must be installed. Here we bring them into our build.
	# A simple binary install of these two will do fine.
	INCLUDEPATH	+= $(UNIXODBC_DIR)/include 
	LIBS		+= -L$(UNIXODBC_LIBDIR)
	LIBS		+= -lodbc -lodbcinst
}



