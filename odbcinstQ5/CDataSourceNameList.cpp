/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#include "CDataSourceNameList.h"

#include "CDriverPrompt.h"
#include "CPropertiesDialog.h"

#define ODBC_HELP_DSN_NAME "*Unique* DSN name. This is the name you use when using ODBC with applications such as StarOffice. Try to keep unusual characters and spaces out of the name."
#define ODBC_HELP_DSN_DESC "DSN description. A long, perhaps more meaningfull name."
#define ODBC_HELP_DSN_UNKNOWN "No help for this DSN property. Please check with the vendor of the driver... perhaps their web site"

CDataSourceNameList::CDataSourceNameList( QWidget *pwidgetParent, int nSource )
	: QTableWidget( pwidgetParent )
{
    this->nSource = nSource;

    setToolTip( tr( "list of data source names" ) );
    setWhatsThis( tr( "This shows a list of the data source names. Data source names (DSN's) are a convenient way for an application to connect to a data source as the options can be specified in advance, saved, and then used at connect-time by simply referring to the name. Applications which use a DSN to connect also provide a means for the User to easily specify a different data source - simply create the DSN with a same/different driver and options." ) );
    QStringList stringlist;
    setColumnCount( 3 );
    setHorizontalHeaderLabels( stringlist << tr( "Name" ) << tr( "Description" ) << tr( "Driver" ) );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    verticalHeader()->setVisible( false );

    connect( this, SIGNAL(itemDoubleClicked( QTableWidgetItem * )), this, SLOT(slotDoubleClick( QTableWidgetItem * )));

    slotLoad();
}

CDataSourceNameList::~CDataSourceNameList()
{
}

QString CDataSourceNameList::getDataSourceName()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
    {
        int nRow = row( listSelectedItems.at( 0 )  );
        return item( nRow, 0 )->text();
    }

    return QString();
}

QString CDataSourceNameList::getDescription()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
    {
        int nRow = row( listSelectedItems.at( 0 )  );
        return item( nRow, 1 )->text();
    }

    return QString();
}

