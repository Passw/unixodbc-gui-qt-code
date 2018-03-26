/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#include <QCoreApplication>
#include "OQConnection.h"
#include "OQStatement.h"
#include <unistd.h>

OQStatement::OQStatement( OQConnection *pconnection )
    : OQHandle( Stm, pconnection )
{
    setObjectName( QString::fromLocal8Bit("OQStatement") );

    nElapsedSeconds = 0;

    // echo up the object hierarchy
    connect( this, SIGNAL(signalMessage(OQMessage)), pconnection, SIGNAL(signalMessage(OQMessage)) );
    connect( this, SIGNAL(signalDiagnostic(OQDiagnostic)), pconnection, SIGNAL(signalDiagnostic(OQDiagnostic)) );
}

OQStatement::~OQStatement()
{
}

SQLRETURN OQStatement::setAttrAsyncEnable( AttrAsyncEnableTypes n )
{
    return setStmtAttr( SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER)n );
}

SQLRETURN OQStatement::setAttrConcurrency( AttrConcurrencyTypes n )
{
    return setStmtAttr( SQL_ATTR_CONCURRENCY, (SQLPOINTER)n );
}

SQLRETURN OQStatement::setAttrMaxLength( SQLUINTEGER n )
{
    return setStmtAttr( SQL_ATTR_MAX_LENGTH, (SQLPOINTER)n );
}

SQLRETURN OQStatement::getAttrAsyncEnable( AttrAsyncEnableTypes *pn )
{
    return getStmtAttr( SQL_ATTR_ASYNC_ENABLE, pn );
}

SQLRETURN OQStatement::getAttrConcurrency( AttrConcurrencyTypes *pn )
{
    return getStmtAttr( SQL_ATTR_CONCURRENCY, pn );
}
/*
 * re. Char Data
 *
 * There is an opportunity for confusion here. The specification says that calling
 * with SQL_C_WCHAR will return wchar_t string.  
 *
 * We assume that the Driver is using the default for unixODBC (UTF16) and is 
 * applying that to wchar data.
 */
