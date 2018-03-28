/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#include "OQConsole.h"
#include "OQDiagnostic.h"
#include <QVariant>

#define MAX_LINE_IN_CHARS 4096
#define MAX_LINE_OUT_CHARS 32001

const char *szSyntax =
"\n" \
"**********************************************\n" \
"* OQConsole                                  *\n" \
"**********************************************\n" \
"* Syntax                                     *\n" \
"*                                            *\n" \
"*   OQConsole [options]                      *\n" \
"*                                            *\n" \
"* Options                                    *\n" \
"*                                            *\n" \
"* -s         list data source names          *\n" \
"* -r         list drivers                    *\n" \
"* -b         batch.(no prompting etc)        *\n" \
"* -dx        delimit columns with x          *\n" \
"* -x0xXX     delimit columns with XX, where  *\n" \
"*            x is in hex, ie 0x09 is tab     *\n" \
"* -w         wrap results in an HTML table   *\n" \
"* -tTable    wrap results in INSERT SQL      *\n" \
"* -qn,n,...  comma seperated list of column  *\n" \
"*            numbers where values should be  *\n" \
"*            quoted (use with -t)            *\n" \
"* -ux        char to use to quote char data  *\n" \
"* -ex        char(s) to use to terminate     *\n" \
"*            statements (i.e. use with -t)   *\n" \
"* -c         column names on first row.      *\n" \
"*            (only used when -d)             *\n" \
"* -mn        limit column display width to n *\n" \
"* -i         wide-char string data           *\n" \
"* -a         make ODBC calls asynch          *\n" \
"* -v         verbose.                        *\n" \
"* --version  version                         *\n" \
"*                                            *\n" \
"* Notes                                      *\n" \
"*                                            *\n" \
"*      OQConsole supports redirection and  *\n" \
"*      piping for batch processing.          *\n" \
"*                                            *\n" \
"* Examples                                   *\n" \
"*                                            *\n" \
"*  OQConsole MyDSN MyID MyPWD -w < My.sql  *\n" \
"*                                            *\n" \
"* Please visit;                              *\n" \
"*                                            *\n" \
"*      http://www.unixodbc.org               *\n" \
"*      pharvey@peterharvey.org               *\n" \
"**********************************************\n\n";

const char *szIntro = 
"\n" \
"+---------------------------------------+\n" \
"|                                       |\n" \
"| Commands starts with a .              |\n" \
"|                                       |\n" \
"| Default command/statement terminator  |\n" \
"| is a ;                                |\n" \
"|                                       |\n" \
"| For example...                        |\n" \
"|                                       |\n" \
"| .help;                                |\n" \
"| .quit;                                |\n" \
"| SELECT COUNT(*) FROM MyTable;         |\n" \
"|                                       |\n" \
"+---------------------------------------+\n\n"; 

const char *szHelp =
"\n" \
".help sql;\n" \
".help show;\n" \
".help connect;\n" \
".help disconnect;\n" \
"\n\n";

const char *szHelpSql =
"\n" \
"SQL statements never start with a .\n\n" \
"The default terminator is a ;\n\n" \
"For example...\n\n" \
"SELECT COUNT(*) FROM MyTable;\n" \
"\n\n";

const char *szShow =
"\n" \
".show;\n" \
".show config;\n" \
".show driver [name];\n" \
".show dsn [name];\n" \
".show catalog [catalog];\n" \
".show schema [[catalog.]schema];\n" \
".show table [[[catalog.]schema.]table];\n" \
".show column [[[[catalog.]schema.]table.]column];\n" \
".show types;\n" \
"\n" \
"NOTE: use %% as a wildcard as needed\n" \
"\n\n";

const char *szHelpConnect =
"\n" \
".connect [DSN[UID[PWD]]];\n\n" \
"Connects to the data source (DSN).\n\n" \
"\n\n";

const char *szHelpDisconnect =
"\n" \
".disconnect;\n\n" \
"Disconnects from the data source.\n" \
"\n\n";


OQConsole::OQConsole( const QStringList &stringlistArguments, QTextStream *pstreamInCommands, QTextStream *pstreamOutData, QTextStream *pstreamOutErrors )
{
    nInteraction            = InteractionUndefined; // try to figure it out based upon call order
    nResultFormat           = Boxed;
    bWideCharData           = false;
    bAsynch                 = false;
    bVerbose                = false;
    nUserWidth              = 0; // 0=not applicable
    cDelimiter              = QLatin1Char('|');
    bColumnNames            = false;
    stringQuoteToUse        = QString::fromLocal8Bit( "\"" );
    cStatementTerminator    = QLatin1Char(';');

    this->pstreamInCommands  = pstreamInCommands;
    this->pstreamOutData     = pstreamOutData;
    this->pstreamOutErrors   = pstreamOutErrors;

    pSystem         = new OQSystem();
    pEnvironment    = new OQEnvironment( pSystem );
    pConnection     = new OQConnection( pEnvironment );
    pStatement      = new OQStatement( pConnection );

    connect( pSystem, SIGNAL(signalMessage(OQMessage)), SLOT(slotMessage(OQMessage)) );
    connect( pEnvironment, SIGNAL(signalMessage(OQMessage)), SLOT(slotMessage(OQMessage)) );
    connect( pConnection, SIGNAL(signalMessage(OQMessage)), SLOT(slotMessage(OQMessage)) );
    connect( pStatement, SIGNAL(signalMessage(OQMessage)), SLOT(slotMessage(OQMessage)) );
    connect( pSystem, SIGNAL(signalDiagnostic(OQDiagnostic)), SLOT(slotDiagnostic(OQDiagnostic)) );

    doParseArgs( stringlistArguments );
}

