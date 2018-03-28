/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "OQGConsole.h"

#include "Execute16.xpm"
#include "Connected16.xpm"
#include "Disconnected16.xpm"
#include "ODBC64.xpm"


OQGConsole::OQGConsole()
    : QMainWindow( 0 /*, WDestructiveClose */ )
{
    // init our gui elements...
    setWindowTitle( QString::fromLocal8Bit("OQGConsole") );
    setWindowIcon( QIcon( QPixmap(xpmODBC64) ) );
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createClientArea();
    createHandles();
}

OQGConsole::~OQGConsole()
{
    // we could just delete pSystem and let QObject destructor cleanup but lets do it explicitly
    if ( pStatement ) delete pStatement;
    if ( pConnection ) delete pConnection;
    if ( pEnvironment ) delete pEnvironment;
    if ( pSystem  ) delete pSystem;
}

void OQGConsole::createHandles()
{
    pSystem         = new OQGSystem(); 
    pSystem->doAlloc();

    pEnvironment    = new OQGEnvironment( pSystem ); 
    pEnvironment->doAlloc();
    pEnvironment->setAttrODBCVersion( OQGEnvironment::OVOdbc3 );

    pConnection     = new OQGConnection( pEnvironment ); 
    pConnection->doAlloc();

    connect( pConnection, SIGNAL(signalConnected()), this, SLOT(slotConnected()) );
    connect( pConnection, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()) );
    pStatement      = new OQGStatement( pConnection );
    connect( pStatement, SIGNAL(signalResults(OQStatement*)), this, SLOT(slotResults(OQStatement*)) );

    // messages
    connect( pSystem, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pEnvironment, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pConnection, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pStatement, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );

    connect( pSystem, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    connect( pEnvironment, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    connect( pConnection, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    connect( pStatement, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
}

void OQGConsole::createActions()
{
    pactionExecute = new QAction( QIcon( QPixmap(xpmExecute16) ), tr("&Execute"), this );
    pactionExecute->setShortcut( tr( "Ctrl+E" ) );
    pactionExecute->setStatusTip( tr( "Submit the text to the data source for execution" ) );
    connect( pactionExecute, SIGNAL(triggered()), this, SLOT(slotExecute()) );

    pactionQuit = new QAction( tr( "&Quit" ), this );
    pactionQuit->setShortcut( tr( "Ctrl+Q" ) );
    pactionQuit->setStatusTip( tr( "Quit this application" ) );
    connect( pactionQuit, SIGNAL(triggered()), this, SLOT(close()) );

    pactionConnect = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("&Connect"), this );
    pactionConnect->setStatusTip( tr( "Connect to a Data Source or Disconnect from current." ) );
    connect( pactionConnect, SIGNAL(triggered()), this, SLOT(slotConnectToggle()) );

    pactionCreateDataSource = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("C&reate Data Source"), this );
    pactionCreateDataSource->setStatusTip( tr( "Create a new Data Source Name that you can conveniently connect to later." ) );
    connect( pactionCreateDataSource, SIGNAL(triggered()), this, SLOT(slotCreateDataSource()) );

    pactionManageDataSources = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("&Manage Data Sources"), this );
    pactionManageDataSources->setStatusTip( tr( "Configure most things ODBC." ) );
    connect( pactionManageDataSources, SIGNAL(triggered()), this, SLOT(slotManageDataSources()) );

    pactionAbout = new QAction( tr( "&About" ), this );
    pactionAbout->setStatusTip( tr( "Show this application's About box" ) );
    connect( pactionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()) );
}

void OQGConsole::createMenus()
{
    pmenuFile = menuBar()->addMenu( tr( "&File" ) );
    pmenuFile->addAction( pactionExecute );
    pmenuFile->addAction( pactionQuit );

    pmenuDataSource = menuBar()->addMenu( tr( "&Data Source" ) );
    pmenuDataSource->addAction( pactionConnect );
    pmenuDataSource->addAction( pactionCreateDataSource );
    pmenuDataSource->addAction( pactionManageDataSources );

    menuBar()->addSeparator();

    pmenuHelp = menuBar()->addMenu( tr( "&Help" ) );
    pmenuHelp->addAction( pactionAbout );
}

void OQGConsole::createToolBars()
{
    ptoolbarFile = addToolBar(tr("File"));
    ptoolbarFile->addAction( pactionExecute );

    ptoolbarDataSource = addToolBar(tr("Data Source"));
    ptoolbarDataSource->addAction( pactionConnect );
}

