// 
// File:   SpeedyFIFOOrderConnection.h
// Author: yuan
//
// Created on June 15, 2009, 11:01 AM
//

#ifndef _SpeedyFIFOOrderConnection_H
#define	_SpeedyFIFOOrderConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//------------------------------------------------------------------------------
typedef enum
{
	csDisconnect = 0,
	csConnecting = 1,
	csConnected  = 2,
        csReady      = 3

} TConnectionStateEnum;
//--------------------------------------------------------------------------
class SpeedyQueueOrderConnection : public UFC::PThread
{
private:    
    ///< Queue infomation
    UFC::PQueue           FOrderQueue;
    UFC::PQueue           FTSEConfirmQueue;
    UFC::PQueue           FOTCConfirmQueue;
    UFC::PQueue           FTSEFillQueue;
    UFC::PQueue           FOTCFillQueue;
    UFC::PQueue           FTFXExecQueue;
    TTaifexConnection*    FConnection;
private:
    void Execute( void );
    void SendOrder( void );
public:
    SpeedyQueueOrderConnection( TTaifexConnection* Dispatcher, 
			        key_t OrderKey, 
			        key_t TSEConfirmKey, key_t TSEFillKey,
			        key_t OTCconfirmKey, key_t OTCFillKey,
                                key_t TFXExecKey );
    virtual ~SpeedyQueueOrderConnection( void );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void RejectToBackend( int NID,
			  UFC::AnsiString Order,
			  UFC::AnsiString UserDef,
			  nsOrderMessageDefine::MarketEnum );
    void RejectR070Backend( int NID,
			  UFC::AnsiString Order,
			  UFC::AnsiString UserDef,
			  nsOrderMessageDefine::MarketEnum );    
};
//------------------------------------------------------------------------------

#endif	/* _SpeedyFIFOOrderConnection_H */