OQConsole::~OQConsole()
{
    delete pStatement;
    delete pConnection;
    delete pEnvironment;
    delete pSystem;
}

bool OQConsole::exec()
{
    if ( nInteraction == CommandLineInteractive )
        return doProcessInteractive();

    return doProcessBatch();
}


/*!
    doParseArgs
    
*/    
bool OQConsole::doParseArgs( const QStringList &stringlistArguments )
{
    for ( int n = 0; n < stringlistArguments.size(); n++ )
    {
        QString stringArgument = stringlistArguments.at( n );

        if ( stringArgument == QString::fromLocal8Bit( "-b" ) )
        {
            continue;
        }

        if ( stringArgument == QString::fromLocal8Bit( "-e" ) )
        {
        }
    }

    return true;
}

bool OQConsole::doConnect( const QString &stringDSN, const QString &stringUID, const QString &stringPWD )
{
    if ( nInteraction == OQConsole::InteractionUndefined )
        nInteraction = ApplicationControlled;

    if ( !SQL_SUCCEEDED( pConnection->doConnect( stringDSN, stringUID, stringPWD ) ) )
    {
        *pstreamOutErrors << tr( "[OQConsole]ERROR: Failed to connect\n" );
        return false;
    }

    pStatement->setAttrMaxLength( nUserWidth );

    return true;
}

bool OQConsole::doDisconnect()
{
//    if ( pStatement->isAlloc( false ) )
//        pStatement->doFree();

    if ( !SQL_SUCCEEDED( pConnection->doDisconnect() ) )
    {
        *pstreamOutErrors << tr("[OQConsole]ERROR: Failed to properly disconnect\n");
        return false;
    }

    return true;
}

/*!
    doProcess
    
    This is the main loop for the application. It accepts and processes
    user input.
*/
bool OQConsole::doProcess()
{
    // Turning the ODBC asynch on may result in some feedback while long requests
    // are waited on. Mostly its just here to test asynch.
    if ( bAsynch )
    {
        if ( !SQL_SUCCEEDED( pStatement->setAttrAsyncEnable( OQStatement::AsyncEnableOn ) ) )
        {
            *pstreamOutErrors << tr("[OQConsole]ERROR: Failed to turn asynch on\n");
            return false;
        }
    }

    //
    // PROCESS
    //
    switch ( nInteraction )
    {
        case ApplicationControlled:
            doProcessInteractive();
            break;
        case CommandLineBatch:
            doProcessBatch();
            break;
        case CommandLineInteractive:
            doProcessInteractive();
            break;
        case InteractionUndefined:
            nInteraction = ApplicationControlled;
            doProcessInteractive();
            break;
    }

    return true;
}

/*!
    doProcessBatch
    
    Main loop.
*/
bool OQConsole::doProcessBatch()
{
    QString     stringCommand;          // command in the making
    QString     stringLine;             // contains last line read
    QChar       cQuote          = 0;    // contains quote if we are in quote

    // display some help
    *pstreamOutData << szHelp;

    // 
    do 
    {
        stringLine = pstreamInCommands->readLine();

        // process line
        for ( int n = 0; n < stringLine.size(); n++ )
        {
            if ( stringLine[n] == cStatementTerminator && cQuote == 0 )
            {
                // we have a command
                if ( !stringCommand.isEmpty() )
                {
                    // process command
                    if ( !doProcessCommand( stringCommand ) )
                    {
                        // we have a quit command so quit both loops and exit function
                        stringLine = QString::null;
                        break; 
                    }

                    // cleanup
                    stringCommand == QString::fromLocal8Bit("");
                }
            }
            else
            {
                // toggle quote state as required
                // - we do not want to interpret a statement terminator when it is in quotes
                // - this is simplistic because it does not handle quotes which may be escaped in string
                if ( cQuote != 0 )
                {
                    if ( cQuote == stringLine[n] )
                        cQuote = 0;
                }
                else
                {
                    if ( stringLine[n] == QLatin1Char( '\'' ) || stringLine[n] == QLatin1Char( '\"' ) )
                        cQuote = stringLine[n];
                }

                // accum
                stringCommand.append( stringLine[n] );
            }
        }

    } while ( !stringLine.isNull() );

    return 1;
}

