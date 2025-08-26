/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnyxTCPConnection.h
 * Author: Kenny
 *
 * Created on October 13, 2020, 1:47 PM
 */

#ifndef ENYXTCPCONNECTION_H
#define ENYXTCPCONNECTION_H

//#include <enyx/cores/cores.h>
//#include <enyx/cores/tcp.h>
#include <list>

#include "Responder.h"
#include "Parser.h"
#include "Session.h"
#include "Mutex.h"

/*
struct EnyxTCPSender
{
  virtual void send_data(const char* data, unsigned int len) = 0;
  virtual void close_connection() = 0;
  virtual unsigned int get_connection_id() = 0;
  virtual void destroy() = 0;  
};
*/

class EnyxTCPInitiator;

class EnyxTCPConnection : public FIX::Responder 
{
public:
    EnyxTCPConnection(FIX::Session* session, unsigned int ethport_id, unsigned int connection_id);
    virtual ~EnyxTCPConnection();
    
    // interface inherited from FIX::Responder
    virtual bool send( const std::string& );
    virtual void disconnect();
    
    //void processQueue();
    void parseMessage();
    void onTimeout();
    void onData(const unsigned char* data, unsigned int size);
    //void onDataSent(unsigned int size);
    /*
    void attachSession(const FIX::SessionID& sessionID, FIX::Session* session) 
    { 
        m_SessionID = sessionID;
        m_pSession = session;
    }
    */
    
    //FIX::Session* getSession() { return m_pSession; }
    //FIX::SessionID getSessionID() { return m_SessionID; }
    
private:
    
    bool readMessage(std::string& msg);
    
    FIX::Parser m_parser;        
    //std::list<std::string> m_sendQueue;
    //unsigned int m_sendLength;
    
    FIX::Session* m_pSession;
    //FIX::SessionID m_SessionID;
        
    //EnyxTCPSender*    m_tcpSender;
    //EnyxTCPInitiator* m_enyxInitiator;
    //FIX::Mutex       m_lock;
    
    //bool m_isIOPending;
    unsigned int m_ethportID;
    unsigned int m_connectionID;
};

#endif /* ENYXTCPCONNECTION_H */

