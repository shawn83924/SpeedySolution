/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnyxTCPInitiator.cpp
 * Author: Kenny
 * 
 * Created on October 13, 2020, 9:37 AM
 */

#include <arpa/inet.h>
#include <dlfcn.h>
#include <map>
#include <pthread.h>
#include "EnyxTCPInitiator.h"

FPGATCPContext* EnyxTCPInitiator::fpgaTCPContext = NULL;
int EnyxTCPInitiator::ctxRefCnt = 0;
//int EnyxTCPInitiator::cpus_specified = 0;

const char* ENYXAPI_WRAPPER = "libEnyxAPIWrapper.so";

typedef int (*func_tcp_create_context)(FPGATCPContext**);

FPGATCPContext* EnyxTCPInitiator::CreateEnyxTCPContext()
{
    if (!fpgaTCPContext)
    {
        std::string dll_name = get_current_dir_name();
        dll_name += "/";
        dll_name += ENYXAPI_WRAPPER;
    
        void* hlib = dlopen(dll_name.c_str(), RTLD_LAZY);
        if (!hlib)
            return nullptr;
    
        func_tcp_create_context fpga_tcp_create_context = (func_tcp_create_context)dlsym(hlib, "fpga_tcp_create_context");
        if (!fpga_tcp_create_context)
            return nullptr;
    
        int rt = fpga_tcp_create_context(&fpgaTCPContext);
    
        if (rt != 0)
            return nullptr;
    
        return fpgaTCPContext;
    }
    else
    {
        return fpgaTCPContext;
    }
}

EnyxTCPInitiator::EnyxTCPInitiator( FIX::Application& application,
                                    FIX::MessageStoreFactory& factory,
                                    const FIX::SessionSettings& settings )
throw( FIX::ConfigError )
: Initiator( application, factory, settings ),
  m_reconnectInterval(30)
  //m_cpuID(-1)
{
    ++ctxRefCnt;
    
    /*
    if (cpus_specified == 0)
    {
        ++cpus_specified;
        m_cpuID = 2;        
    }
    else if(cpus_specified == 1)
    {
        ++cpus_specified;
        m_cpuID = 3;
    }
    */
}

EnyxTCPInitiator::EnyxTCPInitiator( FIX::Application& application,
                                    FIX::MessageStoreFactory& factory,
                                    const FIX::SessionSettings& settings,
                                    FIX::LogFactory& logFactory )
throw( FIX::ConfigError )
: Initiator( application, factory, settings, logFactory ),  
  m_reconnectInterval(30)
  //m_cpuID(-1)
{
    ++ctxRefCnt;
    
    /*
    if (cpus_specified == 0)
    {
        ++cpus_specified;
        m_cpuID = 2;        
    }
    else if(cpus_specified == 1)
    {
        ++cpus_specified;
        m_cpuID = 3;
    }
    */
}

EnyxTCPInitiator::~EnyxTCPInitiator() 
{
    for(int i=0; i<2 ; ++i)
    {
        std::map<unsigned int, EnyxTCPConnection*>::iterator iter = m_enyxTCPConnections[i].begin();
        while (iter != m_enyxTCPConnections[i].end())
        {
            delete iter->second;
            ++iter;
        }
    }
        
    if (--ctxRefCnt <= 0 && fpgaTCPContext)
    {
        fpgaTCPContext->decRefCnt();
        fpgaTCPContext = NULL;
    }
}

void EnyxTCPInitiator::onConfigure( const FIX::SessionSettings& s) throw ( FIX::ConfigError )
{
    try 
    { 
        m_reconnectInterval = s.get().getLong("ReconnectInterval"); 
    }
    catch ( std::exception& )
    {}
}

void EnyxTCPInitiator::onInitialize( const FIX::SessionSettings& ) throw ( FIX::RuntimeError )
{
    // TODO ....
}

void EnyxTCPInitiator::onStart()
{
    /*
    if (m_cpuID != -1)
    {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(m_cpuID, &set);
        pthread_setaffinity_np(pthread_self(), sizeof(set), &set);        
    }
    */
    
    time_t now = time(NULL);
    time_t lastConnectTime = now;
        
    connect();
    
    while ( !isStopped() )
    {
        
        timespec wt,lt;
        wt.tv_sec  = 0;
        wt.tv_nsec = 10000000; // 10 ms
        nanosleep(&wt,&lt);
        
        ::time(&now);
        if (lastConnectTime + m_reconnectInterval < now)
        {
            connect();
            lastConnectTime = now;
        }

        for(int i=0; i<2; ++i)
        {
            std::map<unsigned int, EnyxTCPConnection*>::iterator conn_iter = m_enyxTCPConnections[i].begin();
            while (conn_iter != m_enyxTCPConnections[i].end())
            {
                conn_iter->second->onTimeout();
                ++conn_iter;
            }
        }
    }
    
    time_t start = time(NULL);
    while ( isLoggedOn() )
    {
        timespec wt,lt;
        wt.tv_sec  = 0;
        wt.tv_nsec = 200000000; // 200 ms
        nanosleep(&wt,&lt);
        
        if( time(NULL) >= start + 5)
           break;
    }  
}