/*!
    doProcessInteractive
    
    Main loop.
*/
bool OQConsole::doProcessInteractive()
{
    QString     stringCommand;          // command in the making
    QString     stringLine;             // contains last line read
    QChar       cQuote          = 0;    // contains quote if we are in quote

    // display some help
    *pstreamOutData << tr(szHelp);

    // 
    do 
    {
        if ( cQuote != 0 )
            *pstreamOutData << tr("OQConsole (text currently in quotes)> ");
        else
            *pstreamOutData << QString::fromLocal8Bit( "OQConsole> " );

        stringLine = pstreamInCommands->readLine();

        // process line
        for ( int n = 0; n < stringLine.size(); n++ )
        {
            if ( stringLine[n] == cStatementTerminator && cQuote == 0 )
            {
                // we have a command
                if ( !stringCommand.isEmpty() )
                {
                    // process command
                    if ( !doProcessCommand( stringCommand ) )
                    {
                        // we have a quit command so quit both loops and exit function
                        stringLine = QString::null;
                        break; 
                    }

                    // cleanup
                    stringCommand == QString::fromLocal8Bit( "" );
                }
            }
            else
            {
                // toggle quote state as required
                // - we do not want to interpret a statement terminator when it is in quotes
                // - this is simplistic because it does not handle quotes which may be escaped in string
                if ( cQuote != 0 )
                {
                    if ( cQuote == stringLine[n] )
                        cQuote = 0;
                }
                else
                {
                    if ( stringLine[n] == QLatin1Char('\'') || stringLine[n] == QLatin1Char( '\"' ) )
                        cQuote = stringLine[n];
                }

                // accum
                stringCommand.append( stringLine[n] );
            }
        }

    } while ( !stringLine.isNull() );

    return 1;
}

/*!
    doProcessCommand

    Return false to exit main loop and app.    
*/
bool OQConsole::doProcessCommand( const QString &stringCommand )
{
    QString stringCommandTrimmed = stringCommand.trimmed();

    //
    if ( stringCommandTrimmed.isEmpty() )
        return 1;

    //
    if ( stringCommandTrimmed.left( 4 ) == QString::fromLocal8Bit("quit") )
        return 0;
    else if ( stringCommandTrimmed.left( 5 ) == QString::fromLocal8Bit("show ") )            // process the given 'show' command
        doExecuteShow( stringCommandTrimmed.mid( 6 ) );
    else if ( stringCommandTrimmed.left( 4 ) == QString::fromLocal8Bit("show") )             // display the 'show' commands
        doExecuteShow();
    else if ( stringCommandTrimmed.left( 4 ) == QString::fromLocal8Bit("help") )             // display some help
    {
        *pstreamOutData << szHelp;
        *pstreamOutData << "OQConsole> ";
    }
    else
        doExecuteSQL( stringCommandTrimmed );                         // process the given 'SQL' statement

    return 1;
}

/*!
    doExecuteSQL
    
    Prepare and execute SQL then process any results.
*/
bool OQConsole::doExecuteSQL( const QString &stringSQL )
{
    if ( !SQL_SUCCEEDED( pStatement->doPrepare( stringSQL ) ) )
    {
//        pStatement->doFree();
        *pstreamOutErrors << tr("[OQConsole]ERROR: While preparing statement\n");
        return false;
    }

    if ( !SQL_SUCCEEDED( pStatement->doExecute() ) )
    {
//        pStatement->doFree();
        *pstreamOutErrors << tr("[OQConsole]ERROR: While executing statement\n");
        return false;
    }

    doProcessResultSet();

    return true;
}

