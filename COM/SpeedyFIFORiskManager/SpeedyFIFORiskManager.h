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
//const Int32 MSG_LENGTH = 219;
//const Int32 MSG_LENGTH = 234;//75+4+1+10+128+15+1
const Int32 MSG_LENGTH = 376;//75+4+1+10+270+15+1
const Int32 TAIFEX_USERDATA_LENGTH = 270;
//const Int32 PATS_ORDER_LENGTH = 256;
const Int32 PATS_ORDER_LENGTH = 320;
const Int32 PATS_ERRORCODE_LENGTH = 4;
const Int32 PATS_NID_LENGTH = 10;
const Int32 PATS_USERDATA_LENGTH = 270;
const Int32 PATS_ORDERTIME_LENGTH = 9;
const Int32 PATS_SUBACCOUNT_LENGTH = 15;
const Int32 PATS_USERDATALEN_LENGTH = 5;
const Int32 PATS_CHANNEL_LENGTH = 32;
const Int32 PATS_BACKEND_REPLY_LENGTH = 667;
extern Int32 patsDefaultUserDataLength;
//--------------------------------------------------------------------------
typedef enum market
{
    mFutures = 0,
    mOptions = 1,
    mPATSFutures = 4
}Market;
//--------------------------------------------------------------------------
class SpeedyFIFOConnection : public UFC::PThread
{
public:
    static UFC::PReadWriteLock FNIDLock;
    static UFC::PHashedSet<Int32> ProcessedNIDSet;
    static UFC::AnsiString FProcessedNIDFileName;
    static UFC::FileStreamEx *FProcessedNIDFilePtr;
    static int RecoverProcessedNID();
    
private:
    MessageObject*  FMessageObject;
    UFC::AnsiString FSendFIFOName;
    UFC::AnsiString FRecvFIFOName;
    UFC::FIFO*      FSendPipe;
    UFC::FIFO*      FRecvPipe;
    int             FMessageLength;

public: 
    void OnMessage( const UFC::AnsiString& SendMsg, Market mkt, MTree* Data );
private:
    void Execute( void );
    void OnRecvMessage( const UFC::AnsiString& Message );
public:
    SpeedyFIFOConnection( const UFC::AnsiString& SendPipeName, const UFC::AnsiString& RecvPipeName, MessageObject* MessageObj, int MessageLength, bool StartThread = false );
    virtual ~SpeedyFIFOConnection( void );
};
//------------------------------------------------------------------------------

#endif	/* _SPEEDYFIFOCONNECTION_H */

