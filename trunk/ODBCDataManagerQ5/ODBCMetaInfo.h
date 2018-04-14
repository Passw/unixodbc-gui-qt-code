/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCMETAINFO_H
#define ODBCMETAINFO_H

#include <sqlext.h>
#include <odbcinstext.h>

#include <QtCore>

/*! 
 * \struct tODBCMetaInfoAttrValue 
 * \brief   Describes an attribute value. 
 *  
 *          A list of these can be used to present a list of potential values for an attribute.
 *          ie for a combo/list box
 */
typedef struct tODBCMetaInfoAttrValue
{
    union 
    {
        SQLUINTEGER nUnsignedInteger;
        SQLINTEGER  nInteger;
        SQLSMALLINT nSmallInteger;
    } Numeric;
    const char *pszFormalName;
    const char *pszCasualName;
    const char *pszDescription;

} ODBCMetaInfoAttrValue;

/*! 
 * \struct tODBCMetaInfoAttr 
 * \brief   Describes an attribute. 
 *  
 * \sa tODBCMetaInfoAttrValue 
 */
typedef struct tODBCMetaInfoAttr
{
    SQLINTEGER      nNumeric;
    const char *    pszFormalName;
    const char *    pszCasualName;
    const char *    pszDescription;
    int             nValueType;     // SQL_IS_INTEGER, etc
    bool            bReadOnly;
    ODBCMetaInfoAttrValue *pValues; // list of possible values

} ODBCMetaInfoAttr;

/*!
 * \class ODBCMetaInfo 
 * \brief Attribute information regarding an ODBC handle. 
 *  
 *          The idea here is that info about all ODBC handle attributes are stored in
 *          static data so that they can be processed using a smaller amount of code. 
 *  
 * \sa tODBCMetaInfoAttr
 *  
 * \author pharvey (4/11/18)
 */
class ODBCMetaInfo
{
public:
    enum enumAttrFields
    {
        AttrFieldNumeric,
        AttrFieldFormalName,
        AttrFieldCasualName,
        AttrFieldDescription
    };

    static ODBCMetaInfoAttr *       getSysAttr();                                                                   // to get env attr info
    static ODBCMetaInfoAttr *       getEnvAttr();                                                                   // to get env attr info
                                                                                                                    
    static ODBCMetaInfoAttr *       getAttr( ODBCMetaInfoAttr *pAttr, enumAttrFields nField, QVariant v );          // to lookup an attr from a given field/value to get related info (scans)
    static ODBCMetaInfoAttrValue *  getAttrValue( ODBCMetaInfoAttr *pAttr, enumAttrFields nField, QVariant v );     // to lookup an attr value from a given field/value to get related info  (scans)
    static QVector<QVariant>        getAttrValues( ODBCMetaInfoAttr *pAttr, enumAttrFields nField );                // to load picklists

    static int getCount( ODBCMetaInfoAttr *p );         // returns the number of attributes (scans)
    static int getCount( ODBCMetaInfoAttrValue *p );    // returns the number of attr values (scans)
};

#endif

