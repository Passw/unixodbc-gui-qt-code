/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#include "OQHandle.h"

OQHandle::OQHandle( Types nType, OQHandle *phandleParent )
 : QObject( phandleParent )
{
    this->nType         = nType;
    hHandle             = SQL_NULL_HANDLE;

    /* Echo up the object hierarchy (unless we are top handle). The idea here is that
     * signalMessage() and signalDiagnostic() can be connected to ie a message/output 
     * widget from any handle - and it will get all messages for that handle and its
     * children. So, typically, a message/output widget would be connected to the Sys
     * handle and get messages for everything ODBC.
     */
    if ( phandleParent && nType != Sys )
    {
        connect( this, SIGNAL(signalMessage(OQMessage)), phandleParent, SIGNAL(signalMessage(OQMessage)) );
        connect( this, SIGNAL(signalDiagnostic(OQDiagnostic)), phandleParent, SIGNAL(signalDiagnostic(OQDiagnostic)) );
    }
}

OQHandle::~OQHandle()
{
    if ( isAlloc( false ) )
        doFree();
}

OQHandle::Types OQHandle::getType()
{
    return nType;
}

SQLHANDLE OQHandle::getHandle()
{
    return hHandle;
}

OQHandle *OQHandle::getParent( Types nType )
{
    if ( getType() == nType )
        return this;

    /* The Sys type *could* have a QObject based parent but it would not be a 
     * OQHandle so we ignore it. For our purposes the Sys type is the top level.
     * A call to parent() will get the parent QObject if exists and is needed.
     */
    if ( getType() == Sys )
        return NULL;

    return ((OQHandle *)parent())->getParent( nType );
}

/*! 
 * \brief   Allocate underlying ODBC handle.
 *
 *          This method is not usually called explicitly from an application since 
 *          it will be silently called via \sa isAlloc as needed.
 *
 *          This method is public to handle any exceptional cases such as when
 *          an app wants to regularly \sa doAlloc and \sa doFree to handle unstable
 *          drivers.
 * 
 * \return  SQLRETURN
 *
 * \sa      doFree
 *          isAlloc
 */
SQLRETURN OQHandle::doAlloc()
{
    if ( isAlloc( false ) )
    {
        eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Already allocated.") ) );
        return SQL_ERROR;
    }

    // Sys type is a special case
    // we do not allocate an ODBC handle - we fake it to make other logic work
    if ( getType() == Sys )
    {
        hHandle = (void*)1;
        return SQL_SUCCESS;
    }

    // we have ruled out the Sys handle above - for all others - our parent must exist and be a OQHandle
    OQHandle *pParent = (OQHandle*)(parent());
    if ( !pParent )
        return SQL_ERROR;

    // parent ODBC handle for Env is always null
    SQLHANDLE hParent = ( nType == Env ? SQL_NULL_HANDLE : pParent->getHandle() );

    SQLRETURN nReturn = SQLAllocHandle( nType, hParent, &hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_ERROR") ) );
            hHandle = NULL;
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            hHandle = NULL;
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            hHandle = NULL;
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Free underlying ODBC handle.
 *
 *          Frees resources used to maintain the underlying ODBC handle. Upon success - this will
 *          invalidate any child handles but will not notify them! Errors will be caught but the 
 *          proper way to free a handle is to first ensure that any/all child handles have been
 *          freed.
 *
 *          This method will be silently called in the destructor as needed.
 *
 *          This method is public to handle any exceptional cases such as when
 *          an app wants to regularly \sa doAlloc and \sa doFree to handle unstable
 *          drivers.
 * 
 * \return  SQLRETURN
 *
 * \sa      doAlloc
 *          isAlloc
 */
SQLRETURN OQHandle::doFree()
{
    if ( !isAlloc( false ) )
    {
        eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Already free.") ) );
        return SQL_ERROR;
    }

    // Sys type is a special case
    // we do not allocate (or free) an ODBC handle - we fake it to make other logic work
    if ( getType() == Sys )
    {
        hHandle = SQL_NULL_HANDLE;
        return SQL_SUCCESS;
    }

    SQLRETURN nReturn = SQLFreeHandle( nType, hHandle );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            hHandle = NULL;
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventMessage( OQMessage( OQMessage::Warning, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_SUCCESS_WITH_INFO") ) );
            hHandle = NULL;
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Check if the underlying ODBC handle has been allocated.
 *
 *          The underlying ODBC handle will be allocated, as needed, if ImplicitAlloc is true.
 * 
 * \sa      setImplicitAlloc()
 * \sa      getImplicitAlloc()
 * \sa      isAlloc( bool )
 * 
 * \return  bool
 * \retval  false   Handle was not allocated AND we failed to allocate it.
 * \retval  true    Handle was either allocated OR we succeeded in doing so.
 *
 * \sa      doAlloc
 *          doFree
 */
bool OQHandle::isAlloc()
{
    if ( hHandle == SQL_NULL_HANDLE )
    {
        if ( !getImplicitAlloc() )
            return false;
        // alloc
        SQLRETURN nReturn = doAlloc();
        if ( !SQL_SUCCEEDED( nReturn ) )
            return false;
    }

    return true;
}

/*! 
 * \brief   Check if the underlying ODBC handle has been allocated.
 *
 *          This works like \sa isAlloc() but in this case we override the ImplicitAlloc setting
 *          set by the application with a value we pass during the call.
 * 
 *          Mostly this is used when we *never* want to implicitly allocate the handle.
 * 
 * \param   bImplicitAlloc  True if we want to silently allocate it as needed.
 *
 * \sa      setImplicitAlloc()
 * \sa      getImplicitAlloc()
 * \sa      isAlloc()
 * 
 * \return  bool
 * \retval  false   Handle was not allocated AND we failed to allocate it.
 * \retval  true    Handle was either allocated OR we succeeded in doing so (if implicit alloc true).
 *
 * \sa      doAlloc
 *          doFree
 */
bool OQHandle::isAlloc( bool bImplicitAlloc )
{
    if ( hHandle == SQL_NULL_HANDLE )
    {
        if ( !bImplicitAlloc )
            return false;
        // alloc
        SQLRETURN nReturn = doAlloc();
        if ( !SQL_SUCCEEDED( nReturn ) )
            return false;
    }

    return true;
}

void OQHandle::eventMessage( OQMessage m )
{
    emit signalMessage( m );
}

void OQHandle::eventDiagnostic()
{
    // this should not happen but just in case...
    if ( getType() == Sys )
        return;

    // 
    emit signalDiagnostic(OQDiagnostic( this ));
}


