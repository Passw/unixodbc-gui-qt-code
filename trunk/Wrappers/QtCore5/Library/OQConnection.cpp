/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#include "OQEnvironment.h"
#include "OQConnection.h"
#include "OQStatement.h"

OQConnection::OQConnection( OQEnvironment *penvironment )
    : OQHandle( Dbc, penvironment )
{
    setObjectName( QString::fromLocal8Bit( "OQConnection" ) );

    bPromptDriver           = true;
    bPromptDataSourceName   = true;
    bPromptUserID           = true;
    bPromptPassword         = true;
    bConnected              = false;
}

OQConnection::~OQConnection()
{
    // increase the chances that cleanup will complete fully
    if ( isConnected() )
        doDisconnect();
}

// fixed-length: SQLINTEGER, SQL_IS_INTEGER
//               SQLUINTEGER, SQL_IS_UINTEGER
//               SQLLEN/SQLULEN is dumbed down (if diff) to SQLINTEGER/SQLUINTEGER
// string: SQLWCHAR*, SQL_NTS (QString::utf16() is always null term)
// binary: SQL_POINTER, SQL_LEN_BINARY_ATTR(len)
// others: SQL_POINTER, SQL_IS_POINTER
SQLRETURN OQConnection::setAttrAccessMode( AttrAccessModeTypes nAttrAccessMode )
{
    return setConnectAttr( SQL_ATTR_ACCESS_MODE, (SQLUINTEGER)nAttrAccessMode );
}

SQLRETURN OQConnection::setAttrAsyncEnable( AttrAsyncEnableTypes nAttrAsyncEnable )
{
    return setConnectAttr( SQL_ATTR_ASYNC_ENABLE, (SQLUINTEGER)nAttrAsyncEnable );
}

SQLRETURN OQConnection::setAttrAutocommit( AttrAutocommitTypes nAttrAutocommit )
{
    return setConnectAttr( SQL_ATTR_AUTOCOMMIT, (SQLUINTEGER)nAttrAutocommit );
}

SQLRETURN OQConnection::setAttrConnectionTimeout( SQLUINTEGER nAttrConnectionTimeout )
{
    return setConnectAttr( SQL_ATTR_CONNECTION_TIMEOUT, (SQLUINTEGER)nAttrConnectionTimeout );
}

SQLRETURN OQConnection::setAttrCurrentCatalog( const QString &stringAttrCurrentCatalog )
{
    return setConnectAttr( SQL_ATTR_CURRENT_CATALOG, stringAttrCurrentCatalog );
}

SQLRETURN OQConnection::setAttrLoginTimeout( SQLUINTEGER nAttrLoginTimeout )
{
    return setConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, (SQLUINTEGER)nAttrLoginTimeout );
}

SQLRETURN OQConnection::setAttrMetadataId( bool bAttrMetadataId )
{
    return setConnectAttr( SQL_ATTR_METADATA_ID, (SQLUINTEGER)bAttrMetadataId );
}

SQLRETURN OQConnection::setAttrOdbcCursors( AttrOdbcCursorsTypes nAttrOdbcCursors )
{
    return setConnectAttr( SQL_ATTR_ODBC_CURSORS, (SQLUINTEGER)nAttrOdbcCursors );
}

SQLRETURN OQConnection::setAttrPacketSize( SQLUINTEGER nAttrPacketSize )
{
    return setConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, nAttrPacketSize );
}

