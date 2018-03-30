/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDriver.h"
#include "ODBCModelDrivers.h"

#include "Connected48.xpm"
#include "Remove32.xpm"
#include "Driver48.xpm"

ODBCModelDriver::ODBCModelDriver( OQGEnvironment *pHandle, ODBCModelDrivers *pParent, const QString &stringDriver )
    : ODBCModel( pHandle, pParent )
{
    setObjectName( stringDriver );

    pactionConnect = new QAction( QIcon( QPixmap( xpmConnected48 ) ), "Connect", 0 );
    connect( pactionConnect, SIGNAL(triggered()), this, SLOT(slotConnect()) );

    pactionDelete = new QAction( QIcon( QPixmap( xpmRemove32 ) ), "Delete", 0 );
    connect( pactionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()) );

    doLoadProperties();
}

ODBCModelDriver::~ODBCModelDriver()
{
}

QVariant ODBCModelDriver::data( const QModelIndex &index, int nRole ) const
{
    if ( index.row() < 0 || index.row() >= nRows )
        return QVariant();

    /* Name/Key column... */
    if ( index.column() == 0 )
    {
        switch ( nRole )
        {
            case Qt::DisplayRole:
                return vectorKeys.value( index.row() );
        }
    }

    /* Value column... */
    if ( index.column() == 1 )
    {
        switch ( nRole )
        {
            case Qt::DisplayRole:
                return vectorValues.value( index.row() );
            case Qt::EditRole:
            {
                /*! \todo interp known keywords for enhanced prompting */
                ODBCValueCell ValueCell;
                ValueCell.vValue            = vectorValues.value( index.row() );
                ValueCell.vValueOriginal    = vectorValues.value( index.row() );
                ValueCell.vValueDefault     = vectorValues.value( index.row() );
                ValueCell.nPromptType       = ODBCValueCell::PromptTypeText;
                return QVariant::fromValue( ValueCell );
            }
        }
    }

    return QVariant();
}

bool ODBCModelDriver::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    if ( !index.isValid() )
        return false;

    if ( index.column() != 1 )
        return false;

    if ( index.row() < 0 || index.row() >= nRows )
        return false;

    if ( nRole == Qt::EditRole )
    {
        ODBCValueCell ValueCell = variantValue.value<ODBCValueCell>();
        if ( ValueCell.vValue == ValueCell.vValueOriginal )
            return false;

        // update attribute in sys info
        if ( !doSaveProperty( vectorKeys.at( index.row() ), ValueCell.vValue.toString() ) )
            return false;

        // update attribute in our cache...
        vectorValues[index.row()] = ValueCell.vValue.toString();

        emit dataChanged( index, index );
        return true;
    }

    return false;
}

QIcon ODBCModelDriver::getIcon()
{
    return QIcon( QPixmap( xpmDriver48 ) );
}

bool ODBCModelDriver::doLoad()
{
    // no children (yet)
    bLoaded = true;

    return true;
}

bool ODBCModelDriver::doClear()
{
    bLoaded = false;
    return true;
}

void ODBCModelDriver::doContextMenu( QWidget *pwidgetParent, QPoint pos )
{
    Q_UNUSED(pwidgetParent)

    QList<QAction*> listActions;

    listActions.append( pactionConnect );
    listActions.append( pactionDelete );

    QAction *pAction = QMenu::exec( listActions, pos );

    Q_UNUSED(pAction)
}

void ODBCModelDriver::slotConnect()
{
//    DNodeConnection *pConnection = new DNodeConnection( pNodeWidget, this );
//    setExpanded( true );
printf( "[PAH][%s][%d][%s]\n", __FILE__, __LINE__, __FUNCTION__ );
}

void ODBCModelDriver::slotDelete()
{
printf( "[PAH][%s][%d][%s]\n", __FILE__, __LINE__, __FUNCTION__ );
//    delete this;
}

void ODBCModelDriver::doLoadProperties()
{
    nRows = 0;
    vectorKeys.clear();    
    vectorValues.clear();

    OQGSystem *pSystem = (OQGSystem *)(getHandle()->getParent( OQHandle::Sys ));

    // get all attributes...
    OQDriver Driver = pSystem->getDriver( getText() );

    // translate to vectors since we only want access via index (row number)...
    QMap<QString,QString>::const_iterator i = Driver.mapAttributes.constBegin();
    while ( i != Driver.mapAttributes.constEnd() ) 
    {
        vectorKeys.append( i.key() );
        vectorValues.append( i.value() );
        ++i;
    }

    nRows = vectorKeys.count();
}

bool ODBCModelDriver::doSaveProperty( const QString &stringKey, const QString &stringValue )
{
    OQGSystem *pSystem = (OQGSystem *)(getHandle()->getParent( OQHandle::Sys ));
    SQLRETURN nReturn = pSystem->setDriverAttribute( getText(), stringKey, stringValue );

    return (SQL_SUCCEEDED(nReturn));
}