/*!
    doExecuteShow
    
    Process "show" commands.
*/
bool OQConsole::doExecuteShow( const QString &stringShow )
{
    if ( !stringShow.isEmpty() )
    {
        *pstreamOutData << szShow;
        return true;
    }

    // config
    if ( stringShow.left( 6 ) == QString::fromLocal8Bit("config") )
    {
        return doExecuteShowConfig();
    }
    // driver details
    else if ( stringShow.left( 7 ) == QString::fromLocal8Bit("driver ") )
    {
        return doExecuteShowDriver( stringShow.mid( 8 ) );
    }
    // driver list
    else if ( stringShow.left( 6 ) == QString::fromLocal8Bit("driver") )
    {
        return doExecuteShowDrivers();
    }
    // show dsn
    else if ( stringShow.left( 4 ) ==  QString::fromLocal8Bit("dsn ") )
    {
        return doExecuteShowDataSourceName( stringShow.mid( 5 ) );
    }
    else if ( stringShow.left( 3 ) ==  QString::fromLocal8Bit("dsn") )
    {
        return doExecuteShowDataSourceNames();
    }
    // show catalog
    else if ( stringShow.left( 7 ) == QString::fromLocal8Bit("catalog") )
    {
        SQLRETURN nReturn;

        if ( stringShow.at( 7 ) == QLatin1Char(' ') )
            nReturn = pStatement->doTables( stringShow.mid( 8 ) );
        else
            nReturn = pStatement->doTables( QString::fromLocal8Bit(SQL_ALL_CATALOGS) );

        if ( !SQL_SUCCEEDED( nReturn ) )
        {
            *pstreamOutErrors << tr("[OQConsole]ERROR: Failed to get catalogs\n");
            return false;
        }
    }
    // show schema
    else if ( stringShow.left( 6 ) == QString::fromLocal8Bit("schema") )
    {
        SQLRETURN nReturn;

        if ( stringShow.at( 6 ) == QLatin1Char(' ') )
            nReturn = pStatement->doTables( QString::null, stringShow.mid( 7 ) );
        else
            nReturn = pStatement->doTables( QString::null, QString::fromLocal8Bit(SQL_ALL_SCHEMAS) );

        if ( !SQL_SUCCEEDED( nReturn ) )
        {
            *pstreamOutErrors << tr( "[OQConsole]ERROR: Failed to get schemas\n" );
            return false;
        }
    }
    // show table
    else if ( stringShow.left( 5 ) == QString::fromLocal8Bit("table") )
    {
        SQLRETURN nReturn;

        if ( stringShow.at( 5 ) == QLatin1Char(' ') )
        {
            nReturn = pStatement->doColumns( QString::null, QString::null, stringShow.mid( 6 ), QString::null );
        }
        else
        {
            nReturn = pStatement->doTables( QString::null, QString::null, QString::null, QString::fromLocal8Bit("TABLE") );
        }

        if ( !SQL_SUCCEEDED( nReturn ) )
        {
            *pstreamOutErrors << tr("[OQConsole]ERROR: Failed to get tables\n");
            return false;
        }
    }
    // show column
    else if ( stringShow.left( 6 ) == QString::fromLocal8Bit("column") )
    {
        /*! 
         * \note    1. MS Access needs Catalog = NULL. Catalog = "" does not work.
         *  
         */
        if ( !SQL_SUCCEEDED( pStatement->doColumns( QString::null, QString::null, stringShow.mid( 7 ), QString::fromLocal8Bit("%") ) ) )
        {
            *pstreamOutErrors << tr("[OQConsole]ERROR: While requesting column listing\n");
            return false;
        }
    }
    // show types
    else if ( stringShow.left( 5 ) == QString::fromLocal8Bit("types") )
    {
        if ( !SQL_SUCCEEDED( pStatement->doTypeInfo() ) )
        {
            *pstreamOutErrors << tr("[OQConsole]ERROR: Failed to get type info\n");
            return false;
        }
    }

    doProcessResultSet();

    return true;
}

bool OQConsole::doExecuteShowConfig()
{
    OQAttributes Attributes = pSystem->getAttributes();

    // get col widths
    int nKeyMaxChars    = 5;
    int nValueMaxChars  = 7;
    {
        QMapIterator<QString, QString> i( Attributes.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            nKeyMaxChars    = max( i.key().length(), nKeyMaxChars );
            nValueMaxChars  = max( i.value().length(), nValueMaxChars );
        }
    }

    // header
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("")    << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("Key") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("Value") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << endl;

    // body
    {
        QMapIterator<QString, QString> i( Attributes.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << qSetFieldWidth( nKeyMaxChars ) << i.key() << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << qSetFieldWidth( nValueMaxChars ) << i.value() << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("|") << endl;
        }
    }

    // footer
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QString::fromLocal8Bit("+") << endl;

    return true;
}

bool OQConsole::doExecuteShowDriver( const QString &stringDriver )
{
    OQDriver    Driver = pSystem->getDriver( stringDriver );

    // get col widths
    int nKeyMaxChars    = 5;
    int nValueMaxChars  = 7;
    {
        QMapIterator<QString, QString> i( Driver.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            nKeyMaxChars    = max( i.key().length(), nKeyMaxChars );
            nValueMaxChars  = max( i.value().length(), nValueMaxChars );
        }
    }

    // header
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("")    << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("")      << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("Key") << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("Value") << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("")    << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("")      << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    // body
    {
        QMapIterator<QString, QString> i( Driver.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nKeyMaxChars ) << i.key() << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nValueMaxChars ) << i.value() << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
        }
    }

    // footer
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    return true;
}

bool OQConsole::doExecuteShowDrivers()
{
    QVector<OQDriver>   vectorDrivers = pSystem->getDrivers();

    // get col widths
    int nNameMaxChars        = 6;
    int nDescriptionMaxChars = 13;
    for ( int n = 0; n < vectorDrivers.count(); n++ )
    {
        nNameMaxChars        = max( vectorDrivers[n].stringName.length(), nNameMaxChars );
        nDescriptionMaxChars = max( vectorDrivers[n].mapAttributes[QString::fromLocal8Bit("DESCRIPTION")].length(), nDescriptionMaxChars );
    }

    // header
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("")     << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("")            << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("Name") << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("Description") << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("")     << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("")            << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    // body
    for ( int n = 0; n < vectorDrivers.count(); n++ )
    {
        *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nNameMaxChars ) << vectorDrivers[n].stringName << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nDescriptionMaxChars ) << vectorDrivers[n].mapAttributes[QString::fromLocal8Bit("DESCRIPTION")] << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    }

    // footer
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    return true;
}

