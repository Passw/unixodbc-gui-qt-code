/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCModelDataSource_H
#define ODBCModelDataSource_H

#include "ODBCModel.h"

class ODBCModelDataSourcesFile;
class ODBCModelDataSourcesSystem;
class ODBCModelDataSourcesUser;

class ODBCModelDataSource : public ODBCModel
{
    Q_OBJECT
public:
    enum DataSourceTypes
    {
        DataSourceFile,
        DataSourceSystem,
        DataSourceUser
    };

    explicit ODBCModelDataSource( OQGEnvironment *pHandle, ODBCModelDataSourcesFile *pParent, const QString &stringDataSourceName );
    explicit ODBCModelDataSource( OQGEnvironment *pHandle, ODBCModelDataSourcesSystem *pParent, const QString &stringDataSourceName );
    explicit ODBCModelDataSource( OQGEnvironment *pHandle, ODBCModelDataSourcesUser *pParent, const QString &stringDataSourceName );
    ~ODBCModelDataSource();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void setText( const QString & ) {}

    QIcon getIcon();

    bool doLoad();
    bool doClear();
    void doContextMenu( QWidget *pwidgetParent, QPoint pos );

protected slots:
    void slotNewConnection();
    void slotDelete();

protected:
    DataSourceTypes     nDataSourceType;
    QVector<QString>    vectorKeys;       // property keys...
    QVector<QString>    vectorValues;     // ...and corresponding values (we need to access via index not key)
    QAction *           pactionNewConnection; 
    QAction *           pactionDelete;

    void doLoadProperties();
    bool doSaveProperty( const QString &stringKey, const QString &stringValue );
};

#endif

