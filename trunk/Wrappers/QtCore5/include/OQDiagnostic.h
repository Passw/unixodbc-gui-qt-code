/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \version 1
 * \date    2008
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#pragma once

#include "OQ.h"

class OQHandle;

class OQDiagnostic;

/*! 
 * \class   OQDiagnosticRecord
 * \brief   An ODBC diagnostic record.
 *
 *          This class is an interface to the ODBC diagnostic record information. The data is 'lazy loaded'
 *          or in other words - loaded only as requested. This means that this class should not be
 *          used if the parent \sa OQDiagnostic becomes invalid.
 *
 * \sa      OQDiagnostic
 */
class OQDiagnosticRecord
{
public:
    enum Fields
    {
        ClassOrigin     = SQL_DIAG_CLASS_ORIGIN,
        ColumnNumber    = SQL_DIAG_COLUMN_NUMBER,
        ConnectionName  = SQL_DIAG_CONNECTION_NAME,
        MessageText     = SQL_DIAG_MESSAGE_TEXT,
        Native          = SQL_DIAG_NATIVE,
        RowNumber       = SQL_DIAG_ROW_NUMBER,
        ServerName      = SQL_DIAG_SERVER_NAME,
        Sqlstate        = SQL_DIAG_SQLSTATE,
        SubclassOrigin  = SQL_DIAG_SUBCLASS_ORIGIN
    };

    explicit OQDiagnosticRecord( OQDiagnostic *pDiagnostic, SQLINTEGER nRecord );

    QString     getClassOrigin( SQLRETURN *pnReturn = NULL );
    SQLINTEGER  getColumnNumber( SQLRETURN *pn = NULL );
    QString     getConnectionName( SQLRETURN *pnReturn = NULL );
    QString     getMessageText( SQLRETURN *pnReturn = NULL );
    SQLINTEGER  getNative( SQLRETURN *pn = NULL );
    SQLLEN      getRowNumber( SQLRETURN *pn = NULL );
    QString     getServerName( SQLRETURN *pnReturn = NULL );
    QString     getSqlstate( SQLRETURN *pnReturn = NULL );
    QString     getSubclassOrigin( SQLRETURN *pnReturn = NULL );

protected:
    OQDiagnostic *  pDiagnostic;
    SQLINTEGER      nRecord;

    QString     getDiagFieldString( Fields nField, SQLRETURN *pnReturn = NULL );
    SQLINTEGER  getDiagFieldInteger( Fields nField, SQLRETURN *pnReturn = NULL );
    SQLLEN      getDiagFieldLen( Fields nField, SQLRETURN *pnReturn = NULL );
};

/*! 
 * \class   OQDiagnostic
 * \brief   ODBC diagnostic information.
 *
 *          This class is an interface to the ODBC diagnostic information usually requested upon
 *          receiving a SQL_ERROR or SQL_SUCCESS_WITH_INFO return code. The data is 'lazy loaded'
 *          or in other words - loaded only as requested. This means that this class should not be
 *          used if the parent (Environemment/Connection/Statement) becomes invalid.
 * 
 * \sa      OQDiagnosticRecord
 * 
 * \author  pharvey (8/15/2008)
 */
class OQDiagnostic
{
    friend class OQDiagnosticRecord;
public:
    enum Fields
    {
        CursorRowCount      = SQL_DIAG_CURSOR_ROW_COUNT,
        DynamicFunction     = SQL_DIAG_DYNAMIC_FUNCTION,
        DynamicFunctionCode = SQL_DIAG_DYNAMIC_FUNCTION_CODE,
        Number              = SQL_DIAG_NUMBER,
        Returncode          = SQL_DIAG_RETURNCODE,
        RowCount            = SQL_DIAG_ROW_COUNT
    };

    explicit OQDiagnostic( OQHandle *pHandle );

    SQLLEN      getCursorRowCount( SQLRETURN *pn = NULL );
    QString     getDynamicFunction( SQLRETURN *pnReturn = NULL );
    SQLINTEGER  getDynamicFunctionCode( SQLRETURN *pn = NULL );
    SQLINTEGER  getNumber( SQLRETURN *pn = NULL );
    SQLRETURN   getReturncode( SQLRETURN *pn = NULL );
    SQLLEN      getRowCount( SQLRETURN *pn = NULL );

    OQDiagnosticRecord getRecord( SQLINTEGER nRecord, SQLRETURN *pn = NULL );

protected:
    OQHandle *pHandle;

    QString     getDiagFieldString( Fields nField, SQLRETURN *pnReturn = NULL );
    SQLINTEGER  getDiagFieldInteger( Fields nField, SQLRETURN *pnReturn = NULL );
    SQLLEN      getDiagFieldLen( Fields nField, SQLRETURN *pnReturn = NULL );
};


