/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#include "OQSystem.h"
#include <odbcinst.h>

OQSystem::OQSystem()
    : OQHandle( Sys )
{
    setObjectName( QString::fromLocal8Bit("OQSystem") );
}

OQSystem::~OQSystem()
{
}

/*!
 * \brief   Get ODBC system-wide attributes. 
 *  
 *          These attributes are system-wide and include such things as connection pooling
 *          and DM tracing. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   pnReturn 
 * QVector
 * \return  OQAttributes 
 */
OQAttributes OQSystem::getAttributes( SQLRETURN *pnReturn )
{
    SQLRETURN       nReturn;
    OQAttributes    Attributes = getAttributes( QString::fromLocal8Bit("ODBCINST.INI"), QString::fromLocal8Bit("ODBC"), &nReturn );

    if ( pnReturn ) *pnReturn = nReturn;

    return Attributes;
}

/*!
 * \brief   Get driver attributes. 
 *  
 *          This gets the driver attributes from ODBCINST.INI. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringDriver 
 * \param   pnReturn 
 * 
 * \return  OQDriver 
 */
OQDriver OQSystem::getDriver( const QString &stringDriver, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    OQDriver    Driver = getAttributes( QString::fromLocal8Bit("ODBCINST.INI"), stringDriver, &nReturn );

    if ( pnReturn ) *pnReturn = nReturn;

    return Driver;
}

/*!
 * \brief   Get a list of registered drivers.  
 *  
 *          The list comes from doGetInstalledDrivers().
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   pnReturn 
 * 
 * \return  QVector<QString> 
 */
QVector<QString> OQSystem::getDriverNames( SQLRETURN *pnReturn )
{
    QVector<QString> vectorDriverNames;

    BOOL b = doGetInstalledDrivers( &vectorDriverNames );

    if ( pnReturn )
        *pnReturn = (  b ? SQL_SUCCESS : SQL_ERROR );

    return vectorDriverNames;
}

/*!
 * \brief   Get driver information for all registered drivers.
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   pnReturn 
 * 
 * \return  QVector<OQDriver> 
 */
QVector<OQDriver> OQSystem::getDrivers( SQLRETURN *pnReturn )
{
    SQLRETURN           nReturn;
    QVector<OQDriver>   vectorDrivers;

    // get a list of driver names...
    QVector<QString> vectorDriverNames = getDriverNames( &nReturn );

    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        if ( pnReturn ) *pnReturn = nReturn;
        return vectorDrivers;
    }

    // get & store details...
    for ( int n = 0; n < vectorDriverNames.size(); n++ )
    {
        OQDriver Driver = getDriver( vectorDriverNames[n], &nReturn );
        if ( !SQL_SUCCEEDED( nReturn ) )
        {
            if ( pnReturn ) *pnReturn = nReturn;
            return vectorDrivers;
        }
        vectorDrivers.append( Driver );
    }

    return vectorDrivers;
}

/*!
 * \brief   Get data source information.
 *  
 *          Gets data source information using the given scope.
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringDataSourceName 
 * \param   nScope 
 * \param   pnReturn 
 * 
 * \return  OQDataSourceName 
 */
OQDataSourceName OQSystem::getDataSource( const QString &stringDataSourceName, OQDataSourceName::enumScopes nScope, SQLRETURN *pnReturn )
{
    OQDataSourceName::enumScopes nScopeOrig = OQDataSourceName::ScopeBoth;

    doGetConfigMode( &nScopeOrig );
    doSetConfigMode( nScope );

    SQLRETURN           nReturn;
    SQLRETURN *         pnRet = ( pnReturn ? pnReturn : &nReturn );
    OQDataSourceName    DataSourceName( QString::fromLocal8Bit("ODBC.INI"), nScope );

    DataSourceName.Attributes = getAttributes( DataSourceName.stringFileName, stringDataSourceName, pnRet );

    SQLSetConfigMode( nScopeOrig );

    return DataSourceName;
}

/*!
 * \brief   Get data source information. 
 *  
 *          Gets data source information using the given file name. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringDataSourceName 
 * \param   stringFilename 
 * \param   pnReturn 
 * 
 * \return  OQDataSourceName 
 */
