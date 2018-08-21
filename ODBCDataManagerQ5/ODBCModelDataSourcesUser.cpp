/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDataSourcesUser.h"

#include "ODBCModelDriver.h"

#include "Drivers48.xpm"

ODBCModelDataSourcesUser::ODBCModelDataSourcesUser( OQGEnvironment *pHandle, ODBCModel *pmodelParent )
    : ODBCModel( pHandle, pmodelParent )
{
    setObjectName( "Data Sources - User" );
}

ODBCModelDataSourcesUser::~ODBCModelDataSourcesUser()
{
}

QVariant ODBCModelDataSourcesUser::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    // no props
    return QVariant();
}

bool ODBCModelDataSourcesUser::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    // no props
    return false;
}

QIcon ODBCModelDataSourcesUser::getIcon()
{
    return QIcon( QPixmap(xpmDrivers48) );
}

bool ODBCModelDataSourcesUser::doLoad()
{
    bLoaded = true;
    return true;
}

bool ODBCModelDataSourcesUser::doClear()
{
    bLoaded = false;
    return true;
}

