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
//--------------------------------------------------------------------------
class SpeedyFIFOOrderConnection : public UFC::PThread
{
private:    
    ///< File infomation
    UFC::FileStreamEx*	  FRecvFile;
    UFC::FileStreamEx*    FFUTSendFile;
    UFC::FileStreamEx*    FOPTSendFile;
    TTaifexConnection*    FConnection;
private:
    void Execute( void );
    void SendAck( const UFC::AnsiString& R010, const UFC::AnsiString& UserData, char Market );
    void SendOrder( UFC::FileStreamEx* OrderFile );
public:
    SpeedyFIFOOrderConnection( TTaifexConnection* Dispatcher, const UFC::AnsiString& OrderFile, const UFC::AnsiString FUTExec, const UFC::AnsiString& OPTExec );
    virtual ~SpeedyFIFOOrderConnection( void );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
};
//------------------------------------------------------------------------------

#endif	/* _SpeedyFIFOOrderConnection_H */

