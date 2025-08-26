#include "SpeedyFIFOOrderConnection.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL IsLogon;
extern BOOL IsRecoverFinished;   
const Int32 MSG_LENGTH = 205;
//------------------------------------------------------------------------------
SpeedyFIFOOrderConnection::SpeedyFIFOOrderConnection( TTaifexConnection* Connection, const UFC::AnsiString& OrderFile, const UFC::AnsiString FUTExec, const UFC::AnsiString& OPTExec )
:UFC::PThread( NULL, FALSE )
,FFUTSendFile( NULL )
,FOPTSendFile( NULL )
,FRecvFile( NULL )
,FConnection( Connection )
{    
    UFC::FileStreamEx* RecvFile;
    if( UFC::FileExists( FUTExec ) == FALSE )
    	FFUTSendFile = new UFC::FileStreamEx( FUTExec, "w+" );	
    else	
    	FFUTSendFile = new UFC::FileStreamEx( FUTExec, "a+" );

    if( UFC::FileExists( OPTExec ) == FALSE )
    	FOPTSendFile = new UFC::FileStreamEx( OPTExec, "w+" );	
    else	
    	FOPTSendFile = new UFC::FileStreamEx( OPTExec, "a+" );
    		
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
    delete FFUTSendFile;
    delete FOPTSendFile;
    delete FRecvFile;
}
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::SendOrder( UFC::FileStreamEx* OrderFile )
{
    if( OrderFile != NULL && IsLogon == TRUE && IsRecoverFinished == TRUE )
    {
        char Buffer[ MSG_LENGTH ];
        memset( Buffer, 0x0, MSG_LENGTH );
        int  ReadSize = OrderFile->Read( Buffer, MSG_LENGTH );/// Read 205 bytes  MSG_LENGTH = 205;
        
        if( ReadSize > 0 )
        {
            UFC::BufferedLog::DebugPrintf( " Read[%s]", Buffer );                	    
            UFC::AnsiString R010( Buffer, 75  ); ///< length of R010 is 75.                    
	    UFC::AnsiString MessageTime( Buffer + 214, 9 );
	    UFC::AnsiString Order( Buffer , MSG_LENGTH );            
	    UFC::AnsiString UserData( Order.SubString( 86, 128 ) );
	    
	    UFC::BufferedLog::DebugPrintf( " MessageTime[%s]", MessageTime.c_str() );                        
	    if( MessageTime.ToInt() == 0 )
	    {
		nsOrderMessageDefine::MarketEnum Market = (Order[ 75 ]=='T')? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC;

		UFC::GetTimeString( MessageTime, FALSE );
		UFC::BufferedLog::DebugPrintf( " MessageTime[%s]", MessageTime.c_str() );                        
		UFC::BufferedLog::DebugPrintf( " R010[%s]", R010.c_str() );
		UFC::BufferedLog::DebugPrintf( " Market[%c]", Order[ 75 ] );
		UFC::BufferedLog::DebugPrintf( " UserData[%s]", UserData.c_str() );
		if( Order[3] == '1' || Order[3] == '2') ///< New order
		{
		    TNewOrderMessage NewOrder;
		    NewOrder.SetSrc( R010.c_str() );
		    NewOrder.SetData( UserData.c_str() );
		    NewOrder.SetMarket( Market );                        
		    FConnection->NewOrder( &NewOrder );                            
		}
		else if( Order[3] == '3' ) ///< Replace Qty
		{
		    TReplaceOrderMessage Replace;
		    Replace.SetSrc( R010.c_str() );
		    Replace.SetData( UserData.c_str() );
		    Replace.SetMarket( Market );
		    FConnection->ReplaceOrder( &Replace );
		    SendAck( R010, UserData,Market );
		}
		else if( Order[3] == '4' )///< Order Cancel
		{
		    TCancelOrderMessage Cancel;
		    Cancel.SetSrc( R010.c_str() );
		    Cancel.SetData( UserData.c_str() );
		    Cancel.SetMarket( Market );
		    FConnection->CancelOrder( &Cancel );
		    SendAck( R010, UserData, Market );
		}
		else if( Order[3] == '5' )///< Order Status request
		{
		    TOrderStatusRequest Request;                                               
		    UFC::AnsiString OrderID( Order.SubString( 21, 5 ) );		
		    UFC::AnsiString Account( Order.SubString( 26, 7 ) );
		    UFC::AnsiString Symbol( Order.SubString( 34, 20 ) );
		    UFC::AnsiString BrokerID( Order.SubString( 14, 7 ) );

		    Request.SetMarket( Market );                        
		    Request.SetSymbol( Symbol );
		    Request.SetOrderID( OrderID );
		    Request.SetAccount( Account );
		    Request.SetBrokerID( BrokerID );
		    Symbol.TrimRight();
		    Request.SetSide( ( Order[41]== 'B') ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
		    Request.SetData( UserData.c_str() );
		    FConnection->OrderStatusRequest( &Request );
		}                        
		OrderFile->Seek( -MSG_LENGTH + 6, SEEK_CUR );
		OrderFile->Write( MessageTime.c_str(), 6 );
		OrderFile->Seek( MSG_LENGTH-12, SEEK_CUR );
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
void SpeedyFIFOOrderConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{		
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osReplaced && (  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 ) ) 
        return; ///< Replace Px don't neeed to write execution.
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osCanceled ||
        Msg->GetOrderStatus() == nsOrderMessageDefine::osReplaced ||
        (Msg->GetOrderStatus() == nsOrderMessageDefine::osRejected && !strcmp( Msg->GetStatusCode(), "32" )	) )
    {
    	UFC::AnsiString PVC( Msg->GetPVC() );
    	UFC::AnsiString C030( Msg->GetSrc() );
    	UFC::AnsiString BackEndMsg;
    		
    	if( PVC.Length() > 2 )
            PVC.Delete( 0, PVC.Length() - 2 );    
    	if( C030[0] == '1' ) 
    	{
            C030[69] = PVC[0];
            C030[70] = PVC[1];
    	}
    	else if( C030[0] == '2' )
    	{
            C030[117] = PVC[0];
            C030[118] = PVC[1];
    	}
    	else if( C030[0] == '3' || C030[0] == '4' )
    	{
            C030[65] = PVC[0];
            C030[66] = PVC[1];
    	}    	
    	BackEndMsg.Printf("%s%s",C030.c_str(),Msg->GetData());
    	BackEndMsg.PadThis( 192, ' ' );
    	BackEndMsg += "\n";
    	UFC::BufferedLog::Printf( " C030:[%s]", BackEndMsg.c_str() ); 
    	
    	if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
    	{
            FFUTSendFile->Write( BackEndMsg.c_str(), 193 );
            FFUTSendFile->Flush();
	}
        else
	{
            FOPTSendFile->Write( BackEndMsg.c_str(), 193 );
            FOPTSendFile->Flush();
	}
    }
    else
    {
    	UFC::AnsiString R020;
    		
    	R020.Printf("%s%s", Msg->GetSrc(), Msg->GetData());
    	R020.PadThis( 192, ' ' );
    	R020 += "\n";
    	
    	UFC::BufferedLog::DebugPrintf( " R020:[%s]", R020.c_str() );    	
    	if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
    	{
            FFUTSendFile->Write( R020.c_str(), 193 );
            FFUTSendFile->Flush();
	}
	else
	{
            FOPTSendFile->Write( R020.c_str(), 193 );
            FOPTSendFile->Flush();
	}
    }
}
//------------------------------------------------------------------------------
void SpeedyFIFOOrderConnection::SendAck( const UFC::AnsiString& R010,const UFC::AnsiString& UserData, char Market )
{
    UFC::AnsiString R020;		
    UFC::AnsiString Now;
    
    UFC::GetTimeString( Now, FALSE );
    R020.Printf( "%s00000000%8s00000000%s", R010.c_str(), Now.SubString( 0, 8 ).c_str(), UserData.c_str() );
    R020.PadThis( 192, ' ' );
    R020 += "\n";
    UFC::BufferedLog::DebugPrintf( " Ack:[%s]", R020.c_str() );
    if( Market == 'F')
    {
	FFUTSendFile->Write( R020, 193 );
	FFUTSendFile->Flush();
    }
    else
    {
	FOPTSendFile->Write( R020, 193 );
	FOPTSendFile->Flush();
    }
}
//------------------------------------------------------------------------------