bool OQConsole::doExecuteShowDataSourceName( const QString &stringDataSourceName )
{
    OQDataSourceName DataSourceName = pSystem->getDataSource( stringDataSourceName );

    // get col widths
    int nKeyMaxChars    = 5;
    int nValueMaxChars  = 7;
    {
        QMapIterator<QString, QString> i( DataSourceName.Attributes.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            nKeyMaxChars    = max( i.key().length(), nKeyMaxChars );
            nValueMaxChars  = max( i.value().length(), nValueMaxChars );
        }
    }

    // header
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("")    << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("")      << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("Key") << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("Value") << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("")    << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("")      << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    // body
    {
        QMapIterator<QString, QString> i( DataSourceName.Attributes.mapAttributes );
        while ( i.hasNext() ) 
        {
            i.next();
            *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nKeyMaxChars ) << i.key() << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nValueMaxChars ) << i.value() << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
        }
    }

    // footer
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nKeyMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nValueMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    return true;
}

bool OQConsole::doExecuteShowDataSourceNames()
{
    QVector<OQDataSourceName> vectorDataSourceNames = pSystem->getDataSources();

    // get col widths
    int nNameMaxChars        = 6;
    int nDescriptionMaxChars = 13;
    for ( int n = 0; n < vectorDataSourceNames.count(); n++ )
    {
        OQDataSourceName DataSourceName = vectorDataSourceNames[n];
        nNameMaxChars        = max( DataSourceName.Attributes.stringName.length(), nNameMaxChars );
        nDescriptionMaxChars = max( DataSourceName.Attributes.mapAttributes[QString::fromLocal8Bit("DESCRIPTION")].length(), nDescriptionMaxChars );
    }

    // header
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("")     << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("")            << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("Name") << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("Description") << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("")     << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("")            << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    // body
    for ( int n = 0; n < vectorDataSourceNames.count(); n++ )
    {
        OQDataSourceName DataSourceName = vectorDataSourceNames[n];
        *pstreamOutData << qSetPadChar( QLatin1Char(' ') ) << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nNameMaxChars ) << DataSourceName.Attributes.stringName << qSetFieldWidth( 1 ) << QLatin1Char('|') << qSetFieldWidth( nDescriptionMaxChars ) << DataSourceName.Attributes.mapAttributes[QString::fromLocal8Bit("DESCRIPTION")] << qSetFieldWidth( 1 ) << QLatin1Char('|') << endl;
    }

    // footer
    *pstreamOutData << qSetPadChar( QLatin1Char('-') ) << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nNameMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << qSetFieldWidth( nDescriptionMaxChars ) << QString::fromLocal8Bit("") << qSetFieldWidth( 1 ) << QLatin1Char('+') << endl;

    return true;
}

void OQConsole::doProcessResultSet()
{
    QString     stringSepLine;
    SQLSMALLINT nCols   = -1;
    SQLLEN      nRows   = 0;

    /*
     * check to see if it has generated a result set
     */
    if ( !SQL_SUCCEEDED( pStatement->doNumResultCols( &nCols ) ) )
    {
//        pStatement->doFree();
        *pstreamOutErrors << tr("[OQConsole]ERROR: While requesting number of columns\n");
        return;
    }

    if ( nCols > 0 )
    {
        /****************************
         * WRITE HEADER
         ***************************/
        switch ( nResultFormat )
        {
            case Boxed:
                stringSepLine = doWriteHeaderNormal();
                break;
            case Delimited:
                doWriteHeaderDelimited();
                break;
            case Html:
                doWriteHeaderHTMLTable();
                break;
            case Insert:
                break;
        }

        /****************************
         * WRITE BODY
         ***************************/
        switch ( nResultFormat )
        {
            case Boxed:
                nRows = doWriteBodyNormal();
                break;
            case Delimited:
                doWriteBodyDelimited();
                break;
            case Html:
                nRows = doWriteBodyHTMLTable();
                break;
            case Insert:
                doWriteBodyInsertTable();
                break;
        }
    }

    /****************************
     * WRITE FOOTER
     ***************************/
    switch ( nResultFormat )
    {
        case Boxed:
            doWriteFooterNormal( stringSepLine, nRows );
            break;
        case Delimited:
            break;
        case Html:
            doWriteFooterHTMLTable( nRows );
            break;
        case Insert:
            break;
    }

    // fini
    pStatement->doCloseCursor();
}

/****************************
 * WRITE HTML
 ***************************/
void OQConsole::doWriteHeaderHTMLTable()
{
    SQLUSMALLINT    nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLWCHAR        szColumnName[MAX_DATA_WIDTH+1];

    if ( !SQL_SUCCEEDED( pStatement->doNumResultCols( &nColumns ) ) )
        nColumns = -1;

    *pstreamOutData << QString::fromLocal8Bit( "<div id=ResultSet>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "<table border>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t<thead>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t\t<tr>\n" );

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        *szColumnName = '\0';
        pStatement->doColAttribute( nCol, SQL_DESC_LABEL, szColumnName, MAX_DATA_WIDTH, NULL, NULL );
        *pstreamOutData << QString::fromLocal8Bit( "\t\t\t<th scope=\"col\">" );
        *pstreamOutData << szColumnName;
        *pstreamOutData << QString::fromLocal8Bit( "</th>\n" );
    }
    *pstreamOutData << QString::fromLocal8Bit( "\t\t</tr>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t</thead>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t<tbody>\n" );
}