QString OQStatement::getData( SQLUSMALLINT nColumnNumber, SQLRETURN *pnReturn )
{
    QString     s;
    SQLWCHAR    szValue[1024];
    SQLSMALLINT nType           = SQL_C_WCHAR;
    SQLLEN      nChars          = 1024;
    SQLLEN      nBytes          = nChars * sizeof(SQLWCHAR);
    SQLLEN      nBytesAvailable = 0;

    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );

    if ( !isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return s;
    }

    // do it
    *pnRet = SQL_STILL_EXECUTING;
    while ( *pnRet == SQL_STILL_EXECUTING )
    {
        *pnRet = SQLGetData( getHandle(), nColumnNumber, nType, szValue, nBytes, &nBytesAvailable );
        switch ( *pnRet )
        {
            case SQL_SUCCESS:
                s = QString::fromUtf16( szValue );
                break;
            case SQL_SUCCESS_WITH_INFO:
                {
                    // could get here for various reasons - check to see if its likley because of truncation
                    if ( nBytesAvailable > nChars )
                        s = QString::fromUtf16( szValue, nChars );
                    eventDiagnostic();
                }
            break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    *pnRet = doCancel();
                    if ( SQL_SUCCEEDED( *pnRet ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
                return s;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    return s;
}

double OQStatement::getElapsedSeconds() 
{
    return nElapsedSeconds; 
}

/*
 * Not too much we can do here to simplify this call.
 */
SQLRETURN OQStatement::doBindCol( SQLUSMALLINT nColumnNumber, SQLSMALLINT nTargetType, SQLPOINTER pTargetValue, SQLLEN nBufferLength, SQLLEN *pnStrLenOrInd )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLBindCol( getHandle(), nColumnNumber, nTargetType, pTargetValue, nBufferLength, pnStrLenOrInd );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQStatement::doCancel()
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLCancel( hHandle );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQStatement::doCatalogs( const QString &stringCatalogName )
{
    return doTables( stringCatalogName );
}

SQLRETURN OQStatement::doCloseCursor()
{
    if ( !isAlloc() )
        return SQL_ERROR;

    // DO IT
    SQLRETURN nReturn;

    nReturn = SQLCloseCursor( getHandle() );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQStatement::doColAttribute( SQLUSMALLINT nColumnNumber, SQLUSMALLINT nFieldIdentifier, SQLPOINTER nCharacterAttributePtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr, SQLLEN *pnNumericAttributePtr )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() )
        return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLColAttribute( getHandle(), nColumnNumber, nFieldIdentifier, nCharacterAttributePtr, nBufferLength, pnStringLengthPtr, pnNumericAttributePtr );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    return nReturn;
}

SQLRETURN OQStatement::doColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringColumn )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*)( stringTable.isNull() ? 0 : stringTable.utf16() );
    SQLWCHAR *pszColumn  = (SQLWCHAR*)( stringColumn.isNull() ? 0 : stringColumn.utf16() );

    int         nCatalog   = pszCatalog ? SQL_NTS : 0;
    int         nSchema    = pszSchema ? SQL_NTS : 0; 
    int         nTable     = pszTable ? SQL_NTS : 0;
    int         nColumn    = pszColumn ? SQL_NTS : 0; 

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLColumnsW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable, pszColumn, nColumn );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    nReturn = doCancel();
                    if ( SQL_SUCCEEDED( nReturn ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doExecute()
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLExecute( getHandle() );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NEED_DATA:
            case SQL_NO_DATA:
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    } // while

    if ( SQL_SUCCEEDED( nReturn ) && isCursor() )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doExecDirect( const QString &stringStatement )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLExecDirectW( getHandle(), (SQLWCHAR*)stringStatement.utf16(), SQL_NTS );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
            case SQL_NEED_DATA:
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doFetch()
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() )
        return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLFetch( getHandle() );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_NO_DATA:
                return nReturn;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    return nReturn;
}

SQLRETURN OQStatement::doForeignKeys( const QString &stringPKCatalogName, const QString &stringPKSchemaName, const QString &stringPKTableName, const QString &stringFKCatalogName, const QString &stringFKSchemaName, const QString &stringFKTableName )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszPKCatalogName = (SQLWCHAR*)(stringPKCatalogName.isNull() ? 0 : stringPKCatalogName.utf16() );
    SQLWCHAR *pszPKSchemaName  = (SQLWCHAR*)(stringPKSchemaName.isNull() ? 0 : stringPKSchemaName.utf16() );
    SQLWCHAR *pszPKTableName   = (SQLWCHAR*)(stringPKTableName.isNull() ? 0 : stringPKTableName.utf16() );

    SQLWCHAR *pszFKCatalogName = (SQLWCHAR*)(stringFKCatalogName.isNull() ? 0 : stringFKCatalogName.utf16() );
    SQLWCHAR *pszFKSchemaName  = (SQLWCHAR*)(stringFKSchemaName.isNull() ? 0 : stringFKSchemaName.utf16() );
    SQLWCHAR *pszFKTableName   = (SQLWCHAR*)(stringFKTableName.isNull() ? 0 : stringFKTableName.utf16() );

    int nPKCatalogName = pszPKCatalogName ? SQL_NTS : 0;
    int nPKSchemaName  = pszPKSchemaName ? SQL_NTS : 0;
    int nPKTableName   = pszPKTableName ? SQL_NTS : 0;

    int nFKCatalogName = pszFKCatalogName ? SQL_NTS : 0;
    int nFKSchemaName  = pszFKSchemaName ? SQL_NTS : 0;
    int nFKTableName   = pszFKTableName ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLForeignKeysW( getHandle(), pszPKCatalogName, nPKCatalogName, pszPKSchemaName, nPKSchemaName, pszPKTableName, nPKTableName, pszFKCatalogName, nFKCatalogName, pszFKSchemaName, nFKSchemaName, pszFKTableName, nFKTableName );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doNumResultCols( SQLSMALLINT *pnColumnCountPtr )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !pnColumnCountPtr ) 
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Invalid argument.") ) );
        return SQL_ERROR;
    }

    *pnColumnCountPtr = 0;

    if (!isAlloc())
    {
        eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Handle not allocated.") ) );
        return SQL_ERROR;
    }

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLNumResultCols( getHandle(), pnColumnCountPtr );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    return nReturn;
}