SQLRETURN OQConnection::setAttrQuietMode( SQLHWND hWnd )
{
    Q_UNUSED(hWnd);
    // \todo Need to come up with a cross-platform/cross-GUI way to handle this that works for Drivers.
    // return setConnectAttr( SQL_ATTR_QUIET_MODE, (SQLPOINTER)hWnd );
    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Not supported at this time.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::setAttrTrace( AttrTraceTypes nAttrTrace )
{
    return setConnectAttr( SQL_ATTR_TRACE, (SQLUINTEGER)nAttrTrace );
}

SQLRETURN OQConnection::setAttrTracefile( const QString &stringAttrTracefile )
{
    return setConnectAttr( SQL_ATTR_TRACEFILE, stringAttrTracefile );
}

SQLRETURN OQConnection::setAttrTranslateLib( const QString &stringAttrTranslateLib )
{
    return setConnectAttr( SQL_ATTR_TRANSLATE_LIB, stringAttrTranslateLib );
}

SQLRETURN OQConnection::setAttrTranslateOption( SQLUINTEGER nAttrTranslateOption )
{
    return setConnectAttr( SQL_ATTR_TRANSLATE_OPTION, nAttrTranslateOption );
}

SQLRETURN OQConnection::setAttrTxnIsolation( SQLUINTEGER nAttrTxnIsolation )
{
    return setConnectAttr( SQL_ATTR_TXN_ISOLATION, nAttrTxnIsolation );
}

OQConnection::AttrAccessModeTypes OQConnection::getAttrAccessMode( SQLRETURN *pn )
{
    AttrAccessModeTypes nAttrAccessMode = ModeReadWrite;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ACCESS_MODE, (SQLUINTEGER*)(&nAttrAccessMode) );
    if ( pn ) *pn = nReturn;
    return nAttrAccessMode;
}

OQConnection::AttrAsyncEnableTypes OQConnection::getAttrAsyncEnable( SQLRETURN *pn ) 
{
    AttrAsyncEnableTypes nAttrAsyncEnable = AsyncEnableOff;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ASYNC_ENABLE, (SQLUINTEGER*)(&nAttrAsyncEnable) );
    if ( pn ) *pn = nReturn;
    return nAttrAsyncEnable;
}

bool OQConnection::getAttrAutoIpd( SQLRETURN *pn )
{
    SQLUINTEGER bAttrAutoIpd = SQL_FALSE;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_AUTO_IPD, &bAttrAutoIpd );
    if ( pn ) *pn = nReturn;
    return (bool)bAttrAutoIpd;
}

OQConnection::AttrAutocommitTypes OQConnection::getAttrAutocommit( SQLRETURN *pn )
{
    SQLUINTEGER nAttrAutocommit = AutocommitOn;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_AUTOCOMMIT, &nAttrAutocommit );
    if ( pn ) *pn = nReturn;
    return (AttrAutocommitTypes)nAttrAutocommit;
}

bool OQConnection::getAttrConnectionDead( SQLRETURN *pn )
{
    SQLUINTEGER bAttrConnectionDead = SQL_TRUE;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_CONNECTION_DEAD, &bAttrConnectionDead );
    if ( pn ) *pn = nReturn;
    return (bool)bAttrConnectionDead;
}

SQLUINTEGER OQConnection::getAttrConnectionTimeout( SQLRETURN *pn )
{
    SQLUINTEGER nAttrConnectionTimeout = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_CONNECTION_DEAD, &nAttrConnectionTimeout );
    if ( pn ) *pn = nReturn;
    return nAttrConnectionTimeout;
}

QString OQConnection::getAttrCurrentCatalog( SQLRETURN *pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_CURRENT_CATALOG, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

SQLUINTEGER OQConnection::getAttrLoginTimeout( SQLRETURN *pn )
{
    SQLUINTEGER nAttrLoginTimeout = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_LOGIN_TIMEOUT, &nAttrLoginTimeout );
    if ( pn ) *pn = nReturn;
    return nAttrLoginTimeout;
}

bool OQConnection::getAttrMetadataId( SQLRETURN *pn )
{
    SQLUINTEGER bAttrMetadataId = SQL_TRUE;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_METADATA_ID, &bAttrMetadataId );
    if ( pn ) *pn = nReturn;
    return (bool)bAttrMetadataId;
}

OQConnection::AttrOdbcCursorsTypes OQConnection::getAttrOdbcCursors( SQLRETURN *pn )
{
    SQLUINTEGER nAttrOdbcCursors = CurUseDriver;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_ODBC_CURSORS, &nAttrOdbcCursors );
    if ( pn ) *pn = nReturn;
    return (AttrOdbcCursorsTypes)nAttrOdbcCursors;
}

SQLUINTEGER OQConnection::getAttrPacketSize( SQLRETURN *pn )
{
    SQLUINTEGER nAttrPacketSize = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_PACKET_SIZE, &nAttrPacketSize );
    if ( pn ) *pn = nReturn;
    return nAttrPacketSize;
}