SQLLEN OQConsole::doWriteBodyHTMLTable()
{
    SQLUSMALLINT    nCol        = 0;
    SQLSMALLINT     nColumns    = 0;
    SQLRETURN       nReturn     = 0;
    SQLLEN          nRows       = 0;

    QVariant v;
//    v.setElementsMax( MAX_DATA_WIDTH );

    pStatement->doNumResultCols( &nColumns );

    /* ROWS */
    while ( SQL_SUCCEEDED( pStatement->doFetch() ) )
    {
        nRows++;
        *pstreamOutData << QString::fromLocal8Bit( "\t\t<tr>\n" );
        *pstreamOutData << QString::fromLocal8Bit( "\t\t\t<th scope=\"row\" id=\"") << nRows << QString::fromLocal8Bit( "\"></th>\n" );
        /* COLS */
        for ( nCol = 1; nCol <= nColumns; nCol++ )
        {
            *pstreamOutData << QString::fromLocal8Bit( "\t\t\t<td>" );
            v = pStatement->getData( nCol, &nReturn );
            if ( v.canConvert<QString>() ) 
                *pstreamOutData << v.toString();
            else
                *pstreamOutData << tr("can not convert to string");
            *pstreamOutData << QString::fromLocal8Bit( "</td>\n" );
        }
        *pstreamOutData << QString::fromLocal8Bit( "\t\t</tr>\n" );
    }

    return nRows;
}

void OQConsole::doWriteFooterHTMLTable( SQLLEN nRows )
{
    SQLSMALLINT nColumns      = 1;
    SQLLEN      nRowsAffected = -1;

    pStatement->doNumResultCols( &nColumns );
    pStatement->doRowCount( &nRowsAffected );

    *pstreamOutData << QString::fromLocal8Bit( "\t</tbody>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t<tfoot>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t\t<tr>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t\t\t<td colspan=\"" ) << nColumns << QString::fromLocal8Bit( "\">Rows affected: ") << nRowsAffected << QString::fromLocal8Bit( " returned: ") << nRows << QString::fromLocal8Bit( "</th>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t\t</tr>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "\t</tfoot>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "</table>\n" );
    *pstreamOutData << QString::fromLocal8Bit( "</div>\n" );
}

/****************************
 * WRITE INSERT TABLE 
 ***************************/
void OQConsole::doWriteBodyInsertTable()
{
    SQLUSMALLINT    nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLRETURN       nReturn                         = 0;
    bool *          pbQuote                         = 0;

    QVariant v;
 //   v.setElementsMax( MAX_DATA_WIDTH );

    pStatement->doNumResultCols( &nColumns );

    if ( nColumns <= 0 )
        return;

    pbQuote = (bool*)calloc( sizeof(bool), nColumns );

    // parse stringColumnsToQuote
    if ( !stringColumnsToQuote.isEmpty())
    {
        int         nCursorColumnsToQuote   = 0;
        int         nCursorColumn           = 0;
        QString     stringColumn;

        while ( 1 )
        {
            if ( stringColumnsToQuote[nCursorColumnsToQuote] == QLatin1Char(',') || stringColumnsToQuote[nCursorColumnsToQuote] == QLatin1Char('\0') )
            {
                if ( nCursorColumn )
                {
                    stringColumn[nCursorColumn] = QChar::Null;
                    int nColumn = stringColumn.toInt();
                    if ( nColumn > 0 && nColumn <= nColumns )
                    {
                        pbQuote[nColumn - 1] = 1;
                    }
                    nCursorColumn = 0;
                }
            }
            else
            {
                stringColumn[nCursorColumn] = stringColumnsToQuote[nCursorColumnsToQuote];
                nCursorColumn++;
            }

            if ( stringColumnsToQuote[nCursorColumnsToQuote] == QLatin1Char('\0') )
                break;

            nCursorColumnsToQuote++;
        } // while
    }

    // process results
    while ( SQL_SUCCEEDED( pStatement->doFetch() ) ) /* ROWS */
    {
        *pstreamOutData << QString::fromLocal8Bit("INSERT INTO ") << stringInsertTable << QString::fromLocal8Bit( " VALUES ( ");
        for ( nCol = 1; nCol <= nColumns; nCol++ ) /* COLS */
        {
            if ( nCol > 1 )
                *pstreamOutData << QString::fromLocal8Bit( ", " );

            v = pStatement->getData( nCol, &nReturn );
            if ( v.isNull() )
                *pstreamOutData << QString::fromLocal8Bit( "NULL" );
            else
            {
                *pstreamOutData <<  ( pbQuote[nCol - 1] ? stringQuoteToUse : QString::fromLocal8Bit(("") ) ); 
                if ( v.canConvert<QString>() )
                    *pstreamOutData << v.toString();
                else
                    *pstreamOutData << tr("Could not convert to string.");

                *pstreamOutData <<  ( pbQuote[nCol - 1] ? stringQuoteToUse : QString::fromLocal8Bit(("") ) ); 
            }
        } // cols
        *pstreamOutData << QString::fromLocal8Bit( " )" ) << cStatementTerminator << endl;
    } // rows

    free( pbQuote );
}

