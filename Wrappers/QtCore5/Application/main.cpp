/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See GPL.txt
 */
#include <QCoreApplication>
#include "OQConsole.h"

#ifdef WIN32
int _tmain( int argc, SQLTCHAR *argv[] )
{
    QCoreApplication    theApplication( argc, argv ); 
    QTextStream         streamIn( stdin ); 
    QTextStream         streamOutData( stdout );
    QTextStream         streamOutErrors( stderr ); 
    OQConsole           theConsole( theApplication.arguments(), &streamIn, &streamOutData, &streamOutErrors );

    return 0;
}
#else
int main( int argc, char *argv[] )
{
    QCoreApplication    theApplication( argc, argv ); 
    QTextStream         streamIn( stdin ); 
    QTextStream         streamOutData( stdout );
    QTextStream         streamOutErrors( stderr ); 
    OQConsole           theConsole( theApplication.arguments(), &streamIn, &streamOutData, &streamOutErrors );

    return 0;
}
#endif

