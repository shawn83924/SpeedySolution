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
    UFC::FileStreamEx*    FTSEConfirmFile;
    UFC::FileStreamEx*    FOTCConfirmFile;
    UFC::FileStreamEx*    FTSEFillFile;
    UFC::FileStreamEx*    FOTCFillFile;
    TTaifexConnection*    FConnection;
private:
    void Execute( void );
    void SendOrder( UFC::FileStreamEx* OrderFile );
    void UpdateNID( UFC::FileStreamEx* OrderFile, int NID );
public:
    SpeedyFIFOOrderConnection( TTaifexConnection* Dispatcher, 
			       const UFC::AnsiString& OrderFile, 
			       const UFC::AnsiString& TSEConfirm, const UFC::AnsiString& TSEFill,
			       const UFC::AnsiString& OTCconfirm, const UFC::AnsiString& OTCFill );
    virtual ~SpeedyFIFOOrderConnection( void );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
};
//------------------------------------------------------------------------------

#endif	/* _SpeedyFIFOOrderConnection_H */

