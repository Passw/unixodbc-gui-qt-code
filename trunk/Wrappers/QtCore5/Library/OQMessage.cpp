
/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#include "OQMessage.h"

/*! 
 * \brief   Constructs an OQMessage.
 *
 *          This copies and stores the given message information.
 *  
 * \param   nType           Input. One of OQMessage::MessageTypes.
 * \param   stringRequest  Input. Usually function ie "SQLAllocHandle".
 * \param   stringText     Input. Message text. Can be NULL - pref. not.
 * \param   nNumeric    Input. Numeric value dependent upon message.
 *
 */
OQMessage::OQMessage( Types nType, const QString &stringRequest, const QString &stringText, SQLINTEGER nNumeric )
{
    //
    this->nType = nType;

    //
    this->stringRequest = stringRequest;
    this->stringText = stringText;

    //
    this->nNumeric = nNumeric;
}

OQMessage::OQMessage( const OQMessage &Message )
{
    nType = Message.nType;
    stringRequest = Message.stringRequest;
    stringText = Message.stringText;
    nNumeric = Message.nNumeric;
}

OQMessage::~OQMessage()
{
}

/*! 
 * \brief   Message type.
 * 
 *          This can viewed as the message severity if you want.
 * 
 * \return  Types
 *
 * \sa      getTypeText
 */
OQMessage::Types OQMessage::getType() 
{ 
    return nType; 
}

/*! 
 * \brief   Message type text.
 * 
 *          This can be used to format a nicer message.
 * 
 * \return  const ODBCCHAR*
 *
 * \sa      getType
 */
QString OQMessage::getTypeText()
{
    switch( nType )
    {
        case Info:
            return QObject::tr("INFO");
        case Warning:
            return QObject::tr("WARNING");
        case Error:
            return QObject::tr("ERROR");
    }

    return QObject::tr("UNKNOWN");
}

/*! 
 * \brief   Message request.
 * 
 *          The request which generated this message. This is usually a function or method name.
 * 
 * \return  const ODBCCHAR*
 */
QString OQMessage::getRequest() 
{ 
    return stringRequest; 
}

/*! 
 * \brief   Message text.
 * 
 *          The message text. This can usually be presented, as is, but
 *          one may want to enhance it using other bits of info from 
 *          OQMessage.
 * 
 * \return  const ODBCCHAR*
 */
QString OQMessage::getText() 
{ 
    return stringText; 
}

/*! 
 * \brief   Numeric information.
 * 
 *          The value of this is dependent upon the message context.
 * 
 * \return  SQLINTEGER
 * \retval  -1          Usually (but not always) means null.
 */
SQLINTEGER OQMessage::getNumeric() 
{
    return nNumeric; 
}


