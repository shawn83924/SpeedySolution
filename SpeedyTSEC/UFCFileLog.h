/* 
 * File:   UFCFileLog.h
 * Author: yuan
 *
 * Created on 2014年5月16日, 下午 12:21
 */

#ifndef UFCFILELOG_H
#define	UFCFILELOG_H
#include "FileStore.h"
#include "Log.h"
#include "SessionSettings.h"
#include <fstream>
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------
namespace TWSE
{
    class SpeedyObjectsManager;
}
//------------------------------------------------------------------------------
namespace FIX
{   
//------------------------------------------------------------------------------
/**
 * Creates a file based implementation of Log
 *
 * This stores all log events into flat files
 */
class UFCFileLogFactory : public LogFactory
{
public:
  UFCFileLogFactory( TWSE::SpeedyObjectsManager*  ObjectManager, const SessionSettings& settings, bool RecoverMode )
  : m_settings( settings ), m_globalLog(0), m_globalLogCount(0),m_recover(RecoverMode),FObjectManager( ObjectManager ) {};
  
  UFCFileLogFactory( TWSE::SpeedyObjectsManager*  ObjectManager, const std::string& path )
  : m_path( path ), m_globalLog(0), m_globalLogCount(0),FObjectManager( ObjectManager ) {};
public:
  Log* create();
  Log* create( const SessionID& );
  void destroy( Log* log );
private:
  std::string     m_path;
  SessionSettings m_settings;
  Log*            m_globalLog;
  int             m_globalLogCount;
  bool            m_recover;
  TWSE::SpeedyObjectsManager*  FObjectManager;
};
//------------------------------------------------------------------------------
/**
 * File based implementation of Log
 *
 * Three files are created by this implementation.  One for outgoing
 * messages, one for incoming message, and one for events.
 *
 * The formats of the files are:<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].messages<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].event<br>
 *
 */
class UFCFileLog : public Log
{
public:
  UFCFileLog( TWSE::SpeedyObjectsManager*  ObjectManager, const std::string& path, bool IsRecover );
  UFCFileLog( TWSE::SpeedyObjectsManager*  ObjectManager, const std::string& path, const SessionID& sessionID, bool IsRecover );
public: ///< Implement Log interface  
  virtual ~UFCFileLog(); 
  virtual void clear();
  virtual void onIncoming( const std::string& value );
  virtual void onOutgoing( const std::string& value );
  virtual void onEvent( const std::string& value );
  void backup() {}
private:
  void init( std::string path, const std::string& prefix, bool IsRecover );  
  int  BuildPrefix( bool IsWrite);
  std::ofstream         m_event;
  std::string           m_eventFileName;
  SessionID             m_SessionID;
  int                   m_msg_log_count;
  int                   FTimeStempLen; 
  int                   FSendPerfixLen;
  int                   FRecvPerfixLen;
  char                  FLinkPVCSendStr[ 32 ];
  char                  FLinkPVCRecvStr[ 32 ];  
  char*                 FLogBuffer;
  TWSE::SpeedyObjectsManager* FObjectsManager;
};
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif	/* UFCFILELOG_H */

