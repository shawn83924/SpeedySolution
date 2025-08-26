/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnyxTCPConnection.cpp
 * Author: Kenny
 * 
 * Created on October 13, 2020, 1:47 PM
 */

#include <list>
#include <ctime>
#include <sstream>

#include "EnyxTCPConnection.h"
#include "EnyxTCPInitiator.h"

//#define ENYX_TRACE_IO

EnyxTCPConnection::EnyxTCPConnection(FIX::Session* session, unsigned int ethport_id, unsigned int connection_id) : 
    //m_sendLength(0),
    m_pSession(session), 
    //m_isIOPending(false),
    m_ethportID(ethport_id),
    m_connectionID(connection_id)
{
    
}

EnyxTCPConnection::~EnyxTCPConnection() 
{
    
}

bool EnyxTCPConnection::send( const std::string& msg)
{
    EnyxTCPInitiator::GetEnyxTCPContext()->tcp_send(m_ethportID, m_connectionID, msg.c_str(), msg.length());
    
    return true;    
}
 
void EnyxTCPConnection::disconnect()
{
    FIX::Session* session = m_pSession;
    if( session)
    {
        FIX::Log* log = session->getLog();
        log->onEvent(" close connection");
    }
        
    EnyxTCPInitiator::GetEnyxTCPContext()->tcp_close(m_ethportID, m_connectionID);
}

/*
void EnyxTCPConnection::processQueue()
{
    FIX::Locker l( m_lock );
    
    if (m_sendQueue.size() == 0)
        return;
    
    std::string msg = m_sendQueue.front();
        
    int ret = m_tcpSender->send_data(msg.c_str() + m_sendLength, msg.length() - m_sendLength);
        
    if (ret != 0)
    {
        m_sendLength += ret;
        if (m_sendLength >= msg.length())
        {
            m_sendLength = 0;
            m_sendQueue.pop_front();
        }            
    }    
}
*/

void EnyxTCPConnection::parseMessage()
{
    if( !m_pSession ) return;

    std::string msg;
    while( readMessage( msg ) )
    {
#ifdef ENYX_TRACE_IO        
        std::stringstream ss;
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
    
        ss << "R " << ts.tv_sec % 1000 << "." << ts.tv_nsec / 1000 << " " << msg;
        FIX::Session* session = m_pSession;
        if( session)
        {
            FIX::Log* log = session->getLog();
            log->onEvent(ss.str());
        }
#endif    
        try
        {
            m_pSession->next( msg );
        }
        catch ( FIX::InvalidMessage& )
        {
           if( !m_pSession->isLoggedOn() )
               disconnect();           
        }
    }
}

void EnyxTCPConnection::onTimeout()
{
    if( m_pSession ) m_pSession->next();
}
 
void EnyxTCPConnection::onData(const unsigned char *data, unsigned int size)
{
    m_parser.addToStream( reinterpret_cast<const char*>(data), size );        
 }

bool EnyxTCPConnection::readMessage( std::string& msg )
{ 
  try
  {
    return m_parser.readFixMessage( msg );
  }
  catch ( FIX::MessageParseError& ) 
  {
      
  }
  
  return true; 
}