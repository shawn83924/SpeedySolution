#include "SpeedyFIFOOrderConnection.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL IsLogon;
extern BOOL IsRecoverFinished;   
const Int32 MSG_LENGTH  = 224; ///< 75 + 148 + 1 
const Int32 EXEC_LENGTH = 269; ///< 120 + 148 + 1 
//------------------------------------------------------------------------------
SpeedyFIFOOrderConnection::SpeedyFIFOOrderConnection(   TTaifexConnection*     Connection, 
							const UFC::AnsiString& OrderFile,
							const UFC::AnsiString& TSEConfirm, const UFC::AnsiString& TSEFill,
							const UFC::AnsiString& OTCConfirm, const UFC::AnsiString& OTCFill )	
:UFC::PThread( NULL, FALSE )
,FTSEConfirmFile( NULL )
,FOTCConfirmFile( NULL )
,FTSEFillFile( NULL )
,FOTCFillFile( NULL )
,FRecvFile( NULL )
,FConnection( Connection )
{    
    
    if( UFC::FileExists( TSEConfirm ) == FALSE )
    	FTSEConfirmFile = new UFC::FileStreamEx( TSEConfirm, "w+" );	
    else	
    	FTSEConfirmFile = new UFC::FileStreamEx( TSEConfirm, "a+" );
    if( UFC::FileExists( TSEFill ) == FALSE )
    	FTSEFillFile = new UFC::FileStreamEx( TSEFill, "w+" );	
    else	
    	FTSEFillFile = new UFC::FileStreamEx( TSEFill, "a+" );
    
    if( UFC::FileExists( OTCConfirm ) == FALSE )
    	FOTCConfirmFile = new UFC::FileStreamEx( OTCConfirm, "w+" );	
    else	
    	FOTCConfirmFile = new UFC::FileStreamEx( OTCConfirm, "a+" );
    if( UFC::FileExists( OTCFill ) == FALSE )
    	FOTCFillFile = new UFC::FileStreamEx( OTCFill, "w+" );	
    else	
    	FOTCFillFile = new UFC::FileStreamEx( OTCFill, "a+" );    
    
    if( UFC::FileExists( OrderFile ) == FALSE )
    {
        UFC::AnsiString Commond;	
        Commond.Printf( "touch %s", OrderFile.c_str() );
        system( Commond.c_str() );
    }
    FRecvFile = new UFC::FileStreamEx( OrderFile, "r+" );    		
    FRecvFile->Seek( 0, SEEK_SET );
    Start();
}
//------------------------------------------------------------------------------
SpeedyFIFOOrderConnection::~SpeedyFIFOOrderConnection()
{
    delete FTSEConfirmFile;
    delete FOTCConfirmFile;
    delete FTSEFillFile;
    delete FOTCFillFile;
    delete FRecvFile;
}
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::UpdateNID( UFC::FileStreamEx* OrderFile, int NID )
{
    UFC::AnsiString NIDStr;
    
    NIDStr.Printf( "%010d", NID );
    OrderFile->Seek( -148, SEEK_CUR );
    OrderFile->Write( NIDStr.c_str(), 10 );
    OrderFile->Seek( 138, SEEK_CUR );
}    
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::SendOrder( UFC::FileStreamEx* OrderFile )
{
    if( OrderFile != NULL && IsLogon == TRUE && IsRecoverFinished == TRUE )
    {
        char Buffer[ MSG_LENGTH ];
	int  ReadSize;
	
        memset( Buffer, 0x0, MSG_LENGTH );
        if( (ReadSize = OrderFile->Read( Buffer, MSG_LENGTH )) == MSG_LENGTH ) /// Read 225 bytes  MSG_LENGTH = 225;
        {
            UFC::BufferedLog::DebugPrintf( " Read[%s]", Buffer );                	    
            UFC::AnsiString R010( Buffer, 75  ); ///< length of R010 is 75.                    
	    UFC::AnsiString MessageTime( Buffer + 214, 9 );
	    UFC::AnsiString Order( Buffer , MSG_LENGTH );            
	    UFC::AnsiString UserData( Buffer + 86, 128 );
	    UFC::AnsiString NIDStr( Buffer + 76, 10 );
	    
	    if( NIDStr.ToInt() == 0 )
	    {
		nsOrderMessageDefine::MarketEnum Market = (Order[ 75 ]=='T')? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC;

		UFC::BufferedLog::DebugPrintf( " MessageTime[%s]", MessageTime.c_str() );                        
		UFC::BufferedLog::DebugPrintf( " R010[%s]", R010.c_str() );
		UFC::BufferedLog::DebugPrintf( " Market[%c]", Order[ 75 ] );
		UFC::BufferedLog::DebugPrintf( " UserData[%s]", UserData.c_str() );
		if( Order[3] == '1' || Order[3] == '2') ///< New order
		{
		    TNewOrderMessage NewOrder;
		    
		    NewOrder.SetMarket( Market );                        
		    NewOrder.SetSrc( R010.c_str() );
		    NewOrder.SetData( UserData.c_str() );		    
		    FConnection->NewOrder( &NewOrder );                            
		    UpdateNID( OrderFile, NewOrder.GetNID() );
		}
		else if( Order[3] == '3' ) ///< Reduce Qty
		{
		    TReplaceOrderMessage Replace;
		    
		    Replace.SetMarket( Market );
		    Replace.SetSrc( R010.c_str() );
		    Replace.SetData( UserData.c_str() );		    
		    FConnection->ReplaceOrder( &Replace );
		    UpdateNID( OrderFile,Replace.GetNID() );
		}
		else if( Order[3] == '4' )///< Order Cancel
		{
		    TCancelOrderMessage Cancel;
		    
		    Cancel.SetMarket( Market );
		    Cancel.SetSrc( R010.c_str() );
		    Cancel.SetData( UserData.c_str() );
		    FConnection->CancelOrder( &Cancel );
		    UpdateNID( OrderFile, Cancel.GetNID() );
		}
		else if( Order[3] == '5' )///< Order Status request
		{
		    TOrderStatusRequest Request;                                               

		    Request.SetMarket( Market );                        
		    Request.SetSrc( R010.c_str() );
		    Request.SetData( UserData.c_str() );
		    FConnection->OrderStatusRequest( &Request );
		    UpdateNID( OrderFile, Request.GetNID() );
		}                        				
	    }	
        }
        else ///< ReadSize <= 0 EOF
            UFC::SleepMS( 1 );
    }
    else
        UFC::SleepMS( 100 );    
}
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::Execute( void )
{
    
    while( TRUE )
    {            
        try
        {
            SendOrder( FRecvFile );
        }   
        catch( UFC::Exception& ex )
        {
            UFC::BufferedLog::Printf( "Exception:[%s]", ex.what() );
        }        
    }
}
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    char ExecData[ EXEC_LENGTH ];
    UFC::AnsiString Body( Msg->GetSrc() );
    UFC::AnsiString UserDef( Msg->GetData() );
    UFC::AnsiString NIDStr, MessageTime;
    char            Market = ( Msg->GetMarket() == nsOrderMessageDefine::mTSE )? 'T' :'O';
    
    Body.PadThis( 120, ' ' );
    NIDStr.Printf( "%010d", Msg->GetNID() );
    UserDef.PadThis( 128, ' ' );
    UFC::GetTimeString( MessageTime, FALSE );
    sprintf( ExecData, "%s%c%s%s%s",  Body.c_str(),  Market, NIDStr.c_str(), UserDef.c_str(), MessageTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " Filled:[%s]", ExecData );    	    
    ExecData[ EXEC_LENGTH -1 ] = 0x0A;    
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE )
    {
	FTSEFillFile->Write( ExecData, EXEC_LENGTH );
	FTSEFillFile->Flush();
    }
    else
    {
	FOTCFillFile->Write( ExecData, EXEC_LENGTH );
	FOTCFillFile->Flush();
    }    
}    
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{		
    char ExecData[ EXEC_LENGTH ];
    UFC::AnsiString Body( Msg->GetSrc() );
    UFC::AnsiString UserDef( Msg->GetData() );
    UFC::AnsiString NIDStr, MessageTime;
    char            Market = ( Msg->GetMarket() == nsOrderMessageDefine::mTSE )? 'T' :'O';
    
    Body.PadThis( 120, ' ' );
    NIDStr.Printf( "%010d", Msg->GetNID() );
    UserDef.PadThis( 128, ' ' );
    UFC::GetTimeString( MessageTime, FALSE );
    sprintf( ExecData, "%s%c%s%s%s",  Body.c_str(),  Market, NIDStr.c_str(), UserDef.c_str(), MessageTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " Confirm:[%s]", ExecData );    	    
    ExecData[ EXEC_LENGTH -1 ] = 0x0A;     	
    	
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE )
    {
	FTSEConfirmFile->Write( ExecData, EXEC_LENGTH );
	FTSEConfirmFile->Flush();
    }
    else
    {
	FOTCConfirmFile->Write( ExecData, EXEC_LENGTH );
	FOTCConfirmFile->Flush();
    }    
}
//------------------------------------------------------------------------------






