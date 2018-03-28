/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 1
 * \date    2018
 * \license Copyright unixODBC-GUI-Qt Project 2009-2018, See GPL.txt
 */
#include <odbcinst.h>
#include <odbcinstext.h>

#include <QApplication>
#include <QMessageBox>

/*! 
 * \brief   Invokes a UI to walk caller through creating a new Data Source Name.
 * 
 *          This wraps SQLCreateDataSource to get a simple executable. We are a
 *          Qt5 based app so we explicitly request the Qt5 based UI ("odbcinstQ5"). 
 *
 *          This app can be complimented with other apps which use a different
 *          toolkit (ie GTK). Presumably those other apps will also use a file name
 *          for the executable which follows a similar convention as this..
 *
 *          \li ODBCCreateDataSource<Toolkit>
 *
 *          We can then designate a default, to assist desktop environment developers, 
 *          using a symlink in the file system.
 *
 * \todo    Accept a DSN on the command-line.
 *
 * \param   argc
 * \param   argv
 * 
 * \return  int
 * \retval  0   Success
 * \retval  1   Error
 */
int main( int argc, char **argv )
{
    // init a Qt application...
    QApplication oApplication( argc, argv );
    QCoreApplication::setOrganizationName( QString::fromLocal8Bit( "unixODBC-GUI-Qt" ) );
    QCoreApplication::setOrganizationDomain( QString::fromLocal8Bit( "unixodbc-gui-qt.sourceforge.net" ) );
    QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Create Data Source" ) );

    // specify Qt5 UI plugin explicitly...
    ODBCINSTWND odbcinstwnd;
    strcpy( odbcinstwnd.szUI, "odbcinstQ5" );
    odbcinstwnd.hWnd = (QWidget*)(qApp->desktop());

    // call odbcinst library (which will map to odbcinstQ5 UI plugin)...
    // We can pass NULL for 2nd arg but we pass empty string to work around a bug
    QString s( QString::fromLocal8Bit("") );
    if ( SQLCreateDataSourceW( (HWND)(&odbcinstwnd), (SQLWCHAR*)(s.utf16()) ) )
        return 0;

    // oops - report any errors we can dig up...
    for ( WORD nError = 1; nError < 10; nError++ )
    {
        DWORD   nErrorCode;
        char    szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
        RETCODE nRetCode = SQLInstallerError( nError, &nErrorCode, szErrorMsg, SQL_MAX_MESSAGE_LENGTH, NULL );
        if ( !SQL_SUCCEEDED( nRetCode ) )
        {
            QMessageBox::critical( 0, QObject::tr( "ODBC Create Data Source" ),  QObject::tr( "failed: no more errors to report" ) );
            break;
        }
        QMessageBox::critical( 0, QObject::tr( "ODBC Create Data Source" ),  QString::fromLocal8Bit( szErrorMsg ) );
    }

    // exit with error
    return 1;
}

