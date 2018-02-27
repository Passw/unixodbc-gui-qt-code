bin_PROGRAMS = ODBCManageDataSourcesQ5

INCLUDES = $(QT_CXXFLAGS) \
    -DSYSTEM_FILE_PATH=\"@sysconfdir@\" -DDEFLIB_PATH=\"@libdir@\" $(LTDLINCL)

ODBCManageDataSourcesQ5_LDADD =  \
	$(QT_LDFLAGS) \
	$(QT_LIBS) \
	-lodbc -lodbcinst

ODBCManageDataSourcesQ5_DEPENDANCIES = ../odbcinstQ5/libodbcinstQ5.la

ODBCManageDataSourcesQ5_SOURCES = \
	main.cpp

