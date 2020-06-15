/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 1
 * \date    2018
 * \license Copyright unixODBC-GUI-Qt Project 2007-2018, See GPL.txt
 */
#include <odbcinst.h>
#include <odbcinstext.h>

#include <QApplication>
#include <QMessageBox>

/*! 
 * \brief   Invokes a UI to allow caller to manage/configure the ODBC environment.
 * 
 *          This wraps SQLManageDataSources to get a simple executable. We are a
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
 * \param   argc
 * \param   argv
 * 
 * \return  int
 * \retval  0   Success
 * \retval  1   Error
 */
int main( int argc, char **argv )
{
    // init Qt application...
    QApplication oApplication( argc, argv );
    QCoreApplication::setOrganizationName( QString::fromLocal8Bit( "unixODBC-GUI-Qt" ) );
    QCoreApplication::setOrganizationDomain( QString::fromLocal8Bit( "unixodbc-gui-qt.sourceforge.net" ) );
    QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Administrator" ) );

    // specify Qt5 UI plugin explicitly...
    ODBCINSTWND odbcinstwnd;
    strcpy( odbcinstwnd.szUI, "odbcinstQ5" );
    odbcinstwnd.hWnd = qApp->desktop();

    if ( SQLManageDataSources( (HWND)(&odbcinstwnd) ) )
        return 0;

    // oops - report any errors we can dig up...
    for ( WORD nError = 1; nError < 10; nError++ )
    {
        DWORD   nErrorCode;
        char    szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
        RETCODE nRetCode = SQLInstallerError( nError, &nErrorCode, szErrorMsg, SQL_MAX_MESSAGE_LENGTH, NULL );
        if ( !SQL_SUCCEEDED( nRetCode ) )
        {
            QMessageBox::critical( 0, QObject::tr( "ODBC Administrator" ),  QObject::tr( "Failed: no more errors to report.\n\nThe most likley causes are;\n- failed to find GUI plugin (libodbcinstQ5.so)\n- failed to load it due to unresolved references\n\nCommandLine tools that may help;\n file <path>libodbcinstQ5.so\n dltest <path>libodbcinstQ5.so ODBCManageDataSources\n ldd -r <path>libodbcinstQ5.so\n\nThe most common place to find libodbcinstQ5.so is in /usr/lib64." ) );
            break;
        }
        QMessageBox::critical( 0, QObject::tr( "ODBC Administrator" ),  QString::fromLocal8Bit( szErrorMsg ) );
    }

    // exit with error
    return 1;
}

