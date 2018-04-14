/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelConnection.h"
#include "ODBCModelDriver.h"

#include "ODBC64.xpm"

ODBCModelConnection::ODBCModelConnection( OQGEnvironment *pEnvironment, ODBCModelDriver *pParent )
    : ODBCModel( pEnvironment, pParent )
{
    setObjectName( "Connection" );

    pactionConnect = new QAction( QIcon( QPixmap("") ), "Connect", 0 );
    connect( pactionConnect, SIGNAL(triggered()), this, SLOT(slotConnect()) );
    pactionDisconnect = new QAction( QIcon( QPixmap("") ), "Disconnect", 0 );
    connect( pactionDisconnect, SIGNAL(triggered()), this, SLOT(slotDisconnect()) );
}

ODBCModelConnection::~ODBCModelConnection()
{
}

QVariant ODBCModelConnection::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    return QVariant();
}

bool ODBCModelConnection::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    return false;
}

QIcon ODBCModelConnection::getIcon()
{
    return QIcon( QPixmap( xpmODBC64 ) );
}

bool ODBCModelConnection::doLoad()
{
    bLoaded = true;
    return true;
}

bool ODBCModelConnection::doClear()
{
    bLoaded = false;
    return true;
}

void ODBCModelConnection::doContextMenu( QWidget *pwidgetParent, QPoint pos ) 
{ 
    Q_UNUSED(pwidgetParent); 
    QList<QAction*> listActions;

    listActions.append( pactionConnect );
    listActions.append( pactionDisconnect );

    QAction *pAction = QMenu::exec( listActions, pos );

    Q_UNUSED(pAction)
}

void ODBCModelConnection::slotConnect()
{
    ODBCModelDriver *pDriver    = (ODBCModelDriver*)parent();
    OQGConnection * pConnection = (OQGConnection*)getHandle();
    QString stringIn( "DRIVER=" + pDriver->objectName() );

    /*
    ODBCINSTWND odbcinstwnd;
    strcpy( odbcinstwnd.szUI, "odbcinstQ5" );
    odbcinstwnd.hWnd = (QWidget*)(qApp->desktop());
    */

    SQLRETURN nReturn;
    QString stringConnection = pConnection->getDriverConnect( qApp->desktop(), stringIn, OQConnection::PromptDriverComplete, &nReturn );
}

void ODBCModelConnection::slotDisconnect()
{
}


