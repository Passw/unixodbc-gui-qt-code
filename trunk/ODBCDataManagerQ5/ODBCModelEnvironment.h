/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCMODELENVIRONMENT_H
#define ODBCMODELENVIRONMENT_H

#include "ODBCModel.h"

class ODBCModelSystem;

/*!
 * \class   ODBCModelEnvironment
 * \brief   This model encapsulates an ODBC environment.
 * \author  pharvey (8/7/2008)
 */
class ODBCModelEnvironment : public ODBCModel
{
    Q_OBJECT
public:
    explicit ODBCModelEnvironment( OQGEnvironment *pHandle, ODBCModelSystem *pParent );
    ~ODBCModelEnvironment();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    QIcon getIcon();

    OQGEnvironment *getEnvironment() { return (OQGEnvironment*)(getHandle()); }

    bool doLoad();
    bool doClear();
    void doContextMenu( QWidget *pwidgetParent, QPoint pos );

protected slots:
    void slotAlloc();
    void slotFree();
    void slotDelete();

protected:
    QAction *   pactionAlloc; 
    QAction *   pactionFree; 
    QAction *   pactionDelete; 

private:
    ODBCMetaInfoAttr *  pEnvAttr;
};

#endif

