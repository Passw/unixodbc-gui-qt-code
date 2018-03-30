/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDrivers.h"

#include "Drivers48.xpm"

ODBCModelDrivers::ODBCModelDrivers( ODBCModel *pmodelParent )
    : ODBCModel( pmodelParent )
{
    setObjectName( "Drivers" );
}

ODBCModelDrivers::~ODBCModelDrivers()
{
}

QVariant ODBCModelDrivers::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    // no props
    return QVariant();
}

bool ODBCModelDrivers::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    // no props
    return false;
}

QIcon ODBCModelDrivers::getIcon()
{
    return QIcon( QPixmap(xpmDrivers48) );
}

bool ODBCModelDrivers::doLoad()
{
    OQGEnvironment * pEnvironment = getEnvironment();
    if ( !pEnvironment )
        return false;

    QStringList stringlistDrivers = pEnvironment->getDrivers();
    for ( int n = 0; n < stringlistDrivers.count(); n++ )
    {
        new ODBCModelDriver( this, stringlistDrivers.at( n ) );
    }

    bLoaded = true;
    return true;
}

bool ODBCModelDrivers::doClear()
{
    bLoaded = false;
    return true;
}

