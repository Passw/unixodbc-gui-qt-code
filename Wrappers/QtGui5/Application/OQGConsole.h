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
#include <QtWidgets>

// OQG
#include <OQGSystem.h>
#include <OQGEnvironment.h>
#include <OQGConnection.h>
#include <OQGStatement.h>

#include "OQGMessageOutput.h"

/*!
 * \class OQGConsole 
 * \brief Provides the 'main window' for a sinmple application which allows the User to interact (and test) the wrapper. 
 *  
 *          This provides for a single; environment and connection handle. However; the connection can have
 *          more than one statement. The statements are presented to the User in a tab interface
 *          where each tab/statement contains an SQL area and a Results area.
 *  
 * \note    The UI is intentionaly simple. \sa OQGDataManager provides more functionalty in a richer UI.
 * 
 * \author pharvey (3/28/18)
 */
class OQGConsole : public QMainWindow
{ 
    Q_OBJECT
public:
	explicit OQGConsole();
	~OQGConsole();

protected:
    // these handles are init and remain while OQGConsole exist
    OQGSystem *      pSystem;
    OQGEnvironment * pEnvironment;
    OQGConnection *  pConnection;

    // controls
    QMenu * pmenuFile;
    QMenu * pmenuDataSource;
    QMenu * pmenuStatement;
    QMenu * pmenuHelp;
    QToolBar *  ptoolbarDataSource;
    QToolBar *  ptoolbarStatement;

    // actions
    // file
    QAction *   pactionQuit;
    // data sources
    QAction *   pactionDataSourceConnect;
    QAction *   pactionDataSourceNew;
    QAction *   pactionDataSourcesManage;
    // statement
    QAction *   pactionStatementNew;
    QAction *   pactionStatementClose;
    QAction *   pactionStatementExecute;
    // about
    QAction *   pactionAbout;

    // client area
    QSplitter *         pSplitter;
    QTabWidget *        pTabWidget;         // one tab per statement
    OQGTabOutput *      pmessageoutput;     // one output shared among all handles

    // init
    void createHandles();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createClientArea();

protected slots:
    // data sources
    void slotConnectToggle();
    void slotDataSourceNew();
    void slotDataSourcesManage();
    // statement
    void slotStatementNew();
    void slotStatementClose();
    void slotStatementExecute();
    // about
    void slotAbout();

    // other (not actions)
    void slotConnected();
    void slotDisconnected();
};


