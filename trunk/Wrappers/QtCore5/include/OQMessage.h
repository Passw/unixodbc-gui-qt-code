/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-CPP Project 2003-2009, LGPL
 */
#ifndef ODBCMESSAGE_H
#define ODBCMESSAGE_H

#include "OQ.h"

/*! 
 * \brief   An ODBC message.
 * 
 *          An ODBC message is created by the class library and is provided to the application via
 *          the \sa eventMessage() method. This class compliments \sa OQDiagnostic (and the eventDiagnostic()
 *          method) by providing additional diagnostic information. For example; when a request fails due to 
 *          an invalid handle - in such a case no diagnostic information can be found using OQDiagnostic. 
 *
 * \sa      OQEnvironment
 *          OQConnection
 *          OQStatement
 */
class OQMessage
{
public:
    enum Types 
    { 
        Info = 0,   /*!< the application may ignore this message                    */
        Warning,    /*!< the application may want to pay attention to this message  */ 
        Error       /*!< the application should pay attention to this message       */ 
    };

    OQMessage( Types nType, const QString &stringRequest = QString::null, const QString &stringText = QString::null, SQLINTEGER nNumeric = 0 );
    OQMessage( const OQMessage &Message );
    ~OQMessage();

    Types           getType();
    QString         getRequest();
    QString         getText();
    SQLINTEGER      getNumeric();
    QString         getTypeText();

protected:
    Types       nType;          /*!< message type \sa Types                                */
    QString     stringRequest;  /*!< usually function ie SQLAllocHandle                    */
    QString     stringText;     /*!< message text                                          */
    SQLINTEGER  nNumeric;       /*!< numeric value dependent upon message                  */
};

#endif

