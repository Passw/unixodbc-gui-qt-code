/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#pragma once

// qt
#include <QtGui>

// OQG
#include <OQGSystem.h>
#include <OQGEnvironment.h>
#include <OQGConnection.h>
#include <OQGStatement.h>

class OQGStatementClient : public QWidget
{ 
    Q_OBJECT
public:
	explicit OQGStatementClient( OQGConnection *pConnection, const QWidget *pwidgetParent = NULL );
	~OQGStatementClient();

    OQGStatement *getStatement() { return pStatement; }

public slots:
    void slotExecute();

protected:
    OQGStatement *   pStatement;

    QSplitter *         pSplitter;
    QTextEdit *         ptexteditSQL;
    QTableWidget *      ptablewidgetResults;

    void createClientArea();

    void doResults();
    void doResultGUIGrid();
    void doResultGUIGridHeader( SWORD nColumns );
    void doResultGUIGridBody( SWORD nColumns );
};