SQLRETURN OQStatement::doPrepare( const QString &stringStatementText )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    
    if ( !isAlloc() )
        return SQL_ERROR;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLPrepareW( getHandle(), (SQLWCHAR*)stringStatementText.utf16(), SQL_NTS );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    return nReturn;
}

SQLRETURN OQStatement::doPrimaryKeys( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*)( stringTable.isNull() ? 0 : stringTable.utf16() );
    int         nCatalog   = pszCatalog ? SQL_NTS : 0;
    int         nSchema    = pszSchema ? SQL_NTS : 0; 
    int         nTable     = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLPrimaryKeysW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doProcedureColumns( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure, const QString &stringColumn )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *  pszCatalog      = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *  pszSchema       = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *  pszProcedure    = (SQLWCHAR*)( stringProcedure.isNull() ? 0 : stringProcedure.utf16() );
    SQLWCHAR *  pszColumn       = (SQLWCHAR*)( stringColumn.isNull() ? 0 : stringColumn.utf16() );
    int         nCatalog       = pszCatalog ? SQL_NTS : 0;
    int         nSchema        = pszSchema ? SQL_NTS : 0; 
    int         nProcedure     = pszProcedure ? SQL_NTS : 0;
    int         nColumn        = pszColumn ? SQL_NTS : 0; 

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLProcedureColumnsW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszProcedure, nProcedure, pszColumn, nColumn );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doProcedures( const QString &stringCatalog, const QString &stringSchema, const QString &stringProcedure )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *  pszCatalog      = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *  pszSchema       = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *  pszProcedure    = (SQLWCHAR*)( stringProcedure.isNull() ? 0 : stringProcedure.utf16() );
    int         nCatalog        = pszCatalog ? SQL_NTS : 0;
    int         nSchema         = pszSchema ? SQL_NTS : 0; 
    int         nProcedure      = pszProcedure ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLProceduresW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszProcedure, nProcedure );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doRowCount( SQLLEN *pnRowCountPtr )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLRowCount( getHandle(), pnRowCountPtr );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQStatement::doSchemas( const QString &stringCatalog, const QString &stringSchema )
{
    return doTables( stringCatalog, stringSchema );
}

