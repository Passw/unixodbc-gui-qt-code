/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef DNODEODBC_H
#define DNODEODBC_H

#include <OQGSystem.h>
#include <OQGEnvironment.h>

#include "DNode.h"
class DNodeODBC : public DNode
{
    Q_OBJECT
public:
    DNodeODBC( DNodeWidget *pNodeWidget );
    virtual ~DNodeODBC();

    OQGSystem *     getSystem();        { return pSystem;       }
    OQGEnvironment *getEnvironment()    { return pEnvironment;  }

    void doLoadProperties( DPropWidget *pPropWidget );
    void doClearProperties( DPropWidget *pPropWidget );
    void doLoad() {}
    void doClear() {}

private:
    OQGSystem   *       pSystem;
    OQGEnvironment *    pEnvironment;
};

#endif

