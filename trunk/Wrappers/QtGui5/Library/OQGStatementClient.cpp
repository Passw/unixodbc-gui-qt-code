/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "OQGStatementClient.h"

#include "Execute16.xpm"
#include "Connected16.xpm"
#include "Disconnected16.xpm"
#include "ODBC64.xpm"


OQGStatementClient::OQGStatementClient( OQGConnection *pConnection, const QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    pStatement = new OQGStatement( pConnection );
    pStatement->doAlloc();

    // init our gui elements...
    setWindowTitle( QString::fromLocal8Bit("OQGStatementClient") );
    setWindowIcon( QIcon( QPixmap(xpmODBC64) ) );

    createClientArea();
}

OQGStatementClient::~OQGStatementClient()
{
    if ( pStatement ) delete pStatement;
}

void OQGStatementClient::createClientArea()
{
    pSplitter           = new QSplitter( Qt::Vertical, this );          
    ptexteditSQL        = new QTextEdit( pSplitter );
    ptablewidgetResults = new QTableWidget( pSplitter );

    setCentralWidget( pSplitter );
    resize( 450, 600 );
}

void OQGStatementClient::slotExecute() 
{
    // remove table items...
    ptablewidgetResults->clear();
    // remove table cells...
    ptablewidgetResults->setColumnCount( 0 );
    ptablewidgetResults->setRowCount( 0 );

    //
    pStatement->doExecDirect( ptexteditSQL->toPlainText() );

    doResults();
}

void OQGStatementClient::doResults() 
{
    doResultGUIGrid();
}

void OQGStatementClient::doResultGUIGrid()
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

void OQGStatementClient::doResultGUIGridHeader( SWORD nColumns )
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

void OQGStatementClient::doResultGUIGridBody( SWORD nColumns )
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


