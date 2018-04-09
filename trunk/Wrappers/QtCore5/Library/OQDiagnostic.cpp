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

QString OQDiagnosticRecord::getClassOrigin( SQLRETURN *pn )
{
    return getDiagFieldString( ClassOrigin, pn );
}

SQLINTEGER OQDiagnosticRecord::getColumnNumber( SQLRETURN *pn )
{
    return getDiagFieldInteger( ColumnNumber, pn );
}

QString OQDiagnosticRecord::getConnectionName( SQLRETURN *pn )
{
    return getDiagFieldString( ConnectionName, pn );
}

QString OQDiagnosticRecord::getMessageText( SQLRETURN *pn )
{
    return getDiagFieldString( MessageText, pn );
}

SQLINTEGER OQDiagnosticRecord::getNative( SQLRETURN *pn )
{
    return getDiagFieldInteger( Native, pn );
}

SQLLEN OQDiagnosticRecord::getRowNumber( SQLRETURN *pn )
{
    return getDiagFieldLen( RowNumber, pn );
}

QString OQDiagnosticRecord::getServerName( SQLRETURN *pn )
{
    return getDiagFieldString( ServerName, pn );
}

QString OQDiagnosticRecord::getSqlstate( SQLRETURN *pn )
{
    return getDiagFieldString( Sqlstate, pn );
}

QString OQDiagnosticRecord::getSubclassOrigin( SQLRETURN *pn )
{
    return getDiagFieldString( SubclassOrigin, pn );
}

QString OQDiagnosticRecord::getDiagFieldString( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    QString     stringValue;
    SQLSMALLINT nBytesNeeded = 0;

    // sanity check
    if ( !pDiagnostic->pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return stringValue;
    }

    switch ( nField )
    {
        case ClassOrigin:
        case ConnectionName:
        case MessageText:
        case ServerName:
        case Sqlstate:
        case SubclassOrigin:
            break;
        default:
            *pnRet = SQL_ERROR;
            return stringValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, (SQLSMALLINT)nField, NULL, 0, &nBytesNeeded );
    if ( SQL_SUCCEEDED(*pnRet) && nBytesNeeded > 0 )
    {
        // create buffer
        SQLSMALLINT nBytesAvail = nBytesNeeded + sizeof(SQLWCHAR);
        SQLWCHAR szValue[nBytesAvail];

        memset( szValue, nBytesAvail, '\0' ); // being safe

        // get data
        *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, (SQLSMALLINT)nField, szValue, nBytesAvail, &nBytesNeeded );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                stringValue = QString::fromUtf16( szValue );
                break;
            case SQL_NO_DATA:
                break;
            case SQL_SUCCESS_WITH_INFO:
                // truncation? (should not happen but check anyway)
                if ( nBytesAvail <= nBytesNeeded )
                    stringValue = QString::fromUtf16( szValue, nBytesAvail / sizeof(SQLWCHAR) );
                pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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
    }

    return stringValue;
}

SQLINTEGER OQDiagnosticRecord::getDiagFieldInteger( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    SQLULEN     nValue = 0;

    // sanity check
    if ( !pDiagnostic->pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return (SQLINTEGER)nValue;
    }

    switch ( nField )
    {
        case ColumnNumber:
        case Native:
            break;
        default:
            *pnRet = SQL_ERROR;
            return (SQLINTEGER)nValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, (SQLSMALLINT)nField, &nValue, sizeof(SQLINTEGER), NULL );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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

    return (SQLINTEGER)nValue;
}

SQLLEN OQDiagnosticRecord::getDiagFieldLen( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    SQLULEN     nValue = 0;

    // sanity check
    if ( !pDiagnostic->pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return (SQLLEN)nValue;
    }

    switch ( nField )
    {
        case RowNumber:
            break;
        default:
            *pnRet = SQL_ERROR;
            return (SQLLEN)nValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pDiagnostic->pHandle->getType(), pDiagnostic->pHandle->getHandle(), nRecord, (SQLSMALLINT)nField, &nValue, sizeof(SQLLEN), NULL );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pDiagnostic->pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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

    return (SQLLEN)nValue;
}

OQDiagnostic::OQDiagnostic( OQHandle *pHandle )
{
    this->pHandle = pHandle;
}