OQDataSourceName OQSystem::getDataSource( const QString &stringDataSourceName, const QString &stringFileName, SQLRETURN *pnReturn )
{
    OQDataSourceName::enumScopes nScope = OQDataSourceName::ScopeBoth;

    doGetConfigMode( &nScope );

    if ( stringFileName.toUpper() == QString::fromLocal8Bit( "ODBC.INI" ) )
        return getDataSource( stringDataSourceName, nScope, pnReturn );

    SQLRETURN           nReturn;
    SQLRETURN *         pnRet = ( pnReturn ? pnReturn : &nReturn );
    OQDataSourceName    DataSourceName( stringFileName, nScope );

    DataSourceName.Attributes = getAttributes( stringFileName, stringDataSourceName, pnRet );

    return DataSourceName;
}

/*!
 * \brief   Get a list of data sources.
 *  
 *          Gets a list of data sources (DSN's) from system information using
 *          the given scope. This does NOT include file DSN's.
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   nScope 
 * \param   pnReturn 
 * 
 * \return  QVector<QString> 
 */
QVector<QString> OQSystem::getDataSourceNames( OQDataSourceName::enumScopes nScope, SQLRETURN *pnReturn )
{
    QVector<QString>                vectorDataSourceNames;
    OQDataSourceName::enumScopes    nScopeOrig = OQDataSourceName::ScopeBoth;

    doGetConfigMode( &nScopeOrig );
    doSetConfigMode( nScope );

    BOOL b = doGetPrivateProfileString( QString::null, &vectorDataSourceNames, QString::fromLocal8Bit( "ODBC.INI" ) );
    if ( pnReturn ) *pnReturn = ( b ? SQL_SUCCESS : SQL_ERROR );

    SQLSetConfigMode( nScopeOrig );

    return vectorDataSourceNames;
}

/*!
 * \brief   Get information on all data sources (DSN's).
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   nScope 
 * \param   pnReturn 
 * 
 * \return  QVector<OQDataSourceName> 
 */
QVector<OQDataSourceName> OQSystem::getDataSources( OQDataSourceName::enumScopes nScope, SQLRETURN *pnReturn )
{
    QVector<OQDataSourceName>   vectorDataSources;
    QVector<QString>            vectorDataSourceNames;
    SQLRETURN                   nReturn;
    SQLRETURN *                 pnRet = ( pnReturn ? pnReturn : &nReturn );

    // get a list data source names...
    vectorDataSourceNames = getDataSourceNames( nScope, pnRet );
    if ( !SQL_SUCCEEDED(*pnRet) )
        return vectorDataSources;

    // get data source objects..
    for ( int n = 0; n <  vectorDataSourceNames.size(); n++ )
    {
        OQDataSourceName DataSourceName = getDataSource( vectorDataSourceNames[n], nScope, pnRet ); 
        if ( !SQL_SUCCEEDED(*pnRet) )
            return vectorDataSources;

        vectorDataSources.append( DataSourceName );
    }

    return vectorDataSources;
}

/*!
 * \brief   Get the system error at the given index.
 *  
 *          The system error is usually an ODBC installer error.
 *  
 * \author  pharvey (10/6/2009)
 * 
 * \param   nIndex The spec says 1-8 is valid but more are often supported.
 * \param   pnRetCode 
 * 
 * \return  OQSystemError 
 */
OQSystemError OQSystem::getSystemError( WORD nIndex /* 1 - 8 */, RETCODE *pnRetCode )
{
    OQSystemError SystemError;

    RETCODE nRetCode = doInstallerError( nIndex, &(SystemError.nErrorCode), &(SystemError.stringErrorMsg) );

    if ( pnRetCode ) *pnRetCode = nRetCode;

    return SystemError;
}

/*!
 * \brief   Get all system erros currently available.
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \return  QVector<OQSystemError> 
 */
QVector<OQSystemError> OQSystem::getSystemErrors()
{
    QVector<OQSystemError>  vectorSystemErrors;
    RETCODE                 nRetCode = SQL_SUCCESS;

    for ( WORD n = 0; SQL_SUCCEEDED( nRetCode ); n++ )
    {
        OQSystemError SystemError = getSystemError( n, &nRetCode );
        if ( SQL_SUCCEEDED( nRetCode )  )
            vectorSystemErrors.append( SystemError );
    }

    return vectorSystemErrors;
}

/*!
 * \brief   Invokes a UI to allow User to create a new Data Source Name (DSN). 
 *  
 *          This call will work on MS Windows or when using unixODBC. 
 * 
 * \author pharvey (3/27/18)
 * 
 * \param hwnd Handle to parent window (ODBCINSTWND* when using unixODBC).
 * \param stringDS A new Data Source Name (can be NULL). Input only.
 * 
 * \return BOOL true || false
 */