SQLRETURN OQStatement::doSpecialColumns( SQLSMALLINT nIdentifierType, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nScope, SQLSMALLINT nNullable )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*)( stringTable.isNull() ? 0 : stringTable.utf16() );
    int         nCatalog        = pszCatalog ? SQL_NTS : 0;
    int         nSchema         = pszSchema ? SQL_NTS : 0; 
    int         nTable          = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLSpecialColumnsW( getHandle(), nIdentifierType, pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable, nScope, nNullable );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doStatistics( const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLUSMALLINT nUnique, SQLUSMALLINT nReserved )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalog = (SQLWCHAR*)( stringCatalog.isNull() ? 0 : stringCatalog.utf16() );
    SQLWCHAR *pszSchema  = (SQLWCHAR*)( stringSchema.isNull() ? 0 : stringSchema.utf16() );
    SQLWCHAR *pszTable   = (SQLWCHAR*)( stringTable.isNull() ? 0 : stringTable.utf16() );
    int         nCatalog   = pszCatalog ? SQL_NTS : 0;
    int         nSchema    = pszSchema ? SQL_NTS : 0; 
    int         nTable     = pszTable ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLStatisticsW( getHandle(), pszCatalog, nCatalog, pszSchema, nSchema, pszTable, nTable, nUnique, nReserved );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doTables( const QString &stringCatalogName, const QString &stringSchemaName, const QString &stringTableName, const QString &stringTableType )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    SQLWCHAR *pszCatalogName = (SQLWCHAR*)( stringCatalogName.isNull() ? 0 : stringCatalogName.utf16() );
    SQLWCHAR *pszSchemaName  = (SQLWCHAR*)( stringSchemaName.isNull() ? 0 : stringSchemaName.utf16() );
    SQLWCHAR *pszTableName   = (SQLWCHAR*)( stringTableName.isNull() ? 0 : stringTableName.utf16() );
    SQLWCHAR *pszTableType   = (SQLWCHAR*)( stringTableType.isNull() ? 0 : stringTableType.utf16() );

    int nCatalogNameLength   = pszCatalogName ? SQL_NTS : 0;
    int nSchemaNameLength    = pszSchemaName ? SQL_NTS : 0; 
    int nTableNameLength     = pszTableName ? SQL_NTS : 0;
    int nTableTypeLength     = pszTableType ? SQL_NTS : 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLTablesW( getHandle(), pszCatalogName, nCatalogNameLength, pszSchemaName, nSchemaNameLength, pszTableName, nTableNameLength, pszTableType, nTableTypeLength );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    nReturn = doCancel();
                    if ( SQL_SUCCEEDED( nReturn ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }
    }

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

SQLRETURN OQStatement::doTypeInfo( SQLSMALLINT nDataType )
{
    bool        bCancelled  = false;
    SQLRETURN   nReturn;
    time_t      timeStart;

    if ( !isAlloc() )
        return SQL_ERROR;

    time( &timeStart );
    nElapsedSeconds = 0;

    // do it
    nReturn = SQL_STILL_EXECUTING;
    while ( nReturn == SQL_STILL_EXECUTING )
    {
        nReturn = SQLGetTypeInfo( getHandle(), nDataType );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                break;
            case SQL_SUCCESS_WITH_INFO:
                nElapsedSeconds = difftime( time( NULL ), timeStart );
                eventDiagnostic();
                break;
            case SQL_STILL_EXECUTING:
                if ( !bCancelled && !doWaiting() )
                {
                    if ( SQL_SUCCEEDED( doCancel() ) )
                        bCancelled = true;  // Cancelled! We still need to loop to give the driver time to cleanup.
                }
                break;
            case SQL_ERROR:
                eventDiagnostic();
                break;
            case SQL_INVALID_HANDLE:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
                break;
            default:
                eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
                break;
        }

    } // while

    if ( SQL_SUCCEEDED( nReturn ) )
    {
        emit signalElapsedSeconds( nElapsedSeconds );
        emit signalResults( this );
    }

    return nReturn;
}

