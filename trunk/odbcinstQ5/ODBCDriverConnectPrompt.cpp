/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 1
 * \date    2008
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#include <odbcinstext.h>

#include <QApplication>

#include "CDriverConnectPrompt.h"

/*! 
 * \brief   Manage Data Sources
 * 
 *          SQLManageDataSources done using Qt. Here is where we enter into Qt 'world'.
 * 
 * \param   hWnd
 * 
 * \return  BOOL
 */
static BOOL QT5DriverConnectPrompt( HWND hWnd, SQLCHAR *pszConnectString, SQLSMALLINT nMaxChars )
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
        QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Driver Connect Prompt" ) );
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // sanity check...
    if ( !pszConnectString  )
    {
        QMessageBox::critical( pWidget, QObject::tr( "Select Data Source..." ), QObject::tr("Invalid buffer for data source name!") );
        return false;
    }

    if ( nMaxChars <= 1 )
    {
        QMessageBox::critical( pWidget, QObject::tr("Select Data Source..."), QObject::tr("Invalid max char value for data source name!") );
        return false;
    }

    // show dialog...
    QString stringIn = QString::fromLocal8Bit( (const char *)pszConnectString, nMaxChars );
    CDriverConnectPrompt o( stringIn, pWidget );
    if ( o.exec() == QDialog::Accepted )
    {
        QString stringConnectString = o.getConnectString();
        if ( stringConnectString.length() >= nMaxChars )
        {
            QMessageBox::warning( pWidget, QObject::tr("Select a data source name..."), QObject::tr("Buffer provided is too small. Select a Data Source Name with less characters.") );
            return false;
        }

        /* Convert string to 8-bit ASCII while we copy it.
         * Non ASCII characters will result in character loss.
         */
        memset( pszConnectString, '\0', nMaxChars ); // in case we are truncating
        strncpy( (char *)pszConnectString, stringConnectString.toLocal8Bit().constData(), nMaxChars );
        return true;
    }

    return false;
}

static BOOL QT5DriverConnectPromptW( HWND hWnd, SQLWCHAR *pszConnectString, SQLSMALLINT nMaxChars )
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
        QCoreApplication::setApplicationName( QString::fromLocal8Bit( "ODBC Driver Connect Prompt" ) );
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // sanity check...
    if ( !pszConnectString  )
    {
        QMessageBox::critical( pWidget, QObject::tr("Select Data Source..."), QObject::tr("Invalid buffer for data source name!") );
        return false;
    }

    if ( nMaxChars <= 1 )
    {
        QMessageBox::critical( pWidget, QObject::tr("Select Data Source..."), QObject::tr("Invalid max char value for data source name!") );
        return false;
    }

    // show dialog...
    QString stringIn = QString::fromUtf16( pszConnectString, nMaxChars );
    CDriverConnectPrompt o( stringIn, pWidget );
    if ( o.exec() == QDialog::Accepted )
    {
        QString stringConnectString = o.getConnectString();
        if ( stringConnectString.length() >= nMaxChars )
        {
            QMessageBox::warning( pWidget, QObject::tr("Select a data source name..."), QObject::tr("Buffer provided is too small. Select a Data Source Name with less characters.") );
            return false;
        }

        /* Here we assume the app is using UTF-16 encoding (we know ODBC is).
         * QChar is supposed to be UTF-16 so We simply copy the UTF-16 encoded string to pszConnectString.
         * This seems suspect for variable-width characters.
         */
        int n = 0;
        for ( ; n < stringConnectString.length() && n < nMaxChars; n++ ) 
        {
            pszConnectString[n] = stringConnectString[n].unicode();
        }
        if ( n < nMaxChars ) pszConnectString[n] = QChar::Null; else pszConnectString[nMaxChars-1] = QChar::Null;
        return true;
    }

    return false;
}

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief   Plugin in entry point.
 *
 *          This entry point is called to implement the SQLDriverConnectPrompt call.
 * 
 *          This plugin does it using Qt.
 * 
 *          Calling SQLDriverConnect in the Driver Manager *can* lead to a call to _SQLDriverConnectPrompt in
 *          the odbcinst library - to prompt the User. odbcinst will try to load the GUI plugin (ie us) based upon
 *          information found in the HWND and call ODBCDriverConnectPrompt.
 * 
 * \param   HWND        This is a QWidget* - the parent widget. Can be NULL - in which case the desktop widget is used.
 * \param   SQLCHAR     String buffer. We return the connection string in this after prompting.
 *                      Specification says this could be a partially/completely formed connection string. However; unixODBC
 *                      calls us with a totally uninitialized string buffer - always.
 * \param   SQLSMALLINT The number of chars that can be stored in the string buffer.
 * \return  BOOL
 * \retval  TRUE    pszConnectString contains a viable data source name.
 * \retval  FALSE   User cancelled or something went wrong.
 * 
 * \sa      CDriverConnectPrompt
 *          unixODBC::DriverManager::SQLDriverConnect
 *          unixODBC::DriverManager::SQLDriverConnectPrompt
 */
BOOL ODBCDriverConnectPrompt( HWND hWnd, SQLCHAR *pszConnectString, SQLSMALLINT nMaxChars )
{
    return QT5DriverConnectPrompt( hWnd, pszConnectString, nMaxChars );
}

BOOL ODBCDriverConnectPromptW( HWND hWnd, SQLWCHAR *pszConnectString, SQLSMALLINT nMaxChars )
{
    return QT5DriverConnectPromptW( hWnd, pszConnectString, nMaxChars );
}

#ifdef __cplusplus
};
#endif

