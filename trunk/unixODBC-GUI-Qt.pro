#
# building
# ========
#
# This can be built for UNIX'ism or MS'ism.
#
# UNIX'ism
# --------
#
# NOTE: The gnu auto-tools method is the preferred method 
# to build on UNIX. But, if you want, you can...
#
# Install unixODBC-Core and unixODBC-Dev. Set the following
# environment variables to tell us where unixODBC was
# installed;
#
# export UNIXODBC_DIR=/usr
# export UNIXODBC_LIBDIR=$UNIXODBC_DIR/lib64
#
# Get unixODBC-Test (do not build it yet) and set the
# following environment variable to tell us where it is;
#
# export UNIXODBC_TEST_SRC=~/unixodbc-test/trunk
#
# Build This
# 
# $ qmake
# $ make
# $ sudo make install
#
# MS'ism
# ------
#
# Get unixODBC-Test (do not build it yet) and set the
# following environment variable to tell us where it is;
#
# SET UNIXODBC_TEST_SRC=C:\\unixodbc-test\\AutoTests
#
# Build This
# 
# > qmake
# > nmake
#

TEMPLATE	= subdirs

# #########################################################
# WIN32
# #########################################################
win32 {
    equals( QT_MAJOR_VERSION, 4 ) {
       SUBDIRS	= \
           gtrtstQ4 \
           ODBCTestQ4
    }

    equals( QT_MAJOR_VERSION, 5 ) {
    	SUBDIRS	= \
    		gtrtstQ5 \
    		ODBCTestQ5
    }
}
# #########################################################
# MAC/UNIX shared
# #########################################################
unix {
    equals( QT_MAJOR_VERSION, 4 ) {
    	SUBDIRS	= \
    		ini \
    		odbcinstQ4 \
    		gtrtstQ4 \
    		Wrappers \
    		ODBCCreateDataSourceQ4 \
    		ODBCManageDataSourcesQ4 \
    		ODBCTestQ4
    }

    equals( QT_MAJOR_VERSION, 5 ) {
        SUBDIRS	= \
            ini \
            odbcinstQ5 \
#            gtrtstQ5 \
            ODBCCreateDataSourceQ5 \
            ODBCManageDataSourcesQ5 \
            Wrappers \
            ODBCDataManagerQ5 \
            ODBCTestQ5
    }
}

# #########################################################
# MAC/UNIX specific
# #########################################################
mac {

} else:unix {

}