BOOL OQSystem::doCreateDataSource( HWND hwnd, const QString &stringDSN )
{
    // we can pass a NULL for 2nd arg but a bug has us passing an empty string instead
    QString stringEmpty( QString::fromLocal8Bit("") );
    return SQLCreateDataSourceW( hwnd, ( stringDSN.isNull() ? (SQLWCHAR*)(stringEmpty.utf16()) : (SQLWCHAR*)(stringDSN.utf16()) ) ); 
}

/*! 
 *  \f$     doManageDataSources
 *  \brief  Wrapper for SQLManageDataSources. Invoke a GUI (an ODBC Administrator).
 *  
 *  Q_OS_MACOS  We execute an external program 'ODBC Administrator.app'. In this case
 *              hWnd is ignored.
 *  
 *  Q_OS_UNIX   We simply pass call to SQLManageDataSources().
 *              If we are using unixODBC (most likley) the hWnd should be a
 *              pointer to an ODBCINSTWND. SQLGetInfo() can be used to determine
 *              if unixODBC is being used.
 *  
 *  Q_OS_WIN    We simply pass call to SQLManageDataSources().
 *  
 */
#if defined(Q_OS_MACOS)
BOOL OQSystem::doManageDataSources( HWND )
{
    QProcess  * pprocess = new QProcess( this );

    pprocess->addArgument( "open" );
    pprocess->addArgument( "/Applications/Utilities/ODBC Administrator.app" ); // OSX Jaguar now has a standard ODBC Administrator :)
    if ( pprocess->start() ) 
    {
        return true;
    }
    
    delete pprocess;

    return false;
}
#elif defined(Q_OS_UNIX)
BOOL OQSystem::doManageDataSources( HWND hWnd )
{
    /* The caller should initialize an ODBCINSTWND as follows and then
     * pass that to us as hWnd. This example is for Qt5.
     *
     *  ODBCINSTWND odbcinstwnd;
     *  strcpy( odbcinstwnd.szUI, "odbcinstQ5" );
     *  odbcinstwnd.hWnd = hWnd; // QWidget
     *  
     *  SQLManageDataSources( (HWND)(&odbcinstwnd) );
     */

    return SQLManageDataSources( hWnd );
}
#elif defined(Q_OS_WIN)
BOOL OQSystem::doManageDataSources( HWND hWnd )
{

    return SQLManageDataSources( hWnd );
}
#else
    #error Platform not supported.
#endif

/*!
 * \brief   Gets all key/values for the given 'section'. 
 *  
 *          The 'section' is usually a driver or a dsn. 
 * 
 * \author  pharvey (10/6/2009)
 * 
 * \param   stringFilename 
 * \param   stringSection 
 * \param   pnReturn 
 * 
 * \return  OQAttributes 
 */
OQAttributes OQSystem::getAttributes( const QString &stringFileName, const QString &stringSection, SQLRETURN *pnReturn )
{
    OQAttributes        Attributes;
    QVector<QString>    vectorKeys;
    SQLRETURN           nReturn;
    SQLRETURN *         pnRet = ( pnReturn ? pnReturn : &nReturn );
    BOOL                b;

    b = doGetPrivateProfileString( stringSection, &vectorKeys, stringFileName );
    *pnRet = ( b ? SQL_SUCCESS : SQL_ERROR );

    for ( int n = 0; n < vectorKeys.size(); n++ )
    {
        QString stringValue;
        b = doGetPrivateProfileString( stringSection, vectorKeys[n], QString::null, &stringValue, stringFileName );
        *pnRet = ( b ? SQL_SUCCESS : SQL_ERROR );
        if ( b )
            Attributes.mapAttributes.insert( vectorKeys[n], stringValue );
    }

    Attributes.stringName = stringSection;

    return Attributes;
}

/* 
BOOL OQSystem::doConfigDataSource( HWND hwndParent, enumConfigDataSourceRequest nRequest, const QString &stringDriver, const QMap<Qstring,QString> &mapAttributes )
{
    LPTSTR  pszDriver       = (LPTSTR)OQFromQString(stringDriver);
    LPTSTR  pszAttributes   = getAttributes( mapAttributes );

    BOOL b = SQLConfigDataSource( hwndParent, (WORD)nRequest, pszDriver, pszAttributes );

    free( pszAttributes );

    return b;
}
*/

