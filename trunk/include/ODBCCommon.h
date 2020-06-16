#ifndef H_ODBCCommon
#define H_ODBCCommon

// bring in ODBC stuff...
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <tchar.h>
    #include <sqlext.h>
    #include <odbcinst.h>
#else
    #include <unistd.h>
    #include <odbcinst.h> 
    #include <sqlext.h>
    // Access some unixODBC internals.
    // This is only possible because we define SQL_NOUNICODEMAP and call wide-char methods explicitly.
    #include <odbcinstext.h> 
#endif

// bring in Qt stuff...
#include <QtGui>
#include <QtWidgets>

#endif