/****************************
 * WRITE DELIMITED
 * - this output can be used by the ODBC Text File driver
 * - last column no longer has a delimit char (it is implicit)...
 *   this is consistent with odbctxt
 ***************************/
void OQConsole::doWriteHeaderDelimited()
{
    SQLUSMALLINT    nCol                            = 0;
    SQLSMALLINT     nColumns                        = 0;
    SQLWCHAR        szColumnName[MAX_DATA_WIDTH+1];
    SQLSMALLINT     nBufferLengthBytes = MAX_DATA_WIDTH * sizeof(SQLWCHAR);
    SQLSMALLINT     nStringLengthBytesAvailable = 0;

    if ( !SQL_SUCCEEDED( pStatement->doNumResultCols( &nColumns ) ) )
        nColumns = -1;

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        pStatement->doColAttribute( nCol, SQL_DESC_LABEL, szColumnName, nBufferLengthBytes, &nStringLengthBytesAvailable, NULL );
        *pstreamOutData << qSetFieldWidth( (nStringLengthBytesAvailable<=nBufferLengthBytes ? nStringLengthBytesAvailable : nBufferLengthBytes ) ) << szColumnName;
        if ( nCol < nColumns )
            *pstreamOutData << cDelimiter;
    }
    *pstreamOutData << endl;
}

void OQConsole::doWriteBodyDelimited()
{
    SQLSMALLINT     nColumns                        = 0;
    SQLRETURN       nReturn                         = 0;

    nReturn = pStatement->doNumResultCols( &nColumns );
    if ( nColumns <= 0 )
        return;

    /* ROWS */
    nReturn = pStatement->doFetch();
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        /* COLS */
        for ( SQLUSMALLINT nCol = 1; nCol <= nColumns; nCol++ )
        {
            QVariant v = pStatement->getData( nCol, &nReturn );
            if ( !v.isNull() )
            {
                if ( v.canConvert<QString>() )
                    *pstreamOutData << v.toString();
                else
                    *pstreamOutData << tr("Could not convert to string.");

            }
            if ( nCol < nColumns )
                *pstreamOutData << cDelimiter;
        }
        *pstreamOutData << endl;
        nReturn = pStatement->doFetch();
    }
}

/****************************
 * WRITE NORMAL
 ***************************/
QString OQConsole::doWriteHeaderNormal()
{
    SQLSMALLINT     nColumns                        = 0;
    QString         stringNameLine;
    QString         stringBoxLine; 

    vectorColumnWidths = getColumnWidths( pStatement, nUserWidth );
    nColumns = vectorColumnWidths.size();

    // process columns...
    for ( SQLUSMALLINT nCol = 1; nCol <= nColumns; nCol++ )
    {
        SQLUINTEGER nColumnWidth = vectorColumnWidths.at( nCol - 1 );
        QString     stringColumnName;

        // get stringColumnName
        {
            SQLWCHAR        szColumnName[MAX_DATA_WIDTH+1];
            SQLSMALLINT     nBufferLengthBytes = MAX_DATA_WIDTH * sizeof(SQLWCHAR);
            SQLSMALLINT     nStringLengthBytesAvailable = 0;

            // Get column name - always null terminated but may be truncated.
            pStatement->doColAttribute( nCol, SQL_DESC_LABEL, szColumnName, nBufferLengthBytes, &nStringLengthBytesAvailable, NULL );
            // Always null terminated. Ignore possible truncation (case where nStringLengthBytesAvailable > 0).
            stringColumnName = QString::fromUtf16( szColumnName );
        }

        // Advance the box line...
        stringBoxLine += QLatin1Char('+');
        for ( SQLUINTEGER n = 0; n <= nColumnWidth; n++ ) { stringBoxLine += QLatin1Char('-'); }

        // Advance the name line...
        stringNameLine += QLatin1Char('|');
//        stringNameLine << qSetFieldWidth( vectorColumnWidths.at( nCol - 1 ) ) << stringColumnName;
        stringNameLine += stringColumnName;
    }
    stringBoxLine += QLatin1Char('+');
    stringNameLine += QLatin1Char('|');

    *pstreamOutData << stringBoxLine << endl;
    *pstreamOutData << stringNameLine << endl;
    *pstreamOutData << stringBoxLine << endl;

    // return box line so it can used at the end of the data output
    return stringBoxLine;
}