SQLHWND OQConnection::getAttrQuietMode( SQLRETURN *pn )
{
    // SQLHWND hWnd = NULL;
    // SQLRETURN nReturn = getConnectAttr( SQL_ATTR_QUIET_MODE, &hWnd );
    // if ( pn ) *pn = nReturn;
    // return hWnd;

    // \sa setAttrQuietMode() for more on this.
    if ( pn ) *pn = SQL_ERROR;
    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Not supported at this time.") ) );
    return 0;
}

OQConnection::AttrTraceTypes OQConnection::getAttrTrace( SQLRETURN *pn )
{
    SQLUINTEGER nAttrTrace = OptTraceOff;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TRACE, &nAttrTrace );
    if ( pn ) *pn = nReturn;
    return (AttrTraceTypes)nAttrTrace;
}

QString OQConnection::getAttrTracefile( SQLRETURN *pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_TRACEFILE, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

QString OQConnection::getAttrTranslateLib( SQLRETURN *pn )
{
    QString     stringValue;
    SQLRETURN   nReturn = getConnectAttr( SQL_ATTR_TRANSLATE_LIB, &stringValue );

    if ( pn ) *pn = nReturn;

    return stringValue;
}

SQLUINTEGER OQConnection::getAttrTranslateOption( SQLRETURN *pn )
{
    SQLUINTEGER nAttrTranslateOption = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TRANSLATE_OPTION, &nAttrTranslateOption );
    if ( pn ) *pn = nReturn;
    return nAttrTranslateOption;
}

SQLUINTEGER OQConnection::getAttrTxnIsolation( SQLRETURN *pn )
{
    SQLUINTEGER nAttrTxnIsolation = 0;
    SQLRETURN nReturn = getConnectAttr( SQL_ATTR_TXN_ISOLATION, &nAttrTxnIsolation );
    if ( pn ) *pn = nReturn;
    return nAttrTxnIsolation;
}

