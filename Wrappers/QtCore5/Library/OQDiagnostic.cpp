/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \version 1
 * \date    2008
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#include "OQDiagnostic.h"
#include "OQHandle.h"

OQDiagnosticRecord::OQDiagnosticRecord( OQDiagnostic *pDiagnostic, SQLINTEGER nRecord )
{
    this->pDiagnostic   = pDiagnostic;
    this->nRecord       = nRecord;
}

QString OQDiagnosticRecord::getClassOrigin( SQLRETURN *pnReturn )
{
    SQLWCHAR *  psz;
    SQLSMALLINT nMaxBytes;
    SQLSMALLINT nRequiredBytes;
    SQLRETURN   nReturn;
    QString     s;

    // how many bytes do we need?OQMessage
    nReturn = getDiagField( ClassOrigin, NULL, 0, &nRequiredBytes );
    if ( !SQL_SUCCEEDED( nReturn ) )
        return s;
    // allocate for bytes..
    nMaxBytes = nRequiredBytes + sizeof(SQLWCHAR);
    psz = (SQLWCHAR*)malloc( nMaxBytes );
    // get the bytes...
    nReturn = getDiagField( ClassOrigin, psz, nMaxBytes, &nRequiredBytes );
    if ( SQL_SUCCEEDED( nReturn ) )
        s = QString::fromUtf16( psz );
    free( psz );

    if ( pnReturn ) *pnReturn = nReturn;

    return s;
}

QString OQDiagnosticRecord::getConnectionName( SQLRETURN *pnReturn )
{
    SQLWCHAR *  psz;
    SQLSMALLINT nMaxBytes;
    SQLSMALLINT nRequiredBytes;
    SQLRETURN   nReturn;
    QString     s;

    // how many bytes do we need?
    nReturn = getDiagField( ConnectionName, NULL, 0, &nRequiredBytes );
    if ( !SQL_SUCCEEDED( nReturn ) )
        return s;
    // allocate for bytes..
    nMaxBytes = nRequiredBytes + sizeof(SQLWCHAR);
    psz = (SQLWCHAR*)malloc( nMaxBytes );
    // get the bytes...
    nReturn = getDiagField( ConnectionName, psz, nMaxBytes, &nRequiredBytes );
    if ( SQL_SUCCEEDED( nReturn ) )
        s = QString::fromUtf16( psz );
    free( psz );

    if ( pnReturn ) *pnReturn = nReturn;

    return s;
}

QString OQDiagnosticRecord::getMessageText( SQLRETURN *pnReturn )
{
    SQLWCHAR *  psz;
    SQLSMALLINT nMaxBytes;
    SQLSMALLINT nRequiredBytes;
    SQLRETURN   nReturn;
    QString     s;

    // how many bytes do we need?
    nReturn = getDiagField( MessageText, NULL, 0, &nRequiredBytes );
    if ( !SQL_SUCCEEDED( nReturn ) )
        return s;
    // allocate for bytes..
    nMaxBytes = nRequiredBytes + sizeof(SQLWCHAR);
    psz = (SQLWCHAR*)malloc( nMaxBytes );
    // get the bytes...
    nReturn = getDiagField( MessageText, psz, nMaxBytes, &nRequiredBytes );
    if ( SQL_SUCCEEDED( nReturn ) )
        s = QString::fromUtf16( psz );
    free( psz );

    if ( pnReturn ) *pnReturn = nReturn;

    return s;
}

QString OQDiagnosticRecord::getServerName( SQLRETURN *pnReturn )
{
    SQLWCHAR *  psz;
    SQLSMALLINT nMaxBytes;
    SQLSMALLINT nRequiredBytes;
    SQLRETURN   nReturn;
    QString     s;

    // how many bytes do we need?
    nReturn = getDiagField( ServerName, NULL, 0, &nRequiredBytes );
    if ( !SQL_SUCCEEDED( nReturn ) )
        return s;
    // allocate for bytes..
    nMaxBytes = nRequiredBytes + sizeof(SQLWCHAR);
    psz = (SQLWCHAR*)malloc( nMaxBytes );
    // get the bytes...
    nReturn = getDiagField( ServerName, psz, nMaxBytes, &nRequiredBytes );
    if ( SQL_SUCCEEDED( nReturn ) )
        s = QString::fromUtf16( psz );
    free( psz );

    if ( pnReturn ) *pnReturn = nReturn;

    return s;
}

QString OQDiagnosticRecord::getSubclassOrigin( SQLRETURN *pnReturn )
{
    SQLWCHAR *  psz;
    SQLSMALLINT nMaxBytes;
    SQLSMALLINT nRequiredBytes;
    SQLRETURN   nReturn;
    QString     s;

    // how many bytes do we need?
    nReturn = getDiagField( SubclassOrigin, NULL, 0, &nRequiredBytes );
    if ( !SQL_SUCCEEDED( nReturn ) )
        return s;
    // allocate for bytes..
    nMaxBytes = nRequiredBytes + sizeof(SQLWCHAR);
    psz = (SQLWCHAR*)malloc( nMaxBytes );
    // get the bytes...
    nReturn = getDiagField( SubclassOrigin, psz, nMaxBytes, &nRequiredBytes );
    if ( SQL_SUCCEEDED( nReturn ) )
        s = QString::fromUtf16( psz );
    free( psz );

    if ( pnReturn ) *pnReturn = nReturn;

    return s;
}

