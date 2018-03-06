/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#include <QtGui>

#include "CPropertiesDelegate.h"
#include "CPropertiesModel.h"
#include "CFileSelector.h"

CPropertiesDelegate::CPropertiesDelegate( QObject *pobjectParent )
    : QItemDelegate( pobjectParent )
{
}

/* we could just load the value now and save all of the subsequent logic to do so - but we follow the Qt encouraged flow of things and defer to setEditorData */
QWidget *CPropertiesDelegate::createEditor( QWidget *pwidgetParent, const QStyleOptionViewItem &styleoptionviewitem, const QModelIndex &modelindex ) const
{
    Q_UNUSED(styleoptionviewitem)

    if ( modelindex.column() != 1 )
        return 0;

    HODBCINSTPROPERTY pProperty = modelindex.model()->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            {
                QLabel *pLabel = new QLabel( pwidgetParent );
                return pLabel;
            }
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = new QComboBox( pwidgetParent );
                for ( int n = 0; (pProperty->aPromptData)[n]; n++ )
                {
                    pComboBox->insertItem( 0, QString::fromLocal8Bit( (pProperty->aPromptData)[n] ) );
                }
                return pComboBox;
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = new QComboBox( pwidgetParent );
                for ( int n = 0; (pProperty->aPromptData)[n]; n++ )
                {
                    pComboBox->insertItem( 0, QString::fromLocal8Bit( (pProperty->aPromptData)[n] ) );
                }
                pComboBox->setEditable( true );
                return pComboBox;
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector;

                /* Guess the type of file.
                 * 
                 * We do not have enough information here to do anything more then guess the 
                 * file type by the property name. Failing that - fall back to Generic. 
                 */
                if ( QString::fromLocal8Bit( pProperty->szName ) == QString::fromLocal8Bit( "Database" ) )
                    pFileSelector = new CFileSelector( CFileSelector::Database, QString::null, true, true, pwidgetParent );
                else if ( QString::fromLocal8Bit( pProperty->szName ).left( 6 ) == QString::fromLocal8Bit( "Driver" ) )
                    pFileSelector = new CFileSelector( CFileSelector::Driver, QString::null, true, true, pwidgetParent );
                else if ( QString::fromLocal8Bit( pProperty->szName ).left( 5 ) == QString::fromLocal8Bit( "Setup" ) )
                    pFileSelector = new CFileSelector( CFileSelector::Setup, QString::null, true, true, pwidgetParent );
                else if ( QString::fromLocal8Bit( pProperty->szName ) == QString::fromLocal8Bit( "TraceFile" ) )
                    pFileSelector = new CFileSelector( CFileSelector::TraceFile, QString::null, true, true, pwidgetParent );
                else if ( QString::fromLocal8Bit( pProperty->szName ) == QString::fromLocal8Bit( "TraceLibrary" ) )
                    pFileSelector = new CFileSelector( CFileSelector::TraceLibrary, QString::null, true, true, pwidgetParent );
                else
                    pFileSelector = new CFileSelector( CFileSelector::Generic, QString::null, true, true, pwidgetParent );
                return pFileSelector;
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = new QLineEdit( pwidgetParent );
                if ( pProperty->nPromptType == ODBCINST_PROMPTTYPE_TEXTEDIT_PASSWORD )
                    pLineEdit->setEchoMode( QLineEdit::Password ) ;
                return pLineEdit;
            }
            break;
    }

    return 0;
}

void CPropertiesDelegate::setEditorData( QWidget *pwidgetEditor, const QModelIndex &modelindex ) const
{
    HODBCINSTPROPERTY pProperty = modelindex.model()->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                pComboBox->setCurrentIndex( pComboBox->findText( QString::fromLocal8Bit( pProperty->szValue ), Qt::MatchExactly ) );
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                pComboBox->lineEdit()->setText( QString::fromLocal8Bit( pProperty->szValue ) );
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector = static_cast<CFileSelector*>( pwidgetEditor );
                pFileSelector->setText( QString::fromLocal8Bit( pProperty->szValue ) );
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit*>( pwidgetEditor );
                pLineEdit->setText( QString::fromLocal8Bit( pProperty->szValue ) );
            }
            break;
    }
}

/* we could just update the HODBCINSTPROPERTY value here but we go with the Qt encouraged flow of things and defer to the views setData */
void CPropertiesDelegate::setModelData( QWidget *pwidgetEditor, QAbstractItemModel *abstractitemmodel, const QModelIndex &modelindex) const
{
    HODBCINSTPROPERTY pProperty = abstractitemmodel->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pComboBox->currentText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pComboBox->currentText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector = static_cast<CFileSelector*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pFileSelector->getText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pLineEdit->text(), Qt::EditRole );
            }
            break;
    }
}

void CPropertiesDelegate::updateEditorGeometry( QWidget *pwidgetEditor, const QStyleOptionViewItem &styleoptionviewitem, const QModelIndex &modelindex ) const
{
    Q_UNUSED(modelindex)

    pwidgetEditor->setGeometry( styleoptionviewitem.rect );
}

