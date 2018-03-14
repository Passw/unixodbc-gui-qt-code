#
# \brief	These are some DEFINES for qmake.
#
#               Edit these settings, as needed, before using qmake to
#		generate the Makefiles. All sources are likely to use
#		the options reflected here.
#
# \sa		*.pri *.pro 
#

DEFINES += V_MAJOUR=1 V_MINOR=0 V_RELEASE=1

#
# Lets build for this version of the ODBC specification.
#
DEFINES += ODBCVER=0x0300

#
# Define for UNICODE, but do not map calls to A/W versions as we will call W versions
# explicitly. Also; lets not have Qt silently convert to/from ASCII for us... lets do
# that explicitly as well.
#
DEFINES += UNICODE \
	   SQL_NOUNICODEMAP \
           QT_NO_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

# #########################################################
# WIN32
# #########################################################
win32 {
        DEFINES += _CRT_SECURE_NO_WARNINGS
}

# #########################################################
# MAC/UNIX shared
# #########################################################
unix {
	INSTALL_TARGET_BIN	= /usr/bin
	INSTALL_TARGET_LIB	= /usr/lib64 
}

# #########################################################
# MAC/UNIX specific
# #########################################################
mac {

} else:unix {

}