/*
BOOL OQSystem::doConfigDriver( HWND hwndParent, enumConfigDriverRequest nRequest, const QString &stringDriver, const QString &stringArgs, QString *pstringMsg )
{
    BOOL    b;
    LPTSTR  pszDriver   = (LPTSTR)OQFromQString(stringDriver);
    LPTSTR  pszArgs     = (LPTSTR)OQFromQString(stringArgs);
    WORD    nMsgMax     = 512;
    WCHAR   szMsg[nMsgMax];
    WORD    nMsgMaxOut  = 0;

    b = SQLConfigDriver( hwndParent, (WORD)nRequest, pszDriver, pszArgs, szMsg, nMsgMax, &nMsgMaxOut );
    if ( nMsgMaxOut <= nMsgMax )
    {
        nMsgMax = (nMsgMaxOut + 2);
        {
            WCHAR szMsg[nMsgMax];

            b = SQLConfigDriver( hwndParent, (WORD)nRequest, pszDriver, pszArgs, szMsg, nMsgMax, &nMsgMaxOut );
            if ( b && pstringMsg )
                *pstringMsg = OQToQString( pszMsg );
        }
    }
    else if ( b && pstringMsg )
        *pstringMsg = OQToQString( pszMsg );

    return b;
}
*/

/*!
 * \brief   Wrapper for SQLGetConfigMode.
 * 
 * \author pharvey (10/6/2009)
 * 
 * \param pnConfigMode 
 * 
 * \return BOOL 
 */
BOOL OQSystem::doGetConfigMode( OQDataSourceName::enumScopes *pnConfigMode )
{
    return SQLGetConfigMode( (UWORD*)pnConfigMode );
}

/*!
 * \brief   Wrapper for SQLGetInstalledDrivers.
 * 
 * \author pharvey (10/6/2009)
 * 
 * \param pvectorDrivers 
 * 
 * \return BOOL 
 */
BOOL OQSystem::doGetInstalledDrivers( QVector<QString> *pvectorDrivers )
{
    BOOL        b = false;
    WORD        nCharMax = 1024;
    WCHAR       szChar[nCharMax];
    WORD        nCharOut = 0;

    if ( !pvectorDrivers )
        return false;

    b = SQLGetInstalledDriversW( szChar, nCharMax, &nCharOut );
    if ( !b )
        return false;

    // Buffer too small? Try again with buffer size we actually need.
    if ( nCharOut <= nCharMax )
    {
        nCharMax = (nCharOut + 2);
        {
            WCHAR szChar[nCharMax];

            b = SQLGetInstalledDriversW( szChar, nCharMax, &nCharOut );
            if ( !b )
                return false;
            *pvectorDrivers = getParsedStrings( szChar );
            return true;
        }
    }

    *pvectorDrivers = getParsedStrings( szChar );

    return true;
}

/*!
 * \brief   Wrapper for SQLGetPrivateProfileString. 
 *  
 *          Get ODBC system information from INI files and/or registry. Use this when the
 *          result is expected to be a single string.
 * 
 * \author  pharvey (10/5/2009)  
 * 
 * \param   stringSection 
 * \param   stringEntry 
 * \param   stringDefault 
 * \param   pstring 
 * \param   stringFilename 
 * 
 * \return  BOOL 
 */
BOOL OQSystem::doGetPrivateProfileString( const QString &stringSection, const QString &stringEntry, const QString &stringDefault, QString *pstring, const QString &stringFilename )
{
    if ( !pstring )
    {
        // post an error
        return false;
    }

    pstring->clear();

    LPCWSTR  pszSection = (LPCWSTR)(stringSection.utf16());
    LPCWSTR  pszEntry   = (LPCWSTR)(stringEntry.utf16());
    LPCWSTR  pszDefault = (LPCWSTR)(stringDefault.utf16());
    LPCWSTR  pszFilename= (LPCWSTR)(stringFilename.utf16());

    int     nCharsMax = 1024;
    WCHAR   szChars[nCharsMax];
    memset( szChars, '\0', nCharsMax * sizeof(WCHAR) );

    int nCharsRead = SQLGetPrivateProfileStringW( pszSection, pszEntry, pszDefault, szChars, nCharsMax, pszFilename );

    if ( nCharsRead < 1 )
        return true;

    *pstring = QString::fromUtf16( szChars );

    return true;
}

/*!
 * \brief   Wrapper for SQLGetPrivateProfileString. 
 *  
 *          Get ODBC system information from INI files and/or registry. Use this when
 *          the result is expected to be a string list - when getting all sections or
 *          all keys for a specified section.
 * 
 * \author  pharvey (10/5/2009)
 * 
 * \param   stringSection 
 * \param   stringEntry 
 * \param   pvectorStrings 
 * \param   stringFilename 
 * 
 * \return  BOOL 
 */
