/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCModelDataSourcesFile_H
#define ODBCModelDataSourcesFile_H

#include "ODBCModel.h"

class ODBCModelDataSourcesFile : public ODBCModel
{
    Q_OBJECT
public:
    explicit ODBCModelDataSourcesFile( OQGEnvironment *pHandle, ODBCModel *pmodelParent );
    ~ODBCModelDataSourcesFile();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void    setText( const QString & ) {}

    QIcon   getIcon();

    bool doLoad();
    bool doClear();
};

#endif

