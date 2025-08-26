
#include "UFCFileLog.h"
#include "Settings.h"
#include "TWSESpeedyObjectsManager.h"
//--------------------------------------------------------------------------------------
namespace FIX
{
//--------------------------------------------------------------------------------------
//  class imFIXLogFactory
//--------------------------------------------------------------------------------------
Log* UFCFileLogFactory::create()
{
    m_globalLogCount++;
    if( m_globalLogCount > 1 )
        return m_globalLog;

    try
    {
      if( m_path.size() ) 
          return new UFCFileLog( FObjectManager, m_path, m_recover );
      std::string path;
      Dictionary settings = m_settings.get();
      path = settings.getString( FILE_LOG_PATH );
      return m_globalLog = new UFCFileLog( FObjectManager, path, m_recover );
    }
    catch( ConfigError& )
    {
      m_globalLogCount--;
      throw;
    }
}
//--------------------------------------------------------------------------------------
Log* UFCFileLogFactory::create( const SessionID& s )
{
    if( m_path.size() ) 
        return new UFCFileLog( FObjectManager, m_path, s, m_recover );
    std::string path;
    Dictionary settings = m_settings.get( s );
    path = settings.getString( FILE_LOG_PATH );
    return new UFCFileLog( FObjectManager, path, s, m_recover );
}
//--------------------------------------------------------------------------------------
void UFCFileLogFactory::destroy( Log* pLog )
{
    if( pLog == m_globalLog )
    {
        m_globalLogCount--;
        if( m_globalLogCount == 0 )
        {
            delete pLog;
            m_globalLogCount = 0;
        }
    }
    else
    {
         delete pLog;
    }
}
//--------------------------------------------------------------------------------------
//  class UFCFileLog
//--------------------------------------------------------------------------------------
UFCFileLog::UFCFileLog( TWSE::SpeedyObjectsManager* ObjectsManager, const std::string& path, bool IsRecover )
:m_msg_log_count( 1 )
,FObjectsManager( ObjectsManager ) 
{
    init( path, "GLOBAL", IsRecover );
}
//--------------------------------------------------------------------------------------
UFCFileLog::UFCFileLog( TWSE::SpeedyObjectsManager* ObjectsManager, const std::string& path, const SessionID& s, bool IsRecover )
:m_SessionID( s )
,m_msg_log_count( 1 )
,FObjectsManager( ObjectsManager ) 
{
    FLinkPVCSendStr[0]=0;
    FLinkPVCRecvStr[0]=0;
    const std::string& begin     = s.getBeginString().getString();
    const std::string& sender    = s.getSenderCompID().getString();
    const std::string& target    = s.getTargetCompID().getString();
    const std::string& qualifier = s.getSessionQualifier();
    std::string prefix = begin + "-" + sender + "-" + target;
       

    if( qualifier.size() )
        prefix += "-" + qualifier;    
    FLogBuffer = new char[ 4096*2 ];
    init( path, prefix, IsRecover );
}
//--------------------------------------------------------------------------------------
UFCFileLog::~UFCFileLog()
{
    delete [] FLogBuffer;
    m_event.close();
}
//--------------------------------------------------------------------------------------
int UFCFileLog::BuildPrefix( bool IsWrite )
{    
    if( FLinkPVCSendStr[0] == 0 )
    {
        FIX::SessionSettings*  Setting    = FObjectsManager->GetFIXManager()->GetSettings();
        const FIX::Dictionary& Dictionary = Setting->get( m_SessionID );

        int         LocalPort = Dictionary.getLong( SOCKET_LOCAL_PORT );
        std::string PVCID( m_SessionID.getSenderCompID().getString().c_str() + 5 );

        FSendPerfixLen = sprintf( FLinkPVCSendStr, "Port%05d PVC:%s Send:",LocalPort, PVCID.c_str() );
        FRecvPerfixLen = sprintf( FLinkPVCRecvStr, "Port%05d PVC:%s Recv:",LocalPort, PVCID.c_str() );    
    }
    UFC::GetTimeString_us( FLogBuffer );        
    if( IsWrite == true )
    {
        memcpy( FLogBuffer + FTimeStempLen, FLinkPVCSendStr, FSendPerfixLen );
        return FTimeStempLen +  FSendPerfixLen;
    }
    else
    {
        memcpy( FLogBuffer + FTimeStempLen, FLinkPVCRecvStr, FRecvPerfixLen );
        return FTimeStempLen +  FRecvPerfixLen;
    }
}
//--------------------------------------------------------------------------------------
void UFCFileLog::init( std::string path, const std::string& prefix, bool IsRecover )
{
    char dateString[4];
    char TimeString[32];
    struct tm    LocalTime;
    struct timeb tb;

    ftime( &tb );
    localtime_r( &tb.time, &LocalTime );   
    sprintf( dateString, "%02d.", LocalTime.tm_mday );
    file_mkdir( path.c_str() );
    if ( path.empty() ) path = ".";

    std::string fullPrefix  = file_appendpath(path, prefix + ".");
    m_eventFileName    = fullPrefix + dateString + "event.log";

    UFC::GetTimeString_us( TimeString );        
    FTimeStempLen = strlen( TimeString );
    
    if( IsRecover == true )
    {
        m_event.open( m_eventFileName.c_str(), std::ios::out | std::ios::app );
        if ( !m_event.is_open() ) throw ConfigError( "Could not open event file: " + m_eventFileName );
    }
    else
    {
        m_event.open( m_eventFileName.c_str(), std::ios::out | std::ios::trunc );
        if ( !m_event.is_open() ) throw ConfigError( "Could not open event file: " + m_eventFileName );
    }
}
//--------------------------------------------------------------------------------------
void UFCFileLog::onIncoming( const std::string& value )
{
    int offset = BuildPrefix( false );
    int MsgLen = value.length() +1;
    
    memcpy( FLogBuffer + offset, value.c_str(), MsgLen );
    FObjectsManager->OrderLog->WriteString( FLogBuffer, offset + MsgLen );  
}
//--------------------------------------------------------------------------------------
void UFCFileLog::onOutgoing( const std::string& value )
{
    int offset = BuildPrefix( true );    
    int MsgLen = value.length() +1;
    
    memcpy( FLogBuffer + offset, value.c_str(), MsgLen );
    FObjectsManager->OrderLog->WriteString( FLogBuffer, offset + MsgLen );  
}
//--------------------------------------------------------------------------------------
void UFCFileLog::onEvent( const std::string& value )
{
    UtcTimeStamp now;
    m_event << UtcTimeStampConvertor::convert( now ) << " : " << value << std::endl << std::flush;
}
//--------------------------------------------------------------------------------------
void UFCFileLog::clear()
{
    m_event.close();
    m_event.open( m_eventFileName.c_str(), std::ios::out | std::ios::trunc );
}
//--------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------