/*!
    getExecute
    
    Creates a result set (or NULL if there was an error).
    See OQStatement::getTables.
*/
OQStatement *OQConnection::getExecute( const QString &stringStatement )
{
    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Not connected.") ) );
        return 0;
    }

    OQStatement *   pStatement  = new OQStatement( this );
    SQLRETURN       nReturn     = pStatement->slotExecDirect( stringStatement );

    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getCatalogs
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getTables.
*/
OQStatement *OQConnection::getCatalogs()
{
    OQStatement *    pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doTables(QString::fromLocal8Bit( SQL_ALL_CATALOGS ), QString::null, QString::null );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getSchemas
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getTables.
*/
OQStatement *OQConnection::getSchemas( const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doTables( stringCatalog, QString::fromLocal8Bit( SQL_ALL_SCHEMAS ), QString::null );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getTables
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getTables.
*/
OQStatement *OQConnection::getTables( const QString &stringSchema, const QString &stringCatalog, const QString &stringType )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doTables( stringCatalog, stringSchema, QString::fromLocal8Bit( SQL_ALL_SCHEMAS ), stringType );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getViews
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getTables.
*/
OQStatement *OQConnection::getViews( const QString &stringSchema, const QString &stringCatalog, const QString &stringType )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doTables( stringCatalog, stringSchema, QString::fromLocal8Bit( SQL_ALL_SCHEMAS ), stringType );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getColumns
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getColumns.
*/
OQStatement *OQConnection::getColumns( const QString &stringTable, const QString &stringSchema, const QString &stringCatalog, const QString &stringType )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doColumns( stringCatalog, stringSchema, stringTable, stringType );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getIndexs
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getStatistics.
*/
OQStatement *OQConnection::getIndexs( const QString &stringTable, const QString &stringSchema, const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doStatistics( stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getPrimaryKey
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getPrimaryKey.
*/
OQStatement *OQConnection::getPrimaryKeys( const QString &stringTable, const QString &stringSchema, const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doPrimaryKeys( stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getForeignKeys
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getForeignKeys.
*/
OQStatement *OQConnection::getForeignKeys( const QString &stringTable, const QString &stringSchema, const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doForeignKeys( stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getSpecialColumns
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getSpecialColumns.
*/
OQStatement *OQConnection::getSpecialColumns( const QString &stringTable, const QString &stringSchema, const QString &stringCatalog )
{
    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    OQStatement *   pStatement      = new OQStatement( this );
    SQLSMALLINT     nIdentifierType = SQL_BEST_ROWID;
    SQLRETURN       nReturn         = pStatement->doSpecialColumns( nIdentifierType, stringCatalog, stringSchema, stringTable );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getProcedures
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getProcedures.
*/
OQStatement *OQConnection::getProcedures( const QString &stringSchema, const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doProcedures( stringCatalog, stringSchema );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getProcedureColumns
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getProcedureColumns.
*/
OQStatement *OQConnection::getProcedureColumns( const QString &stringProcedure, const QString &stringSchema, const QString &stringCatalog )
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doProcedureColumns( stringCatalog, stringSchema, stringProcedure );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    getDataTypes
        
    Creates a result set (or NULL if there was an error).
    See OQStatement::getypeInfo.
*/
OQStatement *OQConnection::getDataTypes()
{
    OQStatement *   pStatement = 0;
    SQLRETURN           nReturn;

    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Not connected." ) ) );
        return 0;
    }

    pStatement = new OQStatement( this );
    nReturn = pStatement->doTypeInfo();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        pStatement = 0;
    }

    return pStatement;
}

/*!
    doBrowseConnect
    
    Connects to the data source. emits signalConnected() if things work out.
*/
SQLRETURN OQConnection::doBrowseConnect( const QString &stringIn, QString *pstringOut )
{
    SQLWCHAR        szOut[4096];
    SQLSMALLINT     nCharsOutMax    = 4096;
    SQLSMALLINT     nCharsAvailable = 0;
    // we cast away const for arg1 as spec says its input only
    SQLRETURN       nReturn         = doBrowseConnect( (SQLWCHAR*)(stringIn.utf16()), SQL_NTS, szOut, nCharsOutMax, &nCharsAvailable );

    if ( SQL_SUCCEEDED( nReturn ) || nReturn == SQL_NEED_DATA )
    {
        // check for truncation
        if ( nCharsAvailable > nCharsOutMax )
        {
            *pstringOut = QString::fromUtf16( szOut, nCharsOutMax );
            eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), tr( "String truncated." ) ) );
        }
        else
            *pstringOut = QString::fromUtf16( szOut, nCharsAvailable );
    }

    return nReturn;
}

/*!
    doConnect
    
    Connects to the data source. emits signalConnected() if things work out.
*/
SQLRETURN OQConnection::doConnect( const QString &stringServerName, const QString &stringUserName, const QString &stringAuthentication )
{
    // we cast away the const as spec says they are input only
    return doConnect( (SQLWCHAR*)(stringServerName.utf16()), SQL_NTS, (SQLWCHAR*)(stringUserName.utf16()), SQL_NTS, (SQLWCHAR*)(stringAuthentication.utf16()), SQL_NTS );
}

/*!
    doDriverConnect
    
    Connects to the data source. emits signalConnected() if things work out.
*/
QString OQConnection::getDriverConnect( SQLHWND hWnd, const QString &stringIn, DriverPromptTypes nPrompt, SQLRETURN *pnReturn )
{
    SQLWCHAR        szOut[4096];
    SQLSMALLINT     nCharsOutMax    = 4096;
    SQLSMALLINT     nCharsAvailable = 0;
    SQLRETURN       nReturn;
    SQLRETURN *     pnRet = ( pnReturn ? pnReturn : &nReturn );

    // we cast away the const in arg2 as spec says its input only
    *pnRet = doDriverConnect( hWnd, (SQLWCHAR*)(stringIn.utf16()), SQL_NTS, szOut, nCharsOutMax, &nCharsAvailable, (SQLUSMALLINT)nPrompt );

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        // check for truncation
        if ( nCharsAvailable > nCharsOutMax )
        {
            *pstringOut = QString::fromUtf16( szOut, nCharsOutMax );
            eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), tr( "String truncated." ) ) );
        }
        else
            *pstringOut = QString::fromUtf16( szOut, nCharsAvailable );
    }

    return nReturn;
}

/*!
    doDisconnect
    
    Disconnects from data source.
*/
SQLRETURN OQConnection::doDisconnect()
{
    if ( !isAlloc( false ) )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Not allocated.") ) );
        return SQL_ERROR;
    }
    if ( !isConnected() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr( "Not connected." ) ) );
        return SQL_ERROR;
    }

    if ( children().count() )
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr( "Active statement(s)." ) ) );
        return SQL_ERROR;
    }

    SQLRETURN nReturn = SQLDisconnect( hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = false;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = false;
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr( "Unexpected SQLRETURN value." ), nReturn ) );
            break;
    }

    if ( !isConnected() )
    {
        stringDSN           = QString::null;
        stringUID           = QString::null;
        stringPWD           = QString::null;
        stringConnectString = QString::null;
        emit signalDisconnected();
    }

    return nReturn;
}

