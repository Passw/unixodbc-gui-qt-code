/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDataSourcesFile.h"

#include "ODBCModelDriver.h"

#include "Drivers48.xpm"

ODBCModelDataSourcesFile::ODBCModelDataSourcesFile( OQGEnvironment *pHandle, ODBCModel *pmodelParent )
    : ODBCModel( pHandle, pmodelParent )
{
    setObjectName( "Data Sources - File" );
}

ODBCModelDataSourcesFile::~ODBCModelDataSourcesFile()
{
}

QVariant ODBCModelDataSourcesFile::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    // no props
    return QVariant();
}

bool ODBCModelDataSourcesFile::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    // no props
    return false;
}

QIcon ODBCModelDataSourcesFile::getIcon()
{
    return QIcon( QPixmap(xpmDrivers48) );
}

bool ODBCModelDataSourcesFile::doLoad()
{
    bLoaded = true;
    return true;
}

bool ODBCModelDataSourcesFile::doClear()
{
    bLoaded = false;
    return true;
}