SQLWCHAR *OQDiagnosticRecord::getClassOrigin( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( ClassOrigin, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLINTEGER OQDiagnosticRecord::getColumnNumber( SQLRETURN *pn )
{
    SQLINTEGER  n       = 0;
    SQLRETURN   nReturn = getDiagField( ColumnNumber, &n, SQL_IS_INTEGER, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLWCHAR *OQDiagnosticRecord::getConnectionName( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( ConnectionName, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLWCHAR *OQDiagnosticRecord::getMessageText( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( MessageText, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLINTEGER OQDiagnosticRecord::getNative( SQLRETURN *pn )
{
    SQLINTEGER  n       = 0;
    SQLRETURN   nReturn = getDiagField( Native, &n, SQL_IS_INTEGER, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLLEN OQDiagnosticRecord::getRowNumber( SQLRETURN *pn )
{
    SQLLEN      n       = 0;
    SQLRETURN   nReturn = getDiagField( RowNumber, &n, SQL_IS_INTEGER /* no SQL_IS_LEN! */, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLWCHAR *OQDiagnosticRecord::getServerName( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( ServerName, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLWCHAR *OQDiagnosticRecord::getSqlstate( SQLWCHAR *psz /* better be 5 chars */, SQLRETURN *pnReturn )
{
    if ( psz )
        memset( psz, 5 * sizeof(SQLWCHAR), '\0' );

    SQLRETURN nReturn = getDiagField( Sqlstate, psz, 5, NULL );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

QString OQDiagnosticRecord::getSqlstate( SQLRETURN *pnReturn )
{
    SQLWCHAR p[5];

    memset( p, 5 * sizeof(SQLWCHAR), '\0' );

    SQLRETURN nReturn = getDiagField( Sqlstate, p, 5, NULL );

    if ( pnReturn )
        *pnReturn = nReturn;

    return QString::fromUtf16( p );
}

SQLWCHAR *OQDiagnosticRecord::getSubclassOrigin( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( SubclassOrigin, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLRETURN OQDiagnosticRecord::getDiagField( Fields nField, SQLPOINTER pnDiagInfoPtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr )
{
    return getDiagField( (SQLSMALLINT)nField, pnDiagInfoPtr, nBufferLength, pnStringLengthPtr );
}

SQLRETURN OQDiagnosticRecord::getDiagField( SQLSMALLINT nDiagIdentifier, SQLPOINTER pnDiagInfoPtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr )
{
    if ( !pDiagnostic->pHandle->isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLGetDiagFieldW( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, nDiagIdentifier, pnDiagInfoPtr, nBufferLength, pnStringLengthPtr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_SUCCESS_WITH_INFO") ) );
            break;
        case SQL_ERROR:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_ERROR") ) );
            break;
        case SQL_INVALID_HANDLE:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

OQDiagnostic::OQDiagnostic( OQHandle *pHandle )
{
    this->pHandle = pHandle;
}

SQLLEN OQDiagnostic::getCursorRowCount( SQLRETURN *pn )
{
    SQLLEN      n       = 0;
    SQLRETURN   nReturn = getDiagField( CursorRowCount, &n, SQL_IS_INTEGER /* no SQL_IS_LEN! */, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLWCHAR *OQDiagnostic::getDynamicFunction( SQLWCHAR *psz, SQLSMALLINT nMaxBytes, SQLSMALLINT *pnRequiredBytes, SQLRETURN *pnReturn )
{
    if ( psz && nMaxBytes >= (SQLSMALLINT)sizeof(SQLWCHAR) )
        memset( psz, nMaxBytes, '\0' );

    SQLRETURN nReturn = getDiagField( DynamicFunction, psz, nMaxBytes, pnRequiredBytes );

    if ( pnReturn )
        *pnReturn = nReturn;

    return psz;
}

SQLINTEGER OQDiagnostic::getDynamicFunctionCode( SQLRETURN *pn )
{
    SQLINTEGER  n       = 0;
    SQLRETURN   nReturn = getDiagField( DynamicFunctionCode, &n, SQL_IS_INTEGER, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLINTEGER OQDiagnostic::getNumber( SQLRETURN *pn )
{
    SQLINTEGER  n       = 0;
    SQLRETURN   nReturn = getDiagField( Number, &n, SQL_IS_INTEGER, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLRETURN OQDiagnostic::getReturncode( SQLRETURN *pn )
{
    SQLRETURN   n       = SQL_SUCCESS;
    SQLRETURN   nReturn = getDiagField( Returncode, &n, SQL_IS_INTEGER /* no SQL_IS_RETURNCODE! */, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLLEN OQDiagnostic::getRowCount( SQLRETURN *pn )
{
    SQLLEN      n       = 0;
    SQLRETURN   nReturn = getDiagField( RowCount, &n, SQL_IS_INTEGER /* no SQL_IS_LEN! */, NULL );

    if ( pn )
        *pn = nReturn;

    return n;
}

SQLRETURN OQDiagnostic::getDiagField( Fields nField, SQLPOINTER pnDiagInfoPtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr )
{
    return getDiagField( (SQLSMALLINT)nField, pnDiagInfoPtr, nBufferLength, pnStringLengthPtr );
}

SQLRETURN OQDiagnostic::getDiagField( SQLSMALLINT nDiagIdentifier, SQLPOINTER pnDiagInfoPtr, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLengthPtr )
{
    if ( !pHandle->isAlloc() )
        return SQL_ERROR;

    SQLRETURN nReturn = SQLGetDiagFieldW( pHandle->getType(), pHandle->getHandle(), 0, nDiagIdentifier, pnDiagInfoPtr, nBufferLength, pnStringLengthPtr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_SUCCESS_WITH_INFO") ) );
            break;
        case SQL_ERROR:
            pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_ERROR") ) );
            break;
        case SQL_INVALID_HANDLE:
            pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            pHandle->eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}



