/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#ifndef OQGMESSAGEOUTPUT_h
#define OQGMESSAGEOUTPUT_h

// Qt
#include <QtGui>
#include <QtWidgets>

//
#include <OQMessage.h>
#include <OQDiagnostic.h>

class OQGMsgOutput: public QTableWidget
{
	Q_OBJECT
public:
    OQGMsgOutput( QWidget *pwidgetParent = NULL );
    ~OQGMsgOutput();

    // SETTERS
    virtual void setWaitingMessages( int n );

    // GETTERS
    virtual int getWaitingMessages() { return nWaitingMessages; }

signals:
    void signalWaitingMessages( OQGMsgOutput * );

public slots:
    virtual void slotMessage( OQMessage Message );
    virtual void slotClear();

protected:
    int nWaitingMessages;
};

class OQGDiagOutput: public QTreeWidget
{
	Q_OBJECT
public:
    OQGDiagOutput( QWidget *pwidgetParent = NULL );
    ~OQGDiagOutput();

    // SETTERS
    virtual void setWaitingMessages( int n );

    // GETTERS
    virtual int getWaitingMessages() { return nWaitingMessages; }

signals:
    void signalWaitingMessages( OQGDiagOutput * );

public slots:
    virtual void slotDiagnostic( OQDiagnostic Diagnostic );
    virtual void slotClear();

protected:
    int nWaitingMessages;
};

class OQGTabOutput: public QTabWidget
{
	Q_OBJECT
public:
    OQGDiagOutput *pDiagOutput;
    OQGMsgOutput * pMsgOutput;

    OQGTabOutput( QWidget *pwidgetParent );
    ~OQGTabOutput();

};


#endif