void OQGConsole::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}

void OQGConsole::createClientArea()
{
    pSplitter           = new QSplitter( Qt::Vertical, this );          
    ptexteditSQL        = new QTextEdit( pSplitter );
    ptablewidgetResults = new QTableWidget( pSplitter );
    pmessageoutput      = new OQGMessageOutput( pSplitter );

    setCentralWidget( pSplitter );
    resize( 450, 600 );
}

void OQGConsole::slotConnectToggle()
{
    if ( pConnection->isConnected() )
        pConnection->doDisconnect();
    else
        pConnection->doConnect( this );
}

void OQGConsole::slotConnected()
{
    pactionConnect->setIcon( QIcon( QPixmap( xpmConnected16 ) ) );
}

void OQGConsole::slotDisconnected()
{
    pactionConnect->setIcon( QIcon( QPixmap( xpmDisconnected16 ) ) );
}

void OQGConsole::slotCreateDataSource()
{
    pSystem->doCreateDataSource( this );
}

void OQGConsole::slotManageDataSources()
{
    pSystem->doManageDataSources( this );
}

void OQGConsole::slotExecute() 
{
    // remove table items...
    ptablewidgetResults->clear();
    // remove table cells...
    ptablewidgetResults->setColumnCount( 0 );
    ptablewidgetResults->setRowCount( 0 );

    //
    pStatement->doExecDirect( ptexteditSQL->toPlainText() );
}

void OQGConsole::slotResults( OQStatement * ) 
{
    doResultGUIGrid();
}

void OQGConsole::doResultGUIGrid()
{
    SWORD       nCols;

    // GET NUMBER OF COLUMNS RETURNED
    if ( !SQL_SUCCEEDED( pStatement->doNumResultCols( &nCols )) )
        nCols = 0;

    if ( nCols < 0 ) nCols = 0;

    ptablewidgetResults->setRowCount( 0 );
    ptablewidgetResults->setColumnCount( nCols );

    // GET A RESULTS HEADER (column headers)
    doResultGUIGridHeader( nCols );

    // GET A RESULTS BODY (data)
    if ( nCols > 0 )
        doResultGUIGridBody( nCols );
}

void OQGConsole::doResultGUIGridHeader( SWORD nColumns )
{
	int			nCol;
    SQLSMALLINT nMaxBytes = 200;
	SQLWCHAR	szColumnName[nMaxBytes];	
    QStringList stringlistHeaderLabels;

	for( nCol = 0; nCol < nColumns; nCol++ )
	{
        memset( szColumnName, '\0', nMaxBytes );
		pStatement->doColAttribute( nCol + 1, SQL_DESC_LABEL, szColumnName, nMaxBytes, 0, 0 );
        stringlistHeaderLabels << QString::fromUtf16( szColumnName );
	}
    ptablewidgetResults->setHorizontalHeaderLabels( stringlistHeaderLabels );
}

void OQGConsole::doResultGUIGridBody( SWORD nColumns )
{
    SQLRETURN      	nReturn             = 0;
    SQLINTEGER      nRow                = 0;
    SQLUSMALLINT    nCol            	= 0;
    
    // for each result record...
    while( SQL_SUCCEEDED( (nReturn = pStatement->doFetch()) ) )
    {
        nRow++;
        // SOME DRIVERS DO NOT RETURN THE ROW COUNT PROPERLY SO EXPAND IF NEED BE 
        if ( ptablewidgetResults->rowCount() < nRow )
            ptablewidgetResults->setRowCount( nRow );

        // PROCESS ALL COLUMNS
        for( nCol = 1; nCol <= nColumns; nCol++ )
        {
            QVariant v = pStatement->getData( nCol );
            QTableWidgetItem *ptablewidgetitem;

            if ( v.isNull() )
            {
                ptablewidgetitem = new QTableWidgetItem( QString::fromLocal8Bit("") );
            }
            else
            {
                ptablewidgetitem = new QTableWidgetItem( v.toString() );
            }
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            ptablewidgetResults->setItem( nRow - 1, nCol - 1, ptablewidgetitem );
        }

    } // while rows
}

void OQGConsole::slotAbout()
{
    QMessageBox MessageBox;

    MessageBox.setText( QString::fromLocal8Bit("OQGConsole") );
    MessageBox.exec();
}



