/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelEnvironment.h"
#include "ODBCModelSystem.h"
#include "ODBCModelDrivers.h"
#include "ODBCModelDataSourcesSystem.h"
#include "ODBCModelDataSourcesUser.h"
#include "ODBCModelDataSourcesFile.h"

#include "ODBC64.xpm"

ODBCModelEnvironment::ODBCModelEnvironment( OQGEnvironment *pEnvironment, ODBCModelSystem *pParent )
    : ODBCModel( pEnvironment, pParent )
{
    pEnvAttr    = ODBCMetaInfo::getEnvAttr();
    nRows       = ODBCMetaInfo::getCount( pEnvAttr ); // just attr but perhaps add number of active connections row

    setObjectName( "Environment" );

    pactionAlloc = new QAction( QIcon( QPixmap("") ), "Alloc", 0 );
    connect( pactionAlloc, SIGNAL(triggered()), this, SLOT(slotAlloc()) );
    pactionFree = new QAction( QIcon( QPixmap("") ), "Free", 0 );
    connect( pactionFree, SIGNAL(triggered()), this, SLOT(slotFree()) );
    pactionDelete = new QAction( QIcon( QPixmap("") ), "Delete", 0 );
    connect( pactionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()) );
}

ODBCModelEnvironment::~ODBCModelEnvironment()
{
}

QVariant ODBCModelEnvironment::data( const QModelIndex &index, int nRole ) const
{
    OQGEnvironment *pEnvironment = (OQGEnvironment *)pHandle;

    if ( index.row() < 0 || index.row() >= nRows )
        return QVariant();

    // get metainfo regarding the attribute we are working on...
    ODBCMetaInfoAttr *pAttr = &(pEnvAttr[ index.row() ]);

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
    if ( index.column() == 1 )
    {
        switch ( nRole )
        {
            case Qt::DisplayRole:
            {
                // get current value by getting the Numeric from the system and then xref to a CasualName
                ODBCMetaInfoAttrValue *p = NULL;

                switch ( index.row() )
                {
                    case 0:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrConnectionPooling() );
                        break;
                    case 1:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrCPMatch() );
                        break;
                    case 2:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrODBCVersion() );
                        break;
                    case 3:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrOutputNTS() );
                        break;
                }
                if ( p ) 
                    return p->pszCasualName;
                break;
            }
            case Qt::EditRole:      /* called by CPropertiesDelegate:: createEditor & setEditorData to load cell editor */
            {
                // get metainfo for the current Numeric value...
                ODBCMetaInfoAttrValue * p = NULL; // current value
                ODBCValueCell ValueCell;

                switch ( index.row() )
                {
                    case 0:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrConnectionPooling() );
//                        ValueCell.nPromptType = ODBCValueCell::PromptTypeList;
                        break;
                    case 1:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrCPMatch() );
//                        ValueCell.nPromptType = ODBCValueCell::PromptTypeList;
                        break;
                    case 2:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrODBCVersion() );
//                        ValueCell.nPromptType = ODBCValueCell::PromptTypeList;
                        break;
                    case 3:
//                        p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrOutputNTS() );
//                        ValueCell.nPromptType = ODBCValueCell::PromptTypeBool;
                        break;
                }
                if ( !p )
                    break;

                // describe the value and prompting 
/*
                ValueCell.stringDescription         = pAttr->pszDescription;
                ValueCell.vValue                    = p->pszCasualName;
                ValueCell.vValueOriginal            = p->pszCasualName;
                ValueCell.vectorValues              = ODBCMetaInfo::getAttrValues( pAttr, ODBCMetaInfo::AttrFieldCasualName );
                ValueCell.vectorDescriptions        = ODBCMetaInfo::getAttrValues( pAttr, ODBCMetaInfo::AttrFieldDescription );
                ValueCell.vValueDefault             = ValueCell.vectorValues.at( 0 );
                if ( pAttr->bReadOnly )
                    ValueCell.nPromptType = ODBCValueCell::PromptTypeLabel;

                return QVariant::fromValue( ValueCell );
*/
            }
        }
    }

    // tip
