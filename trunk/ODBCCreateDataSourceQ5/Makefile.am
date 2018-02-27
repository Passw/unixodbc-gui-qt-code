bin_PROGRAMS = ODBCCreateDataSourceQ5

INCLUDES = $(QT_CXXFLAGS) \
    -DSYSTEM_FILE_PATH=\"@sysconfdir@\" -DDEFLIB_PATH=\"@libdir@\" $(LTDLINCL)

ODBCCreateDataSourceQ5_LDADD =  \
	$(QT_LDFLAGS) \
	$(QT_LIBS) \
	-lodbc -lodbcinst

ODBCCreateDataSourceQ5_DEPENDANCIES = ../odbcinstQ5/libodbcinstQ5.la

ODBCCreateDataSourceQ5_SOURCES = \
	main.cpp

