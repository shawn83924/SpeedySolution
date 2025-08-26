// 
// File:   MessageDispatcher.h
// Author: yuan
//
// Created on January 4, 2010, 11:01 AM
//

#ifndef _MESSAGEDISPATCHER_H
#define	_MESSAGEDISPATCHER_H

#include "SpeedyFIFORiskManager.h"
//------------------------------------------------------------------------------
class MessageDispatcher : public MessageListener
{
private:
    MessageObject*        FMessageObject;
    UFC::AnsiString       FMBusIP;
    int                   FMBusPort;
    UFC::AnsiString       FSendFIFOName;
    UFC::AnsiString       FRecvFIFOName;
    UFC::AnsiString       FSubSendFIFOName;
    UFC::AnsiString       FSubRecvFIFOName;
    UFC::AnsiString       FPatsSendFIFOName;
    UFC::AnsiString       FPatsRecvFIFOName;
    SpeedyFIFOConnection  *FDefaultConnection;
    SpeedyFIFOConnection  *FDefaultSubConnection;
    SpeedyFIFOConnection  *FPATSConnection;
    UFC::PHashedList<UFC::AnsiString, SpeedyFIFOConnection*> FTable;
    UFC::PHashedList<UFC::AnsiString, SpeedyFIFOConnection*> FSubTable;
    
    bool GetDataFromOrderKey( const UFC::AnsiString& OrderKey, UFC::AnsiString& AccountExecutive, UFC::AnsiString& UserDefine, UFC::AnsiString& SubBrokerNo, UFC::AnsiString& SubAccount );
    bool GetSubAccountFromOrderKey( const UFC::AnsiString& OrderKey, UFC::AnsiString& SubAccount, UFC::AnsiString& UserDefine );
    
public:
    MessageDispatcher( void );
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
    void LoadConfig( void );  
};
//------------------------------------------------------------------------------
#endif	/* _MESSAGEDISPATCHER_H */

