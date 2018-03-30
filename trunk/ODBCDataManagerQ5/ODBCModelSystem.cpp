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

#include "ODBC64.xpm"

ODBCModelSystem::ODBCModelSystem( OQGSystem *pSystem )
    : ODBCModel( pSystem )
{
    pSysAttr    = ODBCMetaInfo::getSysAttr();
    nRows       = ODBCMetaInfo::getCount( pSysAttr ); // just attr but perhaps add number of active connections row

    setObjectName( "System" );
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
    new ODBCModelDrivers( (OQGSystem*)(getHandle()), this );
    bLoaded = true;
    return true;
}

bool ODBCModelSystem::doClear()
{
    //\todo really get rid of children here

    bLoaded = false;
    return true;
}


