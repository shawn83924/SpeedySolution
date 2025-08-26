// 
// File:   FillExecution.h
// Author: yuan
//
// Created on November 13, 2007, 5:36 PM
//

#ifndef _FILLEXECUTION_H
#define	_FILLEXECUTION_H
//------------------------------------------------------------------------------
#include "Sigo.h"
#include "UFC.h"
#include "iniFile.h"
#include "TFixedFormat.h"
#include "TParseData.h"
//------------------------------------------------------------------------------
#define PROGRAM_NAME "FubonExecution"
//------------------------------------------------------------------------------
class TSEFillListener : public MessageListener
{
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//------------------------------------------------------------------------------
class OTCFillListener : public MessageListener
{
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
};
//----------------------------------------------------------------------------------------------------------------------
#endif	/* _FILLEXECUTION_H */

