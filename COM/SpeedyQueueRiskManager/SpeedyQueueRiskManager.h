// 
// File:   SpeedyFIFOConnection.h
// Author: yuan
//
// Created on June 15, 2009, 11:01 AM
//

#ifndef _SPEEDYFIFOCONNECTION_H
#define	_SPEEDYFIFOCONNECTION_H
//------------------------------------------------------------------------------
#include "Sigo.h" 
#include "UFC.h" 
#include "FIFO.h"
#include "iniFile.h"
//--------------------------------------------------------------------------
typedef enum market
{
    mFutures = 0,
    mOptions = 1,
    mTSE     = 2,
    mOTC     = 3,
    mPATS    = 4 

}Market;

//--------------------------------------------------------------------------
typedef enum 
{
    eTAIFEX  = 0,
    eTWSEOTC = 1,
    ePATS    = 2 

}EchangeEnum;
//--------------------------------------------------------------------------
class SpeedyQueueConnection : public UFC::PThread
{
private:
    MessageObject*      FMessageObject;
    UFC::PQueue         FSendQueue;
    UFC::PQueue         FRecvQueue;
    EchangeEnum         FExchange;
public: 
    void OnMessage( const UFC::AnsiString& SendMsg, Market mkt, MTree* Data );
private:
    void Execute( void );
    void OnRecvMessage( const UFC::AnsiString& Message );
    UFC::AnsiString GetPATSOrderID( const UFC::AnsiString& Order );
public:
    SpeedyQueueConnection( const UFC::AnsiString& SendPipeName, 
                           const UFC::AnsiString& RecvPipeName,
                           MessageObject* MessageObj,
                           EchangeEnum Exchange );
    virtual ~SpeedyQueueConnection( void ){}
};
//------------------------------------------------------------------------------

#endif	/* _SPEEDYFIFOCONNECTION_H */

