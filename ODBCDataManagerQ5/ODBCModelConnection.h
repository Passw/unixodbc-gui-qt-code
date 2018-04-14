/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2018
 * \license Copyright unixODBC-GUI-Qt Project 2007-2018, See GPL.txt
 */
#ifndef ODBCMODELCONNECTION_H
#define ODBCMODELCONNECTION_H

#include "ODBCModel.h"

class ODBCModelDriver;

/*!
 * \class   ODBCModelConnection
 * \brief   This model encapsulates an ODBC connection.
 * \author  pharvey
 */
class ODBCModelConnection : public ODBCModel
{
    Q_OBJECT
public:
    explicit ODBCModelConnection( OQGEnvironment *pHandle, ODBCModelDriver *pParent );
//    explicit ODBCModelConnection( OQGEnvironment *pHandle, ODBCModelDSN *pParent );
//    explicit ODBCModelConnection( OQGEnvironment *pHandle, ODBCModelDSNFile *pParent );
    ~ODBCModelConnection();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    QIcon getIcon();

    bool doLoad();
    bool doClear();
    void doContextMenu( QWidget *pwidgetParent, QPoint pos );

protected slots:
    void slotConnect();
    void slotDisconnect();

protected:
    QAction *   pactionConnect; 
    QAction *   pactionDisconnect; 

private:
};

#endif

