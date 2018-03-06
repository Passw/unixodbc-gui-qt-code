/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2009-2010, LGPL
 */
#ifndef CODBCINST_H
#define CODBCINST_H

#include <ini.h>
#include <odbcinstext.h>

#ifndef odbcinst_system_file_path
    #if defined(__cplusplus)
             extern  "C" {
    #endif
        char * INSTAPI odbcinst_system_file_path( char *buffer );
        char * INSTAPI odbcinst_system_file_name( char *buffer );
        BOOL _odbcinst_FileINI(	char *pszPath );
    #ifdef __cplusplus
             }
    #endif
#endif

#ifndef SHLIBEXT
    #define SHLIBEXT ".so"
#endif

#include <QtGui>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QWizardPage>
#include <QTableView>
#include <QTableWidgetItem>
#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QGridLayout>
#include <QToolButton>
#include <QListView>
#include <QPushButton>
#include <QTextBrowser> 
#include <QTabWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QIcon>
#include <QPixmap>
#include <QListWidget>
#include <QStackedWidget>
#include <QComboBox>
#include <QDesktopWidget>

class CDSNWizardData
{
public:
    enum Type
    {
        TypeUser,
        TypeSystem,
        TypeFile
    };

    CDSNWizardData( const QString &stringDataSourceName = QString::null ) ;
    ~CDSNWizardData();

    Type                nType;
    QString             stringDriver;
    QString             stringDataSourceName;
    HODBCINSTPROPERTY   hFirstProperty;
};

/*! 
 * \brief   A namespace.
 * 
 */
class CODBCInst
{
public:
    static bool saveDataSourceName( QWidget *pwidgetParent, HODBCINSTPROPERTY hFirstProperty, CDSNWizardData::Type nType, const QString &stringIni = QString::null );
    static int showErrors( QWidget *pwidgetParent = 0, const QString &stringConsequence = QString::null );
};

#endif

