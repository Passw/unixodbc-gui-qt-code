/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#pragma once

#include "OQ.h"
#include "OQMessage.h"
#include "OQDiagnostic.h"

/*! 
 * \brief   An ODBC handle.
 *
 *          This is the base class for key classes in this library. Specifically;
 * 
 *          \li OQSystem
 *          \li OQEnvironment
 *          \li OQConnection
 *          \li OQStatement
 *
 * \sa  OQSystem
 *      OQEnvironment
 *      OQConnection
 *      OQStatement
 */
class OQHandle : public QObject
{
    Q_OBJECT
public:

    enum Types
    {
        Sys = 0,
        Env = SQL_HANDLE_ENV,
        Dbc = SQL_HANDLE_DBC,
        Stm = SQL_HANDLE_STMT
    };

    explicit OQHandle( Types nType, OQHandle *phandleParent = NULL );
    virtual ~OQHandle();

    void        setImplicitAlloc( bool b ) { bImplicitAlloc = b; }

    Types       getType();
    SQLHANDLE   getHandle();
    OQHandle *  getParent( Types nType );
    bool        getImplicitAlloc() { return bImplicitAlloc; }

    virtual SQLRETURN doAlloc();        
    virtual SQLRETURN doFree();         

    bool isAlloc( bool bImplicitAlloc );
    bool isAlloc();

    virtual void eventMessage( OQMessage Message );
    virtual void eventDiagnostic();

signals:
    void signalMessage( OQMessage );
    void signalDiagnostic( OQDiagnostic );

protected:
    Types       nType;
    SQLHANDLE   hHandle;
    bool        bImplicitAlloc = false;
};

