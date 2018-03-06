/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 1
 * \date    2009
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#include "CDriverConnectPrompt.h"
#include "CDataSourceNames.h"
#include "CDataSourceNamesFile.h"
#include "CPage.h"

CDriverConnectPrompt::CDriverConnectPrompt( const QString &stringIn, QWidget *pwidgetParent )
    : QDialog( pwidgetParent )
{
    Q_UNUSED(stringIn)

    QVBoxLayout *playout00 = new QVBoxLayout;

    //
    pManageDataSourceNames  = new CManageDataSourceNames;
    playout00->addWidget( pManageDataSourceNames );

    // init dialog buttons...
    {
        QDialogButtonBox *  pdialogbuttonbox    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help );

        connect( pdialogbuttonbox, SIGNAL(accepted()), this, SLOT(slotAccept()) );
        connect( pdialogbuttonbox, SIGNAL(rejected()), this, SLOT(reject()) );
//        connect( pdialogbuttonbox, SIGNAL(helpRequested()), this, SLOT(slotHelp()) );

        playout00->addWidget( pdialogbuttonbox );
    }

    // 
    setLayout( playout00 );
    setWindowTitle( tr( "Select Data Source..." ) );
    setWindowIcon( pManageDataSourceNames->windowIcon() );

    // restore last state...
    loadState();
}

CDriverConnectPrompt::~CDriverConnectPrompt()
{
    saveState();
}

void CDriverConnectPrompt::slotAccept()
{
    QWidget *pwidgetPage = pManageDataSourceNames->currentWidget();

    // sanity checks...
    if ( !pwidgetPage )
        return;

    if ( pwidgetPage->inherits( "CPage" ) )
        return;

    // get data source name...
    QWidget *pwidgetContent = ((CPage*)pwidgetPage)->getContent();

    if ( pwidgetContent->inherits( "CDataSourceNames" ) )
    {
        CDataSourceNames *pDataSourceNames = (CDataSourceNames*)pwidgetContent;
        stringConnectString = pDataSourceNames->getDataSourceName();
        if ( !stringConnectString.isEmpty() )
            stringConnectString = QString::fromLocal8Bit( "DSN=" ) + stringConnectString;
    }
    else if ( pwidgetContent->inherits( "CDataSourceNamesFile" ) )
    {
        CDataSourceNamesFile *pDataSourceNamesFile = (CDataSourceNamesFile*)pwidgetContent;
        stringConnectString = pDataSourceNamesFile->getDataSourceName();
        if ( !stringConnectString.isEmpty() )
            stringConnectString = QString::fromLocal8Bit( "FILEDSN=" ) + stringConnectString;
    }
    else
        return;

    // must select a data source name...
    if ( stringConnectString.isEmpty() )
    {
        QMessageBox::warning( this, tr( "Select a data source name..." ), tr( "Please select a data source name or Cancel." ) );
        return;
    }

    accept();
}

void CDriverConnectPrompt::loadState()
{
    QSettings settings;

    int nW = settings.value( QString::fromLocal8Bit( "CDriverConnectPrompt/w" ), geometry().width() ).toInt();
    int nH = settings.value( QString::fromLocal8Bit( "CDriverConnectPrompt/h" ), geometry().height() ).toInt();

    resize( nW, nH );
}

void CDriverConnectPrompt::saveState()
{
    QSettings settings;

    settings.setValue( QString::fromLocal8Bit( "CDriverConnectPrompt/w" ), width() );
    settings.setValue( QString::fromLocal8Bit( "CDriverConnectPrompt/h" ), height() );
}


