/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDataSourcesSystem.h"

#include "ODBCModelDriver.h"

#include "Drivers48.xpm"

ODBCModelDataSourcesSystem::ODBCModelDataSourcesSystem( OQGEnvironment *pHandle, ODBCModel *pmodelParent )
    : ODBCModel( pHandle, pmodelParent )
{
    setObjectName( "Data Sources - System" );
}

ODBCModelDataSourcesSystem::~ODBCModelDataSourcesSystem()
{
}

QVariant ODBCModelDataSourcesSystem::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    // no props
    return QVariant();
}

bool ODBCModelDataSourcesSystem::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    // no props
    return false;
}

QIcon ODBCModelDataSourcesSystem::getIcon()
{
    return QIcon( QPixmap(xpmDrivers48) );
}

bool ODBCModelDataSourcesSystem::doLoad()
{
    bLoaded = true;
    return true;
}

bool ODBCModelDataSourcesSystem::doClear()
{
    bLoaded = false;
    return true;
}