SQLLEN OQDiagnostic::getCursorRowCount( SQLRETURN *pn )
{
    return getDiagFieldLen( CursorRowCount, pn );
}

QString OQDiagnostic::getDynamicFunction( SQLRETURN *pn )
{
    return getDiagFieldString( DynamicFunction, pn );
}

SQLINTEGER OQDiagnostic::getDynamicFunctionCode( SQLRETURN *pn )
{
    return getDiagFieldInteger( DynamicFunctionCode, pn );
}

SQLINTEGER OQDiagnostic::getNumber( SQLRETURN *pn )
{
    return getDiagFieldInteger( Number, pn );
}

SQLRETURN OQDiagnostic::getReturncode( SQLRETURN *pn )
{
    return getDiagFieldInteger( Returncode, pn );
}

SQLLEN OQDiagnostic::getRowCount( SQLRETURN *pn )
{
    return getDiagFieldLen( RowCount, pn );
}

OQDiagnosticRecord OQDiagnostic::getRecord( SQLINTEGER nRecord, SQLRETURN *pn )
{
    // could do some sanity checks here but...
    if ( pn )
        *pn = SQL_SUCCESS;

    return OQDiagnosticRecord( this, nRecord );
}

QString OQDiagnostic::getDiagFieldString( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    QString     stringValue;
    SQLSMALLINT nBytesNeeded = 0;

    // sanity check
    if ( !pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return stringValue;
    }

    switch ( nField )
    {
        case DynamicFunction:
            break;
        default:
            *pnRet = SQL_ERROR;
            return stringValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pHandle->getType(), pHandle->getHandle(), 0, (SQLSMALLINT)nField, NULL, 0, &nBytesNeeded );
    if ( SQL_SUCCEEDED(*pnRet) && nBytesNeeded > 0 )
    {
        // create buffer
        SQLSMALLINT nBytesAvail = nBytesNeeded + sizeof(SQLWCHAR);
        SQLWCHAR szValue[nBytesAvail];

        memset( szValue, nBytesAvail, '\0' ); // being safe

        // get data
        *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pHandle->getType(), pHandle->getHandle(), 0, (SQLSMALLINT)nField, szValue, nBytesAvail, &nBytesNeeded );
        switch ( nReturn )
        {
            case SQL_SUCCESS:
                stringValue = QString::fromUtf16( szValue );
                break;
            case SQL_NO_DATA:
                break;
            case SQL_SUCCESS_WITH_INFO:
                // truncation? (should not happen but check anyway)
                if ( nBytesAvail <= nBytesNeeded )
                    stringValue = QString::fromUtf16( szValue, nBytesAvail / sizeof(SQLWCHAR) );
                pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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
    }

    return stringValue;
}

SQLINTEGER OQDiagnostic::getDiagFieldInteger( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    SQLULEN     nValue = 0;

    // sanity check
    if ( !pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return (SQLINTEGER)nValue;
    }

    switch ( nField )
    {
        case DynamicFunctionCode:
        case Number:
        case Returncode:
            break;
        default:
            *pnRet = SQL_ERROR;
            return (SQLINTEGER)nValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pHandle->getType(), pHandle->getHandle(), 0, (SQLSMALLINT)nField, &nValue, sizeof(SQLINTEGER), NULL );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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

    return (SQLINTEGER)nValue;
}

SQLLEN OQDiagnostic::getDiagFieldLen( Fields nField, SQLRETURN *pnReturn )
{
    SQLRETURN   nReturn;
    SQLRETURN * pnRet = ( pnReturn ? pnReturn : &nReturn );
    SQLULEN     nValue = 0;

    // sanity check
    if ( !pHandle->isAlloc() )
    {
        *pnRet = SQL_ERROR;
        return (SQLLEN)nValue;
    }

    switch ( nField )
    {
        case CursorRowCount:
        case RowCount:
            break;
        default:
            *pnRet = SQL_ERROR;
            return (SQLLEN)nValue;
    }

    // get number of bytes needed
    *pnRet = SQLGetDiagFieldW( (SQLSMALLINT)pHandle->getType(), pHandle->getHandle(), 0, (SQLSMALLINT)nField, &nValue, sizeof(SQLLEN), NULL );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            pHandle->eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit( __FUNCTION__ ), QString::fromLocal8Bit( "SQL_SUCCESS_WITH_INFO" ) ) );
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

    return (SQLLEN)nValue;
}


