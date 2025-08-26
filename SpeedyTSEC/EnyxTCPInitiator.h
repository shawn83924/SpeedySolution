/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnyxTCPInitiator.h
 * Author: Kenny
 *
 * Created on October 13, 2020, 9:37 AM
 */

#ifndef ENYXTCPINITIATOR_H
#define ENYXTCPINITIATOR_H

#include <map>
#include <list>
#include <set>
#include <ctime>
#include "Initiator.h"
#include "EnyxTCPConnection.h"
#include "EnyxAPIWrapper.h"

class EnyxTCPInitiator : public FIX::Initiator, public FPGATCPEventHandler 
{
public:
    EnyxTCPInitiator( FIX::Application&, FIX::MessageStoreFactory&,
                      const FIX::SessionSettings& ) throw( FIX::ConfigError );
    EnyxTCPInitiator( FIX::Application&, FIX::MessageStoreFactory&,
                      const FIX::SessionSettings&, FIX::LogFactory& ) throw( FIX::ConfigError );
    virtual ~EnyxTCPInitiator();
        
    static FPGATCPContext* CreateEnyxTCPContext();
    static FPGATCPContext* GetEnyxTCPContext() { return fpgaTCPContext; }
    
private:
  // interface inherited from FIX::Initiator
  virtual void onConfigure( const FIX::SessionSettings& ) throw ( FIX::ConfigError );
  virtual void onInitialize( const FIX::SessionSettings& ) throw ( FIX::RuntimeError );
  virtual void onStart();
  virtual bool onPoll() { return true; }
  virtual void onStop();
  virtual bool doConnect( const FIX::SessionID&, const FIX::Dictionary& );

  void getHost( const FIX::SessionID&, const FIX::Dictionary&, std::string&, short& , short&);
  
  // interface inherited from FPGATCPEventHandler
  /*
  void onOpening(unsigned int connection_id) override;
  void onConnected(unsigned int connection_id) override;
  void onClosing(unsigned int connection_id) override;
  void onClosed(unsigned int connection_id) override;
  void onData(unsigned int connection_id, const unsigned char* data, unsigned int len) override;
  */
  
  void onConnected(unsigned int ethport_id, unsigned short connection_id, void* user_data);
  void onConnectFail(void* user_data);
  void onDisconnected(unsigned int ethport_id, unsigned short connection_id, void* user_data);
  void onDataReceived(unsigned int ethport_id, unsigned short connection_id, const unsigned char* data, unsigned int len, void* user_data);
  //void onDataSent(unsigned short connection_id, unsigned int bytes_sent, void* user_data);
  
  //void procPendingConnections();
  //void checkPendingTimeout();
  //void procClosedConnections();
  //void procClosingConnections();
  //void doEventLoop();
    
  //FPGATCPContext*  m_enyxTCPCtx;
  static FPGATCPContext* fpgaTCPContext;
  static int             ctxRefCnt;
  
  int m_reconnectInterval;
 
  //std::set<unsigned int> m_readSet;
  //std::set<unsigned int> m_connectedSet;
  //std::set<unsigned int> m_closedSet;
  //std::map<unsigned int, time_t> m_pendingSet;
  
  std::map<unsigned int, EnyxTCPConnection*> m_enyxTCPConnections[2];
    
  //std::map<unsigned int, FIX::SessionID> m_enyxTCPSessionIDs; // connection_id -> FIX::SessionID
  std::map < FIX::SessionID, int > m_sessionToHostNum;
  
  //static int cpus_specified;
  //int m_cpuID;
  
};

#endif /* ENYXTCPINITIATOR_H */