bool OQConnection::isConnected()
{
/*
    // SANITY CHECKS
    if ( hHandle == SQL_NULL_HDBC )
        return false;

    // DO IT
    SQLRETURN   nReturn;
    SQLUINTEGER nValue;
    SQLINTEGER  nRetSize;

    nReturn = SQLGetConnectAttr( hHandle, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER)(&nValue), SQL_IS_UINTEGER, &nRetSize  );
    if ( nReturn == SQL_SUCCESS )
    {
    }
    else if ( nReturn == SQL_SUCCESS_WITH_INFO )
    {
        doErrors();
    }
    else
    {
        // Lets not echo out the errors here and instead assume its because the
        // hHandle was never connected or because this feature has not been implemented
        // by the driver.
        // doErrors();
        return bConnected; // just use our state
    }

    // synch our connect state with the driver state
    if ( nValue == SQL_CD_TRUE )
    {
        if ( !bConnected )
            bConnected = true;
    }
    else
    {
        if ( bConnected )
            bConnected = false;
    }
*/
    return bConnected;
}

SQLRETURN OQConnection::setConnectAttr( SQLINTEGER nAttribute, SQLUINTEGER n )
{
    switch ( nAttribute )
    {
    case SQL_ATTR_ACCESS_MODE:
    case SQL_ATTR_ASYNC_ENABLE:
    case SQL_ATTR_AUTO_IPD:
    case SQL_ATTR_AUTOCOMMIT:
    case SQL_ATTR_CONNECTION_TIMEOUT:
    case SQL_ATTR_LOGIN_TIMEOUT:
    case SQL_ATTR_METADATA_ID:
    case SQL_ATTR_ODBC_CURSORS:
    case SQL_ATTR_PACKET_SIZE:
    case SQL_ATTR_TRACE:
    case SQL_ATTR_TRANSLATE_OPTION:     // 32bit flag
    case SQL_ATTR_TXN_ISOLATION:        // 32bit mask
        return setConnectAttr( nAttribute, (SQLPOINTER)n, SQL_IS_UINTEGER );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::setConnectAttr( SQLINTEGER nAttribute, SQLINTEGER n )
{
    switch ( nAttribute )
    {
        break; // no Attributes match data type
        return setConnectAttr( nAttribute, (SQLPOINTER)n, SQL_IS_INTEGER );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::setConnectAttr( SQLINTEGER nAttribute, const QString &stringValue )
{
    switch ( nAttribute )
    {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:
        return setConnectAttr( nAttribute, (SQLPOINTER)(stringValue.utf16()), SQL_NTS );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::setConnectAttr( SQLINTEGER nAttribute, QByteArray d )
{
    switch ( nAttribute )
    {
        break; // no Attributes match data type
        return setConnectAttr( nAttribute, (SQLPOINTER)(d.data()), SQL_LEN_BINARY_ATTR(d.size()) );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::setConnectAttr( SQLINTEGER nAttribute, SQLPOINTER p, SQLINTEGER n )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLSetConnectAttrW( hHandle, nAttribute, p, n );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQConnection::getConnectAttr( SQLINTEGER nAttribute, SQLUINTEGER *pn )
{
    switch ( nAttribute )
    {
    case SQL_ATTR_ACCESS_MODE:
    case SQL_ATTR_ASYNC_ENABLE:
    case SQL_ATTR_AUTO_IPD:
    case SQL_ATTR_AUTOCOMMIT:
    case SQL_ATTR_CONNECTION_TIMEOUT:
    case SQL_ATTR_LOGIN_TIMEOUT:
    case SQL_ATTR_METADATA_ID:
    case SQL_ATTR_ODBC_CURSORS:
    case SQL_ATTR_PACKET_SIZE:
    case SQL_ATTR_TRACE:
    case SQL_ATTR_TRANSLATE_OPTION:     // 32bit flag
    case SQL_ATTR_TXN_ISOLATION:        // 32bit mask
        return getConnectAttr( nAttribute, (SQLPOINTER)pn, 0, 0  );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::getConnectAttr( SQLINTEGER nAttribute, SQLINTEGER *pn )
{
    switch ( nAttribute )
    {
        break; // no Attributes match data type
        return getConnectAttr( nAttribute, (SQLPOINTER)pn, 0, 0  );
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::getConnectAttr( SQLINTEGER nAttribute, QString *pValue )
{
    switch ( nAttribute )
    {
    case SQL_ATTR_CURRENT_CATALOG:
    case SQL_ATTR_TRACEFILE:
    case SQL_ATTR_TRANSLATE_LIB:
        {
            SQLWCHAR    szValue[256];
            SQLINTEGER  nLength = 256;
            SQLINTEGER  nRetSize;
            SQLRETURN   nReturn = getConnectAttr( nAttribute, (SQLPOINTER)szValue, nLength, &nRetSize  );
            switch ( nReturn )
            {
                case SQL_SUCCESS:
                case SQL_SUCCESS_WITH_INFO:
                    // even a truncated string is supposed to be null term so ignore nRetSize and whether its Bytes (yes) or Chars (no)
                    *pValue = QString::fromUtf16( szValue );
                    break;
            }

            return nReturn;
        }
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::getConnectAttr( SQLINTEGER nAttribute, QByteArray *pd )
{
    switch ( nAttribute )
    {
        break; // no Attributes match data type
        {
            SQLINTEGER  nRetSize;

            return getConnectAttr( nAttribute, (SQLPOINTER)(pd->data()), pd->size(), &nRetSize );
        }
    }

    eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Data type incorrect for attribute.") ) );
    return SQL_ERROR;
}

SQLRETURN OQConnection::getConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue, SQLINTEGER nBufferLength, SQLINTEGER *pnStringLength )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLGetConnectAttrW( hHandle, nAttribute, pValue, nBufferLength, pnStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQConnection::doConnect( SQLWCHAR *pszDSN, SQLSMALLINT nLength1, SQLWCHAR *pszUID, SQLSMALLINT nLength2, SQLWCHAR *pszPWD, SQLSMALLINT nLength3 )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    // DO IT
    SQLRETURN nReturn = SQLConnectW( hHandle, pszDSN, nLength1, pszUID, nLength2, pszPWD, nLength3 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = true;
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    if ( isConnected() )
    {
        // cache it - perhaps not needed???
        stringDSN = QString::fromUtf16( pszDSN );
        stringUID = QString::fromUtf16( pszUID );
        stringPWD = QString::fromUtf16( pszPWD );
        emit signalConnected();
    }

    return nReturn;
}

SQLRETURN OQConnection::doDriverConnect( SQLHWND hWnd, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn, SQLWCHAR *pszOut, SQLSMALLINT nLengthOut, SQLSMALLINT *pnLengthOut, SQLUSMALLINT nPrompt )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLDriverConnectW( hHandle, hWnd, pszIn, nLengthIn, pszOut, nLengthOut, pnLengthOut, nPrompt );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            bConnected = true;
            break;
        case SQL_SUCCESS_WITH_INFO:
            bConnected = true;
            eventDiagnostic();
            break;
        case SQL_NEED_DATA:
            return nReturn;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    if ( isConnected() )
        emit signalConnected();

    return nReturn;
}

SQLRETURN OQConnection::doGetInfo( SQLUSMALLINT nInfoType, SQLPOINTER pInfoValue, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLGetInfoW( hHandle, nInfoType, pInfoValue, nBufferLength, pnStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_INVALID_HANDLE" ) ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit( __FUNCTION__ ), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}