void EnyxTCPInitiator::onStop()
{
    // TODO ....
}

bool EnyxTCPInitiator::doConnect( const FIX::SessionID& s, const FIX::Dictionary& d)
{
    
  try
  {
    std::string address;
    short port = 0;
    short localport = 0;
    FIX::Session* session = FIX::Session::lookupSession( s );
    if( !session->isSessionTime()) return false;

    FIX::Log* log = session->getLog();

    getHost( s, d, address, port, localport );

    log->onEvent( "[enyx] Connecting to " + address + 
                  " on port " + FIX::IntConvertor::convert((unsigned short)port) + 
                  " local port " + FIX::IntConvertor::convert((unsigned short)localport));
    
    setPending( s );
    
    FPGATCPConnectionParam cp;
    cp.user_data = session;
    cp.port = (unsigned short)port;
    cp.local_port = (unsigned short)localport;
    memset(cp.ip, 0, sizeof(cp.ip));
    strncpy(cp.ip, address.c_str(), sizeof(cp.ip) - 1);
    fpgaTCPContext->tcp_open(&cp,this);
    return true;    
  }
  catch ( std::exception& ) { return false; }
    
}

void EnyxTCPInitiator::getHost( const FIX::SessionID& s, const FIX::Dictionary& d,
                               std::string& address, short& port , short& localport)
{ 
  int num = 0;
  std::map < FIX::SessionID, int >::iterator i = m_sessionToHostNum.find( s );
  if ( i != m_sessionToHostNum.end() ) 
      num = i->second;

  std::stringstream hostStream;
  hostStream << "SocketConnectHost" << num;
  std::string hostString = hostStream.str();

  std::stringstream portStream;
  portStream << "SocketConnectPort" << num;
  std::string portString = portStream.str();

  std::stringstream localportStream;
  localportStream << "SocketLocalPort" << num;
  std::string localportString = localportStream.str();
      
  if( d.has(hostString) && d.has(portString) )
  {
      address = d.getString( hostString );
      port = ( short ) d.getLong( portString );
      if (d.has(localportString))
          localport = ( short ) d.getLong( localportString );
      else
          localport = 0;
  }
  else
  {
      num = 0;
      address = d.getString( "SocketConnectHost" );
      port = ( short ) d.getLong( "SocketConnectPort" );
      if (d.has("SocketLocalPort"))
          localport = ( short ) d.getLong( "SocketLocalPort" );
      else
          localport = 0;
  }

  m_sessionToHostNum[ s ] = ++num;
    
   
}

void EnyxTCPInitiator::onConnected(unsigned int ethport_id, unsigned short connection_id, void* user_data)
{
    FIX::Session* session = reinterpret_cast<FIX::Session*>(user_data);
    FIX::SessionID s = session->getSessionID();
    
    EnyxTCPConnection* connection = new EnyxTCPConnection(session, ethport_id, connection_id);
    FIX::Initiator::getSession(s,*connection);
    m_enyxTCPConnections[ethport_id].insert(std::make_pair(connection_id, connection));
    
    FIX::Log* log = session->getLog();
    log->onEvent( "connection #" + FIX::IntConvertor::convert(ethport_id) + ":" + FIX::IntConvertor::convert(connection_id) + " connected.");
    
    FIX::Initiator::setConnected(s);
    connection->onTimeout();    
}

void EnyxTCPInitiator::onConnectFail(void* user_data)
{
    FIX::Session* session = reinterpret_cast<FIX::Session*>(user_data);
    FIX::SessionID s = session->getSessionID();
    
    FIX::Log* log = session->getLog();
    log->onEvent( "connect fail!");
    
    FIX::Initiator::setDisconnected(s);
}

void EnyxTCPInitiator::onDisconnected(unsigned int ethport_id, unsigned short connection_id, void* user_data)
{
    FIX::Session* session = reinterpret_cast<FIX::Session*>(user_data);
    FIX::SessionID s = session->getSessionID();
    
    session->disconnect();
    
    if (m_enyxTCPConnections[ethport_id].find(connection_id) != m_enyxTCPConnections[ethport_id].end())
    {
        EnyxTCPConnection* connection = m_enyxTCPConnections[ethport_id][connection_id];
        m_enyxTCPConnections[ethport_id].erase(connection_id);
        delete connection;
    }
    
    FIX::Log* log = session->getLog();
    log->onEvent( "disconnected.");
    
    FIX::Initiator::setDisconnected(s);
}

void EnyxTCPInitiator::onDataReceived(unsigned int ethport_id, unsigned short connection_id, const unsigned char* data, unsigned int len, void* user_data)
{
    //FIX::Session* session = reinterpret_cast<FIX::Session*>(user_data);
    //FIX::SessionID s = session->getSessionID();
    
    if (m_enyxTCPConnections[ethport_id].find(connection_id) == m_enyxTCPConnections[ethport_id].end())
        return;
    
    m_enyxTCPConnections[ethport_id][connection_id]->onData(data, len);
    m_enyxTCPConnections[ethport_id][connection_id]->parseMessage();
}