/*!
    getTables
    
    Query based upon a filter where filter may be of the form;
    
    [[CATALOG.]SCHEMA]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doTables( const OQFilter &stringFilter, const QString &stringTableType )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringDummy;

    getParsedFilter( stringFilter, &stringDummy, &stringCatalog, &stringSchema );

    return doTables( stringCatalog, stringSchema, QString::null, stringTableType );
}

/*!
    doColumns
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]TABLE | VIEW]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doColumns( const OQFilter &stringFilter )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringTable );

    return doColumns( stringCatalog, stringSchema, stringTable );
}

/*!
    doStatistics
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]TABLE]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doStatistics( const OQFilter &stringFilter, SQLUSMALLINT nUnique, SQLUSMALLINT nReserved )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringTable );

    return doStatistics( stringCatalog, stringSchema, stringTable, nUnique, nReserved );
}

/*!
    doSpecialColumns
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]TABLE]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doSpecialColumns( const OQFilter &stringFilter, SQLSMALLINT nIdentifierType, SQLSMALLINT nScope, SQLSMALLINT nNullable )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringTable );

    return doSpecialColumns( nIdentifierType, stringCatalog, stringSchema, stringTable, nScope, nNullable );
}

/*!
    doPrimaryKeys
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]TABLE]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doPrimaryKeys( const OQFilter &stringFilter )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringTable );

    return doPrimaryKeys( stringCatalog, stringSchema, stringTable );
}

/*!
    doForeignKeys
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]TABLE]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doForeignKeys( const OQFilter &stringFilter )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringTable );

    return doForeignKeys( stringCatalog, stringSchema, stringTable );
}

/*!
    doProcedures
    
    Query based upon a filter where filter may be of the form;
    
    [[CATALOG.]SCHEMA]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doProcedures( const OQFilter &stringFilter )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringDummy;

    getParsedFilter( stringFilter, &stringDummy, &stringCatalog, &stringSchema );

    return doProcedures( stringCatalog, stringSchema );
}

/*!
    doProcedureColumns
    
    Query based upon a filter where filter may be of the form;
    
    [[[CATALOG.]SCHEMA.]PROCEDURE]
    
    The filter is expanded and the query is executed.
*/
SQLRETURN OQStatement::doProcedureColumns( const OQFilter &stringFilter )
{
    QString stringCatalog;
    QString stringSchema;
    QString stringProcedure;

    getParsedFilter( stringFilter, &stringCatalog, &stringSchema, &stringProcedure );

    return doProcedureColumns( stringCatalog, stringSchema, stringProcedure );
}

/*!
    doWaiting
    
    This will give time to the main Qt event loop during any asynch operation such
    that the application gets some time to do things like; repaint the screen or update
    a progress dialog.
*/
bool OQStatement::doWaiting()
{
    qApp->processEvents();
    // i want to know if a driver actually supports asynch these days...
    printf( "[PAH][%s][%d] Looks like asynch option supported by this driver.\n", __FILE__, __LINE__ );

    // just slow the polling down a bit
#ifdef _WINDOWS_
    Sleep( 1000 );
#else
    sleep( 1000 ); 
#endif
    return true;
}

bool OQStatement::isCursor()
{
    SQLSMALLINT nColumns    = -1;
    SQLRETURN   nReturn     = doNumResultCols( &nColumns );

    if ( SQL_SUCCEEDED( nReturn ) && nColumns > 0 )
        return true;

    return false;
}

SQLRETURN OQStatement::slotExecDirect( const QString &stringStatement )
{
    return doExecDirect( stringStatement );
}

SQLRETURN OQStatement::slotExecute()
{
    return doExecute();
}

SQLRETURN OQStatement::setStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLSetStmtAttr( getHandle(), nAttribute, pValue, 0 );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

SQLRETURN OQStatement::getStmtAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLGetStmtAttr( getHandle(), nAttribute, pValue, 0, NULL );
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
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

/*!
    getParsedFilter

    Will seperate a stringFilter which is expected to be of the form;
    
    [[[CATALOG.]SCHEMA.]OBJECT]
*/
void OQStatement::getParsedFilter(  const OQFilter &stringFilter, QString *pstringCatalog, QString *pstringSchema, QString *pstringObject )
{
    *pstringCatalog = QString::null;
    *pstringSchema  = QString::null;
    *pstringObject  = QString::null;

    QStringList stringlist = stringFilter.split( QLatin1Char('.') );
    if ( stringlist.count() > 0 )
        *pstringObject = stringlist[stringlist.count() - 1];

    if ( stringlist.count() > 1 )
        *pstringSchema = stringlist[stringlist.count() - 2];

    if ( stringlist.count() > 2 )
        *pstringCatalog = stringlist[stringlist.count() - 3];
}


