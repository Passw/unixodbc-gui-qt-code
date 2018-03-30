#
# \brief	These bring in the core and dev stuff.
#
#
# \sa		*.pri *.pro
#

#
# Lets build for this version of the ODBC specification.
#
DEFINES += ODBCVER=0x0300

#
# Define for UNICODE, but do not map calls to A/W versions as we will call W versions
# explicitly. 
#
DEFINES += UNICODE \
	   SQL_NOUNICODEMAP

# unixODBC-Test sources must be available so we can access 
# autotest.h. This is needed for, at least, ODBCTestQx.

INCLUDEPATH	+= $(UNIXODBC_TEST_SRC)/AutoTests/include

# #########################################################
# WIN32
# #########################################################
win32 {
#        INCLUDEPATH     += ../../include
#
#       We use MS's Driver Manager and Installer library.
#
        LIBS            += odbc32.lib odbccp32.lib
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
        LIBS	        += -liodbc -liodbcinst
} else:unix {
#
#       unixODBC-Core and unixODBC-Dev must be installed. Here we bring them into our build.
#       A simple binary install of these two will do fine.
#
	INCLUDEPATH	+= $(UNIXODBC_DIR)/include 
	LIBS		+= -L$(UNIXODBC_LIBDIR)
	LIBS		+= -lodbc -lodbcinst
}



