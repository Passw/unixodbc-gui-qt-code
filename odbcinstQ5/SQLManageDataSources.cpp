/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#include <ODBCCommon.h>

#include "CODBCConfig.h"
#include "CDSNWizard.h"

/*! 
 * \brief   Manage Data Sources
 * 
 *          SQLManageDataSources done using Qt. Here is where we enter into Qt 'world'.
 * 
 * \param   hWnd
 * 
 * \return  BOOL
 */
static BOOL QT5ManageDataSources( HWND hWnd )
{
    /*
     * Here, we handle case where a non QT app has called us. Our approach here is a bit different 
     * than what we used for Qt3 because we were getting segfault upon app exit. Here; we instantiate
     * with a 'new' and NOT 'delete' it. Its a work around.
     */ 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ5", NULL };

        QApplication *pApplication = new QApplication( argc, argv );
        QCoreApplication::setOrganizationName( QString::fromLocal8Bit( "unixODBC" ) );
        QCoreApplication::setOrganizationDomain( QString::fromLocal8Bit( "unixodbc.org" ));
        QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Administrator" ) );
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // show dialog...
    CODBCConfig odbcconfig( pWidget );
    if ( odbcconfig.exec() == QDialog::Accepted )
        return true;

    return false;
}

/*! 
 * \brief   Create Data Source
 * 
 *          SQLCreateDataSource done using Qt. Here is where we enter into Qt 'world'.
 * 
 * \param   hWnd                QWidget *
 * \param   pszDataSourceName   UTF-16 encoded string
 * \return  BOOL
 */
static BOOL QT5CreateDataSource( HWND hWnd, LPCSTR pszDataSourceName )
{
    /*
     * Here, we handle case where a non QT app has called us. Our approach here is a bit different 
     * than what we used for Qt3 because we were getting segfault upon app exit. Here; we instantiate
     * with a 'new' and NOT 'delete' it. Its a work around.
     */ 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ5", NULL };

        QApplication *pApplication = new QApplication( argc, argv );
        QCoreApplication::setOrganizationName( QString::fromLocal8Bit( "unixODBC" ) );
        QCoreApplication::setOrganizationDomain( QString::fromLocal8Bit( "unixodbc.org" ) );
        QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Administrator" ) );
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // show dialog...
    CDSNWizardData  WizardData( ( pszDataSourceName ? QString::fromLocal8Bit( pszDataSourceName ) : QString::null ) );
    CDSNWizard      odbccreate( &WizardData, pWidget );
    int nReturn = odbccreate.exec();

    //  sucks to have to do this after wizard is on its way out - no fail and Back :(
    if ( nReturn == QDialog::Accepted )
        return CODBCInst::saveDataSourceName( pWidget, WizardData.hFirstProperty, WizardData.nType );

    return false;
}

/*! 
 * \brief   Create Data Source
 * 
 *          SQLCreateDataSource done using Qt. Here is where we enter into Qt 'world'.
 * 
 * \param   hWnd                QWidget *
 * \param   pszDataSourceName   UTF-16 encoded string
 * \return  BOOL
 */
static BOOL QT5CreateDataSourceW( HWND hWnd, LPCWSTR pszDataSourceName )
{
    /*
     * Here, we handle case where a non QT app has called us. Our approach here is a bit different 
     * than what we used for Qt3 because we were getting segfault upon app exit. Here; we instantiate
     * with a 'new' and NOT 'delete' it. Its a work around.
     */ 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ5", NULL };

        QApplication *pApplication = new QApplication( argc, argv );
        QCoreApplication::setOrganizationName( QString::fromLocal8Bit( "unixODBC" ) );
        QCoreApplication::setOrganizationDomain( QString::fromLocal8Bit( "unixodbc.org" ) );
        QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Administrator" ) );
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // show dialog...
    CDSNWizardData  WizardData( ( pszDataSourceName ? QString::fromUtf16( pszDataSourceName ) : QString::null ) );
    CDSNWizard      odbccreate( &WizardData, pWidget );
	int nReturn = odbccreate.exec();

    //  sucks to have to do this after wizard is on its way out - no fail and Back :(
    if ( nReturn == QDialog::Accepted )
        return CODBCInst::saveDataSourceName( pWidget, WizardData.hFirstProperty, WizardData.nType );

    return false;
}

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief   Plugin in entry point.
 *
 *          This entry point is called to implement the SQLManageDataSources call.
 * 
 *          This plugin does it using Qt5.
 * 
 *          The typical way to get to here is to call into odbcinst via SQLManageDataSource. 
 */
BOOL ODBCManageDataSources( HWND hWnd )
{
    return QT5ManageDataSources( hWnd );
}

/*!
 * \brief   Plugin in entry point.
 *
 *          This entry point is called to implement the SQLCreateDataSource call.
 * 
 *          This plugin does it using Qt.
 * 
 *          The typical way to get to here is to call into odbcinst via SQLCreateDataSource.
 * 
 *          GUI applications using unixODBC can call SQLCreateDataSource with a unique window
 *          handle which allows specifying a plugin to support the GUI that the
 *          application (and presumably the desktop) is using. odbcinstQ5 is the plugin.
 *          ODBCCreateDataSourceQ5 is an application which shows how this is done.
 * 
 *          This 'hook' is unique to unixODBC.
 * 
 * \sa      ODBCINSTWND
 * \sa      ODBCCreateDataSourceQ5
 * 
 * \param   hWnd                QWidget *
 * \param   pszDataSourceName   UTF-16 encoded string
 * \return  BOOL
 * 
 */
BOOL ODBCCreateDataSource( HWND hWnd, LPCSTR lpszDS )
{
    return QT5CreateDataSource( hWnd, lpszDS );
}

BOOL ODBCCreateDataSourceW( HWND hWnd, LPCWSTR lpszDS )
{
    return QT5CreateDataSourceW( hWnd, lpszDS );
}

#ifdef __cplusplus
};
#endif