/*
    switch ( nRole )
    {
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
        {
            ODBCMetaInfoAttrValue * p = NULL;
            switch ( index.row() )
            {
                case 0:
                    p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrConnectionPooling() );
                    break;
                case 1:
                    p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrCPMatch() );
                    break;
                case 2:
                    p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrODBCVersion() );
                    break;
                case 3:
                    p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldNumeric, pEnvironment->getAttrOutputNTS() );
                    break;
            }
            if ( !p )
                break;
            return QVariant( p->pszDescription );
        }
        break;
    }
*/

    return QVariant();
}

bool ODBCModelEnvironment::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    OQGEnvironment *pEnvironment = (OQGEnvironment*)(getHandle());

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

        ODBCMetaInfoAttr *pAttr = &(pEnvAttr[ index.row() ]);

        // variantValue has CasualName - xref to numeric
        ODBCMetaInfoAttrValue *p = ODBCMetaInfo::getAttrValue( pAttr, ODBCMetaInfo::AttrFieldCasualName, ValueCell.vValue.toString().toLatin1().constData() );
        if ( !p )
            return false;

        SQLRETURN nReturn = SQL_ERROR;
        switch ( index.row() )
        {
            case 0:
                nReturn = pEnvironment->setAttrConnectionPooling( (OQEnvironment::AttrConnectionPoolingTypes)p->Numeric.nUnsignedInteger );
                break;
            case 1:
                nReturn = pEnvironment->setAttrCPMatch( (OQEnvironment::AttrCPMatchTypes)p->Numeric.nUnsignedInteger );
                break;
            case 2:
                nReturn = pEnvironment->setAttrODBCVersion( (OQGEnvironment::AttrODBCVersionTypes)p->Numeric.nInteger );
                break;
            case 3:
                nReturn = pEnvironment->setAttrOutputNTS( (bool)p->Numeric.nSmallInteger );
                break;
        }

        if ( !SQL_SUCCEEDED( nReturn ) )
            return false;

        emit dataChanged( index, index );
        return true;
    }

    return false;
}

QIcon ODBCModelEnvironment::getIcon()
{
    return QIcon( QPixmap( xpmODBC64 ) );
}

bool ODBCModelEnvironment::doLoad()
{
    // lazy load
    // load mandatory children
    OQGEnvironment *pEnvironment = getEnvironment();
    if ( !pEnvironment->isAlloc() )
    {
        QMessageBox::information( 0, tr("Load"), tr("Right-Click with mouse to Alloc handle with current properties.") );
        return false;
    }

    new ODBCModelDrivers( (OQGEnvironment*)(getHandle()), this );
    new ODBCModelDataSourcesSystem( (OQGEnvironment*)(getHandle()), this );
    new ODBCModelDataSourcesUser( (OQGEnvironment*)(getHandle()), this );
    new ODBCModelDataSourcesFile( (OQGEnvironment*)(getHandle()), this );
    bLoaded = true;

    return true;
}

bool ODBCModelEnvironment::doClear()
{
    bLoaded = false;
    return true;
}

void ODBCModelEnvironment::doContextMenu( QWidget *pwidgetParent, QPoint pos ) 
{ 
    Q_UNUSED(pwidgetParent); 
    QList<QAction*> listActions;

    listActions.append( pactionAlloc );
    listActions.append( pactionFree );
    listActions.append( pactionDelete );

    QAction *pAction = QMenu::exec( listActions, pos );

    Q_UNUSED(pAction)
}

void ODBCModelEnvironment::slotAlloc()
{
    // alloc underlying (SQLHENV) handle
    OQGEnvironment *pEnvironment = getEnvironment();
    if ( pEnvironment->isAlloc() )
        return;

    pEnvironment->doAlloc();
}

void ODBCModelEnvironment::slotFree()
{
    // free underlying (SQLHENV) handle
    OQGEnvironment *pEnvironment = getEnvironment();
    if ( !pEnvironment->isAlloc() )
        return;

    pEnvironment->doFree();
}

void ODBCModelEnvironment::slotDelete()
{
    // delete ourself
}

