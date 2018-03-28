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

#include "New16.xpm"
#include "Close16.xpm"
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
    if ( pConnection ) delete pConnection;
    if ( pEnvironment ) delete pEnvironment;
    if ( pSystem  ) delete pSystem;
}

void OQGConsole::createActions()
{

    // file
    pactionQuit = new QAction( tr( "&Quit" ), this );
    pactionQuit->setShortcut( tr( "Ctrl+Q" ) );
    pactionQuit->setStatusTip( tr( "Quit this application" ) );
    connect( pactionQuit, SIGNAL(triggered()), this, SLOT(close()) );

    // data sources
    pactionDataSourceConnect = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("&Connect"), this );
    pactionDataSourceConnect->setStatusTip( tr( "Connect to a Data Source or Disconnect from current." ) );
    connect( pactionDataSourceConnect, SIGNAL(triggered()), this, SLOT(slotConnectToggle()) );

    pactionDataSourceNew = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("C&reate Data Source"), this );
    pactionDataSourceNew->setStatusTip( tr( "Create a new Data Source Name that you can conveniently connect to later." ) );
    connect( pactionDataSourceNew, SIGNAL(triggered()), this, SLOT(slotDataSourceNew()) );

    pactionDataSourcesManage = new QAction( QIcon( QPixmap(xpmDisconnected16) ), tr("&Manage Data Sources"), this );
    pactionDataSourcesManage->setStatusTip( tr( "Configure most things ODBC." ) );
    connect( pactionDataSourcesManage, SIGNAL(triggered()), this, SLOT(slotDataSourcesManage()) );

    // statements
    pactionStatementNew = new QAction( QIcon( QPixmap(xpmNew16) ), tr("&New"), this );
    pactionStatementNew->setShortcut( tr( "Ctrl+N" ) );
    pactionStatementNew->setStatusTip( tr( "Create a new statement." ) );
    connect( pactionStatementNew, SIGNAL(triggered()), this, SLOT(slotStatementNew()) );

    pactionStatementClose = new QAction( QIcon( QPixmap(xpmClose16) ), tr("&Close"), this );
    pactionStatementClose->setShortcut( tr( "Ctrl+C" ) );
    pactionStatementClose->setStatusTip( tr( "Close current statement." ) );
    connect( pactionStatementClose, SIGNAL(triggered()), this, SLOT(slotStatementClose()) );

    pactionStatementExecute = new QAction( QIcon( QPixmap(xpmExecute16) ), tr("&Execute"), this );
    pactionStatementExecute->setShortcut( tr( "Ctrl+E" ) );
    pactionStatementExecute->setStatusTip( tr( "Execute current statement." ) );
    connect( pactionStatementExecute, SIGNAL(triggered()), this, SLOT(slotStatementExecute()) );

    // about
    pactionAbout = new QAction( tr( "&About" ), this );
    pactionAbout->setStatusTip( tr( "Show this application's About box" ) );
    connect( pactionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()) );
}

void OQGConsole::createMenus()
{
    // file
    pmenuFile = menuBar()->addMenu( tr( "&File" ) );
    pmenuFile->addAction( pactionQuit );

    // data source
    pmenuDataSource = menuBar()->addMenu( tr( "&Data Source" ) );
    pmenuDataSource->addAction( pactionDataSourceConnect );
    pmenuDataSource->addAction( pactionDataSourceNew );
    pmenuDataSource->addAction( pactionDataSourcesManage );

    // statement
    pmenuStatement = menuBar()->addMenu( tr( "&Statement" ) );
    pmenuStatement->addAction( pactionStatementNew );
    pmenuStatement->addAction( pactionStatementExecute );
    pmenuStatement->addSeparator();
    pmenuStatement->addAction( pactionStatementClose );

    // about
    menuBar()->addSeparator();

    pmenuHelp = menuBar()->addMenu( tr( "&Help" ) );
    pmenuHelp->addAction( pactionAbout );
}

void OQGConsole::createToolBars()
{
    // data source
    ptoolbarDataSource = addToolBar(tr("Data Source"));
    ptoolbarDataSource->addAction( pactionDataSourceConnect );

    // statement
    ptoolbarStatement = addToolBar(tr("Statement"));
    ptoolbarStatement->addAction( pactionStatementExecute );
}

void OQGConsole::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}

void OQGConsole::createClientArea()
{
    pSplitter           = new QSplitter( Qt::Vertical, this );          
    pTabWidget          = new QTabWidget( pSplitter );
    pmessageoutput      = new OQGMessageOutput( pSplitter );

    setCentralWidget( pSplitter );
    resize( 450, 600 );
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

    // so we can change icon on control(s) 
    connect( pConnection, SIGNAL(signalConnected()), this, SLOT(slotConnected()) );
    connect( pConnection, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()) );

    // messages
    connect( pSystem, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pEnvironment, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pConnection, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );

    connect( pSystem, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    connect( pEnvironment, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    connect( pConnection, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
}

void OQGConsole::slotConnectToggle()
{
    if ( pConnection->isConnected() )
        pConnection->doDisconnect();
    else
        pConnection->doConnect( this );
}

void OQGConsole::slotDataSourceNew()
{
    pSystem->doCreateDataSource( this );
}

void OQGConsole::slotDataSourcesManage()
{
    pSystem->doManageDataSources( this );
}

void OQGConsole::slotStatementNew()
{
    /* for when creating a new statement tab
    pStatement      = new OQGStatement( pConnection );
    connect( pStatement, SIGNAL(signalResults(OQStatement*)), this, SLOT(slotResults(OQStatement*)) );
    connect( pStatement, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );
    connect( pStatement, SIGNAL(signalDiagnostic(OQDiagnostic)), pmessageoutput, SLOT(slotDiagnostic(OQDiagnostic)) );
    */
}

void OQGConsole::slotStatementClose()
{
/*
    OQGStatementClient *pClient = (OQGStatementClient*)pTabWidget->currentWidget();
    if ( !pClient )
        return;
*/
}

void OQGConsole::slotStatementExecute() 
{
/*
    OQGStatementClient *pClient = (OQGStatementClient*)pTabWidget->currentWidget();
    if ( !pClient )
        return;

    pClient->slotExecute();
*/
}

void OQGConsole::slotAbout()
{
    QMessageBox MessageBox;

    MessageBox.setText( tr("OQGConsole is a simple tool from the unixODBC-GUI-Qt project.\nIt provides limited, but useful, functionality primarily for testing unixODBC-GUI-Qt.") );
    MessageBox.exec();
}

void OQGConsole::slotConnected()
{
    pactionDataSourceConnect->setIcon( QIcon( QPixmap( xpmConnected16 ) ) );
}

void OQGConsole::slotDisconnected()
{
    pactionDataSourceConnect->setIcon( QIcon( QPixmap( xpmDisconnected16 ) ) );
}


