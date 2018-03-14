/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 1
 * \date    2018
 * \license Copyright unixODBC-GUI-Qt Project 2003-2018, LGPL
 */
#ifndef OQ_H
#define OQ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Characters/Strings 
 *  
 * Microsoft 
 *  
 * The platform wide-char is 16bit and encoded for UTF-16 (variable width char). 
 * ODBC assumes wide-char is 16bit and encoded for UTF-16. 
 * We can build for 8bit char (legacy) or define UNICODE to build for wide-char (default).
 * SQLTCHAR maps to platform char or wchar_t and things pretty much just work.
 *  
 * UNIX/Linux 
 *  
 * The platform wide-char is often 32bit and encoded for UCS-4 (fixed width char) but can be 16bit and encoded for UTF-16. 
 * unixODBC assumes wide-char is 16bit and encoded for UTF-16 (by default). Drivers *should* do the same.
 * Applications can build for 8bit char (legacy) or define UNICODE to build for wide-char.
 * SQLTCHAR (for wide-char on unixODBC) does not necessarily map to platform wchar_t - it always maps to 16bit. unixODBC 
 * can be built to map to platform wchar_t by defining SQL_WCHART_CONVERT but this is never done for standard binary 
 * distributions of unixODBC. 
 *  
 * Apps/Driver Manager/Driver/Database
 *  
 * The Driver Manager (DM) can work with an App and Driver of any combination of 8Bit ASCII and 16bit (UTF-16). The DM will
 * try to convert strings as needed. This will lead to character loss when 16bits is needed to hold a char that is 
 * destined for a 8Bit space. If the DM detects SQLConnectW (or other wide-char) function in Driver it will assume it
 * supports wide-char calls. The DM will prefer this over 8bit char calls. When needed - it will also assume UTF-16 encoding. 
 *
 * The char data in the database is subject to its configured character-set and collation etc. The DM will not get involved 
 * in this.... its left to the App and the Driver to understand the database configuration.
 *
 * So its best if the App and the Driver are built with UNICODE and use 16bit char with UTF-16 encoding.
 *  
 * Qt 
 *  
 * QString stores a string of 16bit QChars, where each QChar corresponds one Unicode 4.0 character. 
 * Unicode characters with code values above 65535 are stored using surrogate pairs, i.e., two consecutive QChars. 
 * QString converts the const char * data into Unicode using the fromUtf8() function.
 *
 * Some Qt string operations and QChar itself seem restricted to fixed-width usage even if variable-width is supported ?? 
 *  
 * We use QString for convenience functions but also so we do not have to worry about discrepancies between the platform 
 * implementation of a char (MOB, char size, encoding). QString will accept literals as UTF-8. All else goes into a 
 * QString using QString::fromUtf16() and comes out of a QString using QString::utf16(). 
 *  
 * The following build options can change the default behaviour of QString; 
 *  
 * - QT_NO_CAST_FROM_ASCII (we use this)
 * - QT_RESTRICTED_CAST_FROM_ASCII 
 * - QT_NO_CAST_TO_ASCII (we use this) 
 *  
 * ODBC 
 *  
 * A few ODBC functions are called differently with UNICODE (ie byte counts become char counts etc). 
 * A UNICODE built Driver Manager (should be all of them) can interoperate with 8bit char applications and 
 * 8bit char Drivers - but any non-ASCII chars may be lost in such a scenario.
 *  
 * SQLWCHAR
 *
 * This is a char for UNICODE in unixODBC/MS ODBC. This should be equivalent to the following; ushort, unsigned short, WCHAR, char16_t
 * all of which are guaranteed to be 2 bytes. SQLTCHAR will map to SQLWCHAR for UNICODE so it can be used as well. However; wchar_t is 
 * NOT guaranteed to be 2 bytes in all cases so its best to ignore it.
 *
 * UTF-8 
 *  
 * This is a very common encoding for code editors, web documents, etc. As mentioned above - QString converts 
 * 'const char *' into UNICODE using fromUtf8() by default. Like UTF-16 - its a variable width character but is better 
 * than UTF-16 when the characters are more likley to just be ASCII. UTF-8 is NOT used for string encoding for 
 * us. We are built for either ASCII or UNICODE using UTF-16.
 *  
 * unixODBC-GUI-Qt
 *
 * It was decided, during the 2018 effort to make everything build for Qt 5, that unixODBC-GUI-Qt should only be
 * built for UNICODE (and to use UTF-16 encoding as per unixODBC-Core). 
 *
 * !!! this code WILL NOT WORK for an ANSII only 8bit char build !!!
 *
 */

#ifdef WIN32
    /* build against MS ODBC */
    #include <windows.h>
    #include <tchar.h>
    #include <sqlext.h>
    #include <odbcinst.h>

#else
    /* build against a default build of unixODBC-Core */
    #include <sqlext.h>
    #include <sqlucode.h>
    #include <odbcinst.h>
    /* An MS'ism which we ignore with UNIX'ism - compiler still needs to see something. */
    #define _TRUNCATE 0
#endif

#include <QObject>
#include <QString>
#include <QMap>

#endif