void CDataSourceNameList::slotAdd()
{
    // odbc.ini INFO
    QString				stringDataSourceName( tr("") );
    QString				stringDataSourceDescription( tr("") );
    QString				stringDataSourceDriver( tr("") );
    // odbcinst.ini INFO
    QString				stringDriverName( tr("") );
    QString				stringDriverDescription( tr("") );
    QString				stringDriverFile( tr("") );
    QString				stringSetupFile( tr("") );

    CDriverPrompt	 *  pDriverPrompt;
    CPropertiesDialog * pProperties;
    HODBCINSTPROPERTY	hFirstProperty	= NULL;
    HODBCINSTPROPERTY	hCurProperty	= NULL;
    char				szINI[FILENAME_MAX+1];

    pDriverPrompt = new CDriverPrompt( this );
    if ( pDriverPrompt->exec() )
    {
        stringDriverName		= pDriverPrompt->getFriendlyName();
        stringDriverDescription	= pDriverPrompt->getDescription();
        stringDriverFile		= pDriverPrompt->getDriver();
        stringSetupFile			= pDriverPrompt->getSetup();
        stringDataSourceDriver	= stringDriverName;
        delete pDriverPrompt;

        //
        // can we call SQLConfigDataSource ?
        //

        {
            int mode;

            if( nSource == ODBC_USER_DSN )
            {
                sprintf( szINI, "~/.odbc.ini" );
                mode = ODBC_ADD_DSN;
            }
            else
            {
				char b1[ 256 ];
                sprintf( szINI, "%s/odbc.ini", odbcinst_system_file_path( b1 ));
                mode = ODBC_ADD_SYS_DSN;
            }
            if (SQLConfigDataSource((HWND)1, mode, stringDataSourceDriver.toLatin1().constData(), ""))
            {
                slotLoad();
                return;
            }
        }

        // GET PROPERTY LIST FROM DRIVER
        if ( ODBCINSTConstructProperties( stringDataSourceDriver.toLatin1().data(), &hFirstProperty ) != ODBCINST_SUCCESS )
        {
            QMessageBox::information( this, tr( "ODBC Administrator" ), QString( tr( "Could not construct a property list for (%1)" ) ).arg( stringDataSourceDriver ) );
            return;
        }

        // ALLOW USER TO EDIT
        pProperties = new CPropertiesDialog( this, hFirstProperty );
        pProperties->setWindowTitle( tr( "Data Source Properties (new)" ) );
        if ( pProperties->exec() )
        {
            /* DELETE ENTIRE SECTION IF IT EXISTS (no entry given) */
            SQLSetConfigMode( nSource );
            if ( SQLWritePrivateProfileString( hFirstProperty->szValue, NULL, NULL, "odbc.ini" ) == FALSE )
            {
                SQLSetConfigMode( ODBC_BOTH_DSN );
                delete pProperties;
                ODBCINSTDestructProperties( &hFirstProperty );
                CODBCInst::showErrors( this, QString( tr("Could not write to (%1)") ).arg( QString::fromLocal8Bit( szINI ) ) );
                return;
            }
            stringDataSourceName = QString::fromLocal8Bit( hFirstProperty->szValue );
            /* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
            QString stringName;
            for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
            {
                stringName = QString::fromLocal8Bit( hCurProperty->szName );
                if ( stringName.toUpper() == tr( "DESCRIPTION" ) )
                    stringDataSourceDescription = QString::fromLocal8Bit( hCurProperty->szValue );

                SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, "odbc.ini" );
            }
            SQLSetConfigMode( ODBC_BOTH_DSN );
        }
        delete pProperties;
        ODBCINSTDestructProperties( &hFirstProperty );
    }
    else
        delete pDriverPrompt;

    // RELOAD (slow but safe)
    slotLoad();
}

void CDataSourceNameList::slotEdit()
{
	// odbc.ini INFO
	QString				stringDataSourceName( tr("") );
	QString				stringDataSourceDescription( tr("") );
	QString				stringDataSourceDriver( tr("") );
	// odbcinst.ini INFO
	QString				stringDriverFile( tr("") );
	QString				stringSetupFile( tr("") );
	QString				stringError( tr("") );

	CPropertiesDialog *  pProperties;
	HODBCINSTPROPERTY	hFirstProperty	= NULL;
	HODBCINSTPROPERTY	hCurProperty	= NULL;

	char				szEntryNames[4096];
	char				szProperty[INI_MAX_PROPERTY_NAME+1];
	char				szValue[INI_MAX_PROPERTY_VALUE+1];
	
//	DWORD				nErrorCode;
//	char				szErrorMsg[101];
	char				szINI[FILENAME_MAX+1];
	int					nElement;	

	// HAS THE USER SELECTED SOMETHING
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
    {
        int nRow = row( listSelectedItems.at( 0 )  );
        stringDataSourceName		= item( nRow, 0 )->text();
        stringDataSourceDescription	= item( nRow, 1 )->text();
        stringDataSourceDriver		= item( nRow, 2 )->text();
    }
    else
    {
        QMessageBox::information( this, tr( "ODBC Administrator" ),  tr( "Please select a Data Source from the list first" ) );
        return;
    }

    //
    // can we call SQLConfigDataSource ?
    //

    {
        char attr[ 128 ];
        int mode;

        sprintf( attr, "DSN=%s", stringDataSourceName.toLatin1().data() );

		/*
		 * add extra null
		 */

		attr[ strlen( attr ) ] = '\0';

        if( nSource == ODBC_USER_DSN )
        {
            sprintf( szINI, "~/.odbc.ini" );
            mode = ODBC_CONFIG_DSN;
        }
        else
        {
			char b1[ 256 ];
            sprintf( szINI, "%s/odbc.ini", odbcinst_system_file_path( b1 ));
            mode = ODBC_CONFIG_SYS_DSN;
        }

        if ( SQLConfigDataSource(( HWND ) 1, mode,
            stringDataSourceDriver.toLatin1().data(), attr ))
        {
			SQLSetConfigMode( ODBC_BOTH_DSN );
            slotLoad();
            return;
        }
        SQLSetConfigMode( ODBC_BOTH_DSN );
    }

	// GET PROPERTY LIST FROM DRIVER
	if ( ODBCINSTConstructProperties( (char*) stringDataSourceDriver.toLatin1().data(), &hFirstProperty ) != ODBCINST_SUCCESS )
	{
        CODBCInst::showErrors( this, QString( tr( "Could not construct a property list for (%1)" ) ).arg( stringDataSourceDriver ) );
		return;
	}

	// COPY EXISTING VALUES INTO PROPERTIES LIST
	SQLSetConfigMode( nSource );
    ODBCINSTSetProperty( hFirstProperty, QString( tr( "Name" ) ).toLocal8Bit().data(), stringDataSourceName.toLocal8Bit().data() );
    memset( szEntryNames, 0, sizeof( szEntryNames ));
	SQLGetPrivateProfileString( stringDataSourceName.toLocal8Bit().data(), NULL, NULL, szEntryNames, 4090, "odbc.ini" ); // GET ALL ENTRY NAMES FOR THE SELCTED DATA SOURCE
	for ( nElement = 0; iniElement( szEntryNames, '\0', '\0', nElement, szProperty, 1000 ) == INI_SUCCESS ; nElement++ )
	{
		SQLGetPrivateProfileString( stringDataSourceName.toLocal8Bit().data(), szProperty, "", szValue, INI_MAX_PROPERTY_VALUE, szINI ); // GET VALUE FOR EACH ENTRY

		if ( ODBCINSTSetProperty( hFirstProperty, szProperty, szValue ) == ODBCINST_ERROR )
        {
		    ODBCINSTAddProperty( hFirstProperty, szProperty, szValue );
        }
	}
	SQLSetConfigMode( ODBC_BOTH_DSN );

	// ALLOW USER TO EDIT
	pProperties = new CPropertiesDialog( this, hFirstProperty );
	pProperties->setWindowTitle( tr( "Data Source Properties (edit)" ) );
	if ( pProperties->exec() )
	{
		SQLSetConfigMode( nSource );
		/* DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry) */
		if ( SQLWritePrivateProfileString( stringDataSourceName.toLatin1().data(), NULL, NULL, "odbc.ini" ) == FALSE )
		{
			SQLSetConfigMode( ODBC_BOTH_DSN );
			delete pProperties;
			ODBCINSTDestructProperties( &hFirstProperty );
			CODBCInst::showErrors( this, QString( tr( "Could not write to (%1)" ) ).arg( QString::fromLocal8Bit( szINI ) ) );
			return;
		}
        stringDataSourceName = QString::fromLocal8Bit( hFirstProperty->szValue );
		/* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
        QString stringName;
		for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
		{
            stringName = QString::fromLocal8Bit( hCurProperty->szName );
			if ( stringName.toUpper() == tr( "DESCRIPTION" ) )
				stringDataSourceDescription = QString::fromLocal8Bit( hCurProperty->szValue );

			SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, szINI );
		}
		SQLSetConfigMode( ODBC_BOTH_DSN );
	}
	delete pProperties;
	ODBCINSTDestructProperties( &hFirstProperty );

	// RELOAD (slow but safe)
	slotLoad();
}

void CDataSourceNameList::slotDelete()
{
	QString stringDataSourceName;
	QString	stringError;
//	DWORD	nErrorCode;
//	char	szErrorMsg[FILENAME_MAX+1];

	// GET SELECT DATA SOURCE NAME
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
	{
        int nRow = row( listSelectedItems.at( 0 ) );
		stringDataSourceName = item( nRow, 0 )->text();
	}
	else
	{
		QMessageBox::information( this, tr( "ODBC Administrator" ),  tr( "Please select a Data Source from the list first" ) );
		return;
	}

	// DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry)
	SQLSetConfigMode( nSource );
	if ( SQLWritePrivateProfileString( stringDataSourceName.toLocal8Bit().data(), NULL, NULL, "odbc.ini" ) == FALSE )
		CODBCInst::showErrors( this, QString( tr( "Could not write property list for (%1)" ) ).arg( stringDataSourceName ) );

	SQLSetConfigMode( ODBC_BOTH_DSN );
	
	// RELOAD (slow but safe)
	slotLoad();
}

void CDataSourceNameList::slotLoad()
{
	QString         stringError;
//	DWORD	        nErrorCode;
//	char   	        szErrorMsg[101];
	char	        szSectionNames[4096];
	char	        szSectionName[INI_MAX_OBJECT_NAME+1];
	char	        szDriver[INI_MAX_PROPERTY_VALUE+1];
	char	        szDescription[INI_MAX_PROPERTY_VALUE+1];
	int		        nElement;

    setRowCount( 0 );

    // GET SECTION NAMES (Data Sources)
	memset( szSectionNames, 0, sizeof(szSectionNames) );
	SQLSetConfigMode( nSource );
	if ( SQLGetPrivateProfileString( NULL, NULL, NULL, szSectionNames, 4090, "odbc.ini" ) >= 0 )
	{
        for ( nElement = 0; iniElement( szSectionNames, '\0', '\0', nElement, szSectionName, INI_MAX_OBJECT_NAME ) == INI_SUCCESS ; nElement++ )
		{
			// GET DRIVER AND DESCRIPTION
			szDriver[0]		= '\0';
			szDescription[0]	= '\0';
#ifdef PLATFORM64
			SQLGetPrivateProfileString( szSectionName, "Driver64", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
			if ( szDriver[ 0 ] == '\0' )
			{
				SQLGetPrivateProfileString( szSectionName, "Driver", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
			}
#else
			SQLGetPrivateProfileString( szSectionName, "Driver", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
#endif
			SQLGetPrivateProfileString( szSectionName, "Description", "", szDescription, INI_MAX_PROPERTY_VALUE, "odbc.ini" );

            QTableWidgetItem *ptablewidgetitem;
            setRowCount( nElement + 1 );

            ptablewidgetitem = new QTableWidgetItem( QString::fromLocal8Bit( szSectionName ) );
//            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 0, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( QString::fromLocal8Bit( szDescription ) );
//            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 1, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( QString::fromLocal8Bit( szDriver ) );
//            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 2, ptablewidgetitem );
		}
		SQLSetConfigMode( ODBC_BOTH_DSN );
	}
	else
	{
        SQLSetConfigMode( ODBC_BOTH_DSN );
		CODBCInst::showErrors( this, QString( tr( "Could not load odbc.ini" ) ) );
	}
}

void CDataSourceNameList::slotDoubleClick( QTableWidgetItem * )
{
    slotEdit();
}

