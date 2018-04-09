/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "OQGMessageOutput.h"

#include <OQMessage.h>
#include <OQDiagnostic.h>

#include <OQGSystem.h>

#include "Information16.xpm"
#include "Warning16.xpm"
#include "Error16.xpm"

OQGMsgOutput::OQGMsgOutput( QWidget *pwidgetParent )
    : QTableWidget( 0, 4, pwidgetParent )
{
    QStringList stringlistHeaderLabels;

    setColumnCount( 4 );
    setHorizontalHeaderLabels( stringlistHeaderLabels << tr( "Severity" ) << tr( "Request" ) << tr( "Text" ) << tr( "Numeric" ) );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    verticalHeader()->setVisible( false );

    nWaitingMessages = 0;
}

OQGMsgOutput::~OQGMsgOutput()
{
}

void OQGMsgOutput::setWaitingMessages( int n ) 
{ 
    nWaitingMessages = n; 
    emit signalWaitingMessages( this );
}


/*!
    slotMessage
    
    Append the message to our list of messages.
*/
void OQGMsgOutput::slotMessage( OQMessage Message )
{
    int nRows = rowCount() + 1;
    int nRow  = nRows - 1;

    // add row
    setRowCount( nRows );

    // column 1
    switch ( Message.getType() )
    {
        case OQMessage::Info:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( QPixmap( xpmInformation16 ) ), QString::fromLocal8Bit("") ) );
            break;
        case OQMessage::Error:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( QPixmap( xpmError16 ) ), QString::fromLocal8Bit("") ) );
            break;
        case OQMessage::Warning:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( QPixmap( xpmWarning16 ) ), QString::fromLocal8Bit("") ) );
            break;
    }

    // column 2
    setItem( nRow, 1, new QTableWidgetItem( Message.getRequest() ) );

    // column 3
    setItem( nRow, 2, new QTableWidgetItem( Message.getText() ) );

    // column 4
    setItem( nRow, 3, new QTableWidgetItem( QString::number( Message.getNumeric() ) ) );

    nWaitingMessages++;
    emit signalWaitingMessages( this );
}

void OQGMsgOutput::slotClear()
{
    clearContents();
}




OQGDiagOutput::OQGDiagOutput( QWidget *pwidgetParent )
    : QTreeWidget( pwidgetParent )
{
    setRootIsDecorated( true );
    QStringList stringlistLabels;
    stringlistLabels << tr("Return Code") << tr("Message Text") << tr("Cursor Row Count") << tr("Dynamic Function") << tr("Dyanmic Function Code") << tr("Number") << tr("Row Count") << tr("Class Origin") << tr("Column Number") << tr("Connection Name") << tr("Native") << tr("Row Number") << tr("Server Name") << tr("SQL State") << tr("Sub Class Origin");
    setHeaderLabels( stringlistLabels );

    nWaitingMessages = 0;
}

OQGDiagOutput::~OQGDiagOutput()
{
}

void OQGDiagOutput::setWaitingMessages( int n ) 
{ 
    nWaitingMessages = n; 
    emit signalWaitingMessages( this );
}

void OQGDiagOutput::slotClear()
{
    clear();
}

void OQGDiagOutput::slotDiagnostic( OQDiagnostic diagnostic )
{
    QTreeWidgetItem *pitemParent = new QTreeWidgetItem( this );
    SQLRETURN n = diagnostic.getReturncode();

    if ( n == SQL_ERROR )
        pitemParent->setIcon( 0, QIcon( QPixmap( xpmError16 ) ) );
    else if ( n == SQL_SUCCESS_WITH_INFO )
        pitemParent->setIcon( 0, QIcon( QPixmap( xpmWarning16 ) ) );
    else
        pitemParent->setIcon( 0, QIcon( QPixmap( xpmInformation16 ) ) );

    pitemParent->setText( 2, QString().arg(diagnostic.getCursorRowCount()) );
    pitemParent->setText( 3, diagnostic.getDynamicFunction() );
    pitemParent->setText( 4, QString().arg(diagnostic.getDynamicFunctionCode()) );
    SQLINTEGER nRecords = diagnostic.getNumber();
    pitemParent->setText( 5, QString().arg(nRecords) );
    pitemParent->setText( 6, QString().arg(diagnostic.getRowCount()) );

    for ( SQLINTEGER nRecord = 1; nRecord <= nRecords; nRecord++ )
    {
        OQDiagnosticRecord  record      = diagnostic.getRecord( nRecord );
        QTreeWidgetItem *   pitemChild  = new QTreeWidgetItem( pitemParent );

        pitemChild->setText( 1, record.getMessageText() );
        pitemChild->setText( 7, record.getClassOrigin() );
        pitemChild->setText( 8, QString().arg(record.getColumnNumber()) );
        pitemChild->setText( 9, record.getConnectionName() );
        pitemChild->setText( 10, QString().arg(record.getNative()) );
        pitemChild->setText( 11, QString().arg(record.getRowNumber()) );
        pitemChild->setText( 12, record.getServerName() );
        pitemChild->setText( 13, record.getSqlstate() );
        pitemChild->setText( 14, record.getSubclassOrigin() );

        nWaitingMessages++;
    }

    pitemParent->setExpanded( true );

    emit signalWaitingMessages( this );
}

OQGTabOutput::OQGTabOutput( QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    pDiagOutput = new OQGDiagOutput();
    pMsgOutput = new OQGMsgOutput();
    addTab( pDiagOutput, tr("Diagnostics") );
    addTab( pMsgOutput, tr("Messages") );
}

OQGTabOutput::~OQGTabOutput()
{
    clear();
    delete pDiagOutput;
    delete pMsgOutput;
}