BOOL OQSystem::doGetPrivateProfileString( const QString &stringSection, QVector<QString> *pvectorStrings, const QString &stringFilename )
{
    if ( !pvectorStrings )
    {
        // post an error
        return false;
    }

    pvectorStrings->clear();

    LPCWSTR  pszSection      = (LPCWSTR)(stringSection.utf16());
    LPCWSTR  pszFilename     = (LPCWSTR)(stringFilename.utf16());

    int     nCharsMax = 4096;
    WCHAR   szChars[nCharsMax];
    memset( szChars, '\0', nCharsMax * sizeof(WCHAR) );

    int nCharsRead = SQLGetPrivateProfileStringW( pszSection, NULL, NULL, szChars, nCharsMax, pszFilename );

    if ( nCharsRead < 1 )
        return true;

    // decode result
    pvectorStrings->clear();
    for ( WCHAR *pszCursor = szChars; pszCursor[1] != '\0'; )
    {
        // grab the string
        pvectorStrings->append( QString::fromUtf16( pszCursor ) );
        // scan until next string
        while ( *pszCursor != '\0' )
            pszCursor++;
        pszCursor++;
    }

    return true;
}

/*!
 * \brief   Wrapper for SQLInstallerError.
 * 
 * \author pharvey (10/6/2009)
 * 
 * \param nError 
 * \param pnErrorCode 
 * \param pstringMsg 
 * 
 * \return RETCODE 
 */
RETCODE OQSystem::doInstallerError( WORD nError, DWORD *pnErrorCode, QString *pstringMsg )
{
    WORD    nErrorMsgMaxChars               = 512;
    WORD    nErrorMsgChars                  = 0;
    WCHAR   szErrorMsg[nErrorMsgMaxChars];
    RETCODE nRetCode;

    memset( szErrorMsg, '\0', nErrorMsgMaxChars * sizeof(WCHAR) );

    nRetCode = SQLInstallerErrorW( nError, pnErrorCode, szErrorMsg, nErrorMsgMaxChars, &nErrorMsgChars );
    if ( nErrorMsgChars >= nErrorMsgMaxChars  )
    {
        nErrorMsgMaxChars = nErrorMsgChars + 1;
        {
            WCHAR szErrorMsg[nErrorMsgMaxChars];
            memset( szErrorMsg, '\0', nErrorMsgMaxChars * sizeof(WCHAR) );
            nRetCode = SQLInstallerErrorW( nError, pnErrorCode, szErrorMsg, nErrorMsgMaxChars, &nErrorMsgChars );
        }
    }

    if ( SQL_SUCCEEDED(nRetCode) && pstringMsg )
        *pstringMsg = QString::fromUtf16( szErrorMsg );

    return nRetCode;
}

/* 
RETCODE OQSystem::doPostInstallerError( DWORD nErrorCode, const QString &stringErrorMsg )
{
    return SQLPostInstallerError( nErrorCode, (LPTSTR)OQFromQString(stringErrorMsg); );
}
*/

/*!
 * \brief   Wrapper for SQLSetConfigMode.
 * 
 * \author pharvey (10/6/2009)
 * 
 * \param nConfigMode 
 * 
 * \return BOOL 
 */
BOOL OQSystem::doSetConfigMode( OQDataSourceName::enumScopes nConfigMode )
{
    return SQLSetConfigMode( (UWORD)nConfigMode );
}

/*!
 * \brief Parse result string from SQLGetPrivateProfileString().
 * 
 * \author pharvey (3/26/18)
 * 
 * \param sz    This is a series of null terminated strings where 2 consecutive 
 *              nulls means no more data.
 * 
 * \return QVector&lt;QString&gt;   
 */
QVector<QString> OQSystem::getParsedStrings( WCHAR* sz )
{
    WCHAR *             pszCursor = sz;
    QVector<QString>    vectorStrings;
    QString             s;

    if ( !sz ) return vectorStrings;

    s = QString::fromUtf16( pszCursor );
    while ( 1 )
    {
        if ( !s.isEmpty() )
            vectorStrings.append( s );

        pszCursor += s.length() + 1;
        s = QString::fromUtf16( pszCursor );

        if ( s.isEmpty() )
            break;
    }

    return vectorStrings;
}

