#include "SpeedyMarketDataConnectionObject.h"
#include "SpeedyObjectsManager.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/X25Connection.h"
#include "../Migo/Sigo.h"
#include "../UFC/iniFile.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   IsOptions;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 3,8,9 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
SpeedyMarketDataConnectionObject::SpeedyMarketDataConnectionObject( SpeedyObjectsManager* ObjManager,
                                                                    ConnectionParameter& info,
                                                                    ConnectionListener* CListener,
                                                                    UFC::ThreadListener* TListener,
                                                                    BOOL IsOpt    )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, TRUE ),
 FObjectManager( ObjManager ),
 FConnectionListener( CListener ),
 FConnection( NULL ),
 FListen( FALSE ),
 FStart( FALSE ),
 FState( PVC_STATUS_UNKNOWN ),
 FIsOptions( IsOpt )
{
    UFC::AnsiString LineBrokerID = FParameters.GetBrokerID();
    long CmdQID,DataQID;
    long sub = ((LineBrokerID[4] -'0')*256 +(LineBrokerID[5] -'0')*16 + (LineBrokerID[6] -'0'));

    if( FIsOptions == TRUE )
    {
        CmdQID  = 0x73C20000 + sub;
        DataQID = 0x73c00000 + sub;
    }
    else
    {
        CmdQID  = 0x72C20000 + sub;
        DataQID = 0x72c00000 + sub;
    }
    if( !FCommandQueue.Open( CmdQID, true ) )
    {
        UFC::BufferedLog::Printf( " Open PVC10 Command queue failed." );
        exit(-1);
    }
    if( !FDataQueue.Open( DataQID, true ) )
    {
        UFC::BufferedLog::Printf( " Open PVC10 data queue failed." );
        exit(-1);
    }
    FLinkPVCStr.Printf( "Link:%03d PVC:%03d Recv:", info.GetLink(), info.GetPVC() );
    InitMessageLengthTable();
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyMarketDataConnectionObject::~SpeedyMarketDataConnectionObject()
{
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::InitMessageLengthTable( void )
{
    UFC::AnsiString Name,Value;
    UFC::AnsiString SectionName = (FIsOptions == TRUE)?"Option":"Futures";
    UFC::UiniFile   Config( "../cfg/MarketData.cfg" );

    memset( FMsgLenTable, 0 , sizeof(Int32)*64 );
    if( Config.SectionExists( SectionName ) == TRUE )
    {
        for( int i = 0; i < 64; i++ )
        {
            Name.Printf( "%d", i );
            if( Config.GetValue( SectionName, Name, Value ) == TRUE )
                FMsgLenTable[i] = Value.ToInt();
            else
                FMsgLenTable[i] = 0;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_GET_STATE: *((int*)Data ) = FState;
                                break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::Listen( BOOL IsListen )
{
    if( FConnection != NULL )
        FListen = IsListen;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::WriteLog( char* Data, int Size )
{
    ThreadClock::GetTimeString( FLogBuffer, TRUE, TRUE );
    strcat( FLogBuffer, FLinkPVCStr.c_str() );
    strncat( FLogBuffer, Data, Size );
    FObjectManager->GetMarketDataLog()->WriteString( FLogBuffer );
    if( FListen == TRUE )
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, (void*)FLogBuffer );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::CheckCommand()
{
    UFC::UQueueData Command( 0, 256 );

    if( FCommandQueue.RecvNoWait( Command ) == TRUE )
    {
        if( memcmp( Command.GetData(),"33333333333333",14 ) ==  0 )
        {
            FStart = TRUE;
            UFC::BufferedLog::Printf( " %s PVC10 get start command.", FIsOptions?"Options":"Futures" );
        }
        else if( memcmp( Command.GetData(),"44444444444444",14 ) ==  0 )
        {
            FStart = TRUE;
            UFC::BufferedLog::Printf( " %s PVC10 get stop command.", FIsOptions?"Options":"Futures" );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyMarketDataConnectionObject::Execute( void )
{
    char DataBuffer[256];
    int  DataSize;
    UFC::AnsiString LineBrokerID = FParameters.GetBrokerID();
    
    FConnection = new X25Connection( );
    for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ ) 
    {
        try 
        {
            FConnection->Open( FParameters.GetLink(), FParameters.GetPVC() );
            FState = PVC_STATUS_ONLINE_READY;
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY, NULL );
            while( TRUE ) 
            {
                memset( DataBuffer, 0, 256 );
                FState = PVC_STATUS_ONLINE_READY;
                CheckCommand();
                if((DataSize = FConnection->Receive( DataBuffer, 256, 1 )) >= PVC_OK ) ///< Receive a command message
                {
                    Int32 Pos =0;
                    Int32 MsgSize;
                    Int32 MsgBeginPos = 0; 
                    
                    FState = PVC_STATUS_ONLINE_BUSY;
                    for( int i = 0; i < DataSize-1; i++ )
                    {
                    	   if( DataBuffer[i] == 0x0d && DataBuffer[i+1] == 0x0a )
                    	   {
                    	   	  MsgSize = i - MsgBeginPos + 2;
                            UFC::AnsiString DataStr( DataBuffer + MsgBeginPos, MsgSize  );                    	   	
                            MsgBeginPos += MsgSize;
                            i++;
                    	   	  if( FStart == TRUE ) 
                            {
                                UFC::UQueueData QueueData( 1L,(char*)DataStr.c_str(), MsgSize );
                                try 
                                {
                                    FDataQueue.Send( QueueData ); ///< Send Market data to queue.
                                }
                                catch( UFC::QueueException& ) 
                                {
                                    UFC::BufferedLog::Printf( " Add Market Data to queue filled!" );
                                }
                            }
                            MTree           Data;
                            
                            Data.append( "DATA", DataStr );
                            if( IsOptions == TRUE )
                                MessageObj->Send( "MD.OPT", LineBrokerID, Data, FALSE );
                            else
                                MessageObj->Send( "MD.FUT", LineBrokerID, Data, FALSE );
                            WriteLog( DataBuffer + Pos, MsgSize );
                    	   }    
                   }               	   
                }
                else if( DataSize <= PVC_COMMAND ) ///< Is a control command.
                    throw( X25Exception( DataSize, "Receive PVC Reset request" ) );
            }
        }
        catch( X25Exception& e ) 
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FConnection->Close();
            FState = PVC_STATUS_BROKEN;
            UFC::BufferedLog::Printf( " %s. Try to reconnect after 5 sec....\n", e.what() );
            sleep( 5 );
            continue;
        }
        catch( std::exception& ) 
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            return;
        }
    }
    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
    delete FConnection;
    return;
}
//----------------------------------------------------------------------------------------------------------------------
