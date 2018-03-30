/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelSystem.h"

// children we support
#include "ODBCModelEnvironment.h"

#include "ODBC64.xpm"

ODBCModelSystem::ODBCModelSystem( OQGSystem *pHandle )
    : ODBCModel( pHandle )
{
    pSysAttr    = ODBCMetaInfo::getSysAttr();
    nRows       = ODBCMetaInfo::getCount( pSysAttr ); // just attr but perhaps add number of active connections row

    setObjectName( "System" );

    pactionNewEnvironment = new QAction( QIcon( QPixmap("") ), "New Environment", 0 );
    connect( pactionNewEnvironment, SIGNAL(triggered()), this, SLOT(slotNewEnvironment()) );
}

ODBCModelSystem::~ODBCModelSystem()
{
}

QVariant ODBCModelSystem::data( const QModelIndex &index, int nRole ) const
{
    if ( index.row() < 0 || index.row() >= nRows )
        return QVariant();

    // get metainfo regarding the attribute we are working on...
    ODBCMetaInfoAttr *pAttr = &(pSysAttr[ index.row() ]);

    /* Name column... */
    if ( index.column() == 0 )
    {
        switch ( nRole )
        {
            case Qt::DisplayRole:
                return pAttr->pszCasualName;
        }
    }

    /* Value column... */
    return QVariant();
}

bool ODBCModelSystem::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(variantValue)

    if ( !index.isValid() )
        return false;

    if ( index.column() != 1 )
        return false;

    if ( index.row() < 0 || index.row() >= nRows )
        return false;

    if ( nRole == Qt::EditRole )
    {
        emit dataChanged( index, index );
        return true;
    }

    return false;
}

QIcon ODBCModelSystem::getIcon()
{
    return QIcon( QPixmap(xpmODBC64) );
}

bool ODBCModelSystem::doLoad()
{
    // create mandatory children (not optional children)
    // no mandatory children at this time
    bLoaded = true;
    return true;
}

bool ODBCModelSystem::doClear()
{
    // clear mandatory and optional children here
    bLoaded = false;
    return true;
}

void ODBCModelSystem::doContextMenu( QWidget *pwidgetParent, QPoint pos )
{
    Q_UNUSED(pwidgetParent)

    QList<QAction*> listActions;

    listActions.append( pactionNewEnvironment );

    QAction *pAction = QMenu::exec( listActions, pos );

    Q_UNUSED(pAction)
}

void ODBCModelSystem::slotNewEnvironment()
{
    OQGSystem *pSystem = (OQGSystem*)(getHandle());
    OQGEnvironment *pEnvironment = new OQGEnvironment( pSystem );
    new ODBCModelEnvironment( pEnvironment, this );
}

