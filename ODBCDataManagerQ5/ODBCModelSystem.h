/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCMODELSYSTEM_H
#define ODBCMODELSYSTEM_H

#include "ODBCModel.h"

/*!
 * \class   ODBCModelSystem
 * \brief   This model encapsulates the entire ODBC system - including 
 *          the system information related to installing and configuring
 *          ODBC components such as drivers and data source names.
 * \author  pharvey (8/7/2008)
 */
class ODBCModelSystem : public ODBCModel
{
    Q_OBJECT
public:
    explicit ODBCModelSystem( OQGSystem *pHandle );
    ~ODBCModelSystem();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    QIcon getIcon();

    bool doLoad();
    bool doClear();
    void doContextMenu( QWidget *pwidgetParent, QPoint pos );

protected slots:
    void slotNewEnvironment();

protected:
    QAction *   pactionNewEnvironment; 

private:
    ODBCMetaInfoAttr *  pSysAttr;
};

#endif