SQLLEN OQConsole::doWriteBodyNormal()
{
    SQLSMALLINT     nColumns                        = 0;
    SQLRETURN       nReturn                         = 0;
    SQLLEN          nRows                           = 0;

    /*! 
     * We ignore the following for now. 
     *  
     * \note
     *          We can be a single-char app and still work with wide-char data. Conversely; we can be 
     *          a wide-char app and need to work with single-char data.
     *  
     * \sa      setWideCharData
     */

    nColumns == vectorColumnWidths.size();
    
    /* ROWS */
    nReturn = pStatement->doFetch();
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        /* COLS */
        for ( SQLUSMALLINT nCol = 1; nCol <= nColumns; nCol++ )
        {
            // SQLUINTEGER nColumnWidth = vectorColumnWidths.at( nCol - 1 );

            // get the data
            QVariant v = pStatement->getData( nCol, &nReturn );
            *pstreamOutData << QLatin1Char( '|' );
            if ( !v.isNull() )
            {
                if ( v.canConvert<QString>() )
                    *pstreamOutData << v.toString();
                else
                    *pstreamOutData << tr("Could not convert to string.");

            }
/*
            if ( v.isNull() )
                ODBCPrintF( (ODBCCPTR)TEXT("| %-*s"), (int)nColumnWidth, TEXT("") );
            else
            {
                if ( bWideCharData )
                    wprintf( L"| %-*.*s", (int)nColumnWidth, (int)nColumnWidth, (SQLWCHAR*)v.getBuffer() );
                else
                    printf( "| %-*.*s", (int)nColumnWidth, (int)nColumnWidth, (SQLCHAR*)v.getBuffer() );
            }
*/
        } /* for columns */

        nRows++;
        *pstreamOutData << QLatin1Char( '|' ) << endl;

        // next record
        nReturn = pStatement->doFetch();
    } /* while rows */

    return nRows;
}

void OQConsole::doWriteFooterNormal( const QString &stringBoxLine, SQLLEN nRows )
{
    SQLLEN nRowsAffected = -1;

    // close the box off...
    *pstreamOutData << stringBoxLine << endl;

    // show number of rows returned/affected...
    pStatement->doRowCount( &nRowsAffected );
    if ( nRowsAffected >= 0 && nInteraction == CommandLineInteractive )
        *pstreamOutData << nRowsAffected << tr( " rows affected" ) << endl;

    if ( nRows && nInteraction == CommandLineInteractive )
        *pstreamOutData << nRows << tr( " rows returned" ) << endl;
}

QVector<SQLUINTEGER> OQConsole::getColumnWidths( OQStatement *pStatement, SQLUINTEGER nUserWidth )
{
    QVector<SQLUINTEGER> vectorColumnWidths;
    SQLUSMALLINT        nCol       = 0;
    SQLSMALLINT         nColumns   = 0;

    if ( !SQL_SUCCEEDED( pStatement->doNumResultCols( &nColumns ) ) )
        nColumns = -1;

    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        SQLUINTEGER nColumnWidth = getColumnWidth( pStatement, nUserWidth, nCol );
        vectorColumnWidths.push_back( nColumnWidth );
    }

    return vectorColumnWidths;
}

/*!
 * \brief   Get the optimal display width for a column.
 * 
 *          The display width is calculated using;
 * 
 *          \li SQL_DESC_DISPLAY_SIZE (not less than this - subject to following values)
 *          \li width of SQL_DESC_LABEL (not less than this - subject to following values)
 *          \li any User provided max (never exceeds this)
 *          \li MAX_DATA_WIDTH (never exceeds this)
 * 
 * \param pStatement 
 * \param nUserWidth 
 * \param nCol 
 * 
 * \return SQLUINTEGER 
 *
 * \author pharvey (8/22/2008)
 */
SQLUINTEGER OQConsole::getColumnWidth( OQStatement *pStatement, SQLUINTEGER nUserWidth, SQLUSMALLINT nCol )
{
    SQLLEN      nLabelWidth                     = 10;
    SQLLEN      nDataWidth                      = 10;
    SQLUINTEGER nOptimalDisplayWidth            = 10;
    SQLWCHAR    szColumnName[MAX_DATA_WIDTH+1];

    pStatement->doColAttribute( nCol, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, &nDataWidth );
    pStatement->doColAttribute( nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
    nLabelWidth = QString::fromUtf16( szColumnName ).length();

    nOptimalDisplayWidth = max( nLabelWidth, nDataWidth );

    if ( nUserWidth > 0 )
        nOptimalDisplayWidth = min( nOptimalDisplayWidth, nUserWidth );

    if ( nOptimalDisplayWidth > MAX_DATA_WIDTH )
        nOptimalDisplayWidth = MAX_DATA_WIDTH;

    return nOptimalDisplayWidth;
}

void OQConsole::slotMessage( OQMessage Message )
{
    QTextStream streamErr( stderr );
    streamErr << Message.getText();
}

void OQConsole::slotDiagnostic( OQDiagnostic Diagnostic )
{
    QTextStream streamErr( stderr );
    SQLINTEGER nRecords = Diagnostic.getNumber();

    for ( SQLINTEGER nRecord = 1; nRecord <= nRecords; nRecord++ )
    {
        *pstreamOutErrors << OQDiagnosticRecord( &Diagnostic, nRecord ).getMessageText() << endl;
    }
}


