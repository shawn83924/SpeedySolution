#include "SpeedyFIFORiskManager.h"
#include "NameValueMessage.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
Int32 patsDefaultUserDataLength = 145;
UFC::UiniFile ErrorCodeTable( "../cfg/RMErrorCode.ini" );

//---------------------------- SpeedyFIFOConnection ----------------------------
UFC::PReadWriteLock SpeedyFIFOConnection::FNIDLock;
UFC::PHashedSet<Int32> SpeedyFIFOConnection::ProcessedNIDSet;
UFC::AnsiString SpeedyFIFOConnection::FProcessedNIDFileName = "";
UFC::FileStreamEx *SpeedyFIFOConnection::FProcessedNIDFilePtr = 0;
    
//------------------------------------------------------------------------------
SpeedyFIFOConnection::SpeedyFIFOConnection( const UFC::AnsiString& SendPipeName, const UFC::AnsiString& RecvPipeName, MessageObject* MessageObj, int MessageLength, bool StartThread )
:UFC::PThread( NULL, FALSE )
,FMessageObject( MessageObj )
,FSendFIFOName( SendPipeName )
,FRecvFIFOName( RecvPipeName )
,FMessageLength(MessageLength)
{
    UFC::AnsiString logHead = "SpeedyFIFOConnection()";
    UFC::AnsiString curPipeName = FSendFIFOName;
    try 
    {
        FSendPipe = new UFC::FIFO( curPipeName, UFC::FIFO_NonBlockWriteOnly );//UFC::FIFO_NonBlockReadOnly );    
        
        curPipeName = FRecvFIFOName;
        FRecvPipe = new UFC::FIFO( curPipeName, UFC::FIFO_NonBlockReadOnly );//UFC::FIFO_NonBlockWriteOnly );
    }
    catch (UFC::Exception e)
    {
        UFC::BufferedLog::Printf(" %s open Pipe[%s] failed. Exception:[%s]", logHead.c_str(), curPipeName.c_str(), e.what() );
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
    
    if (StartThread) this->Start();
}
//------------------------------------------------------------------------------
SpeedyFIFOConnection::~SpeedyFIFOConnection()
{
    delete FSendPipe;
    delete FRecvPipe;
}
//------------------------------------------------------------------------------
void SpeedyFIFOConnection::Execute( void )
{
    char Buffer[ PIPE_BUF + 1 ];
    while( TRUE )
    {
        memset( Buffer, 0x0, PIPE_BUF + 1 );
        try
        {
            int ReadSize = FRecvPipe->Read( Buffer, PIPE_BUF );
            if( ReadSize > 0 )
            {
                if( ReadSize % FMessageLength == 0 )
                {
                    int MsgCount = ReadSize / FMessageLength;
                    
                    for( register int i = 0; i < MsgCount; i++ )
                    {
                        UFC::AnsiString Result( Buffer + i * FMessageLength, FMessageLength );
                        OnRecvMessage( Result );
                    }
                }
            }
            else
                UFC::SleepMS( 1 );
        }
        catch( UFC::Exception& ex )
        {
            UFC::BufferedLog::Printf( "Exception:[%s]", ex.what() );
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyFIFOConnection::OnRecvMessage( const UFC::AnsiString& Message )
{
    UFC::BufferedLog::Printf( " Recv Pipe:[%s] %d", Message.c_str(), Message.Length() );
    int recvMsgLength = Message.Length();
    UFC::AnsiString errorCode, nidStr, msgKey, userKey, orderMsg, orderID, channel = "", subAccount = "";
    char msgMarket;
    
    if (recvMsgLength == MSG_LENGTH) //75+4+1+10+128+15+1=376
    {
        orderMsg = Message.SubString( 0, 75 );
        errorCode = Message.SubString( 75, 4 );
        msgMarket = Message[79];
        nidStr = Message.SubString( 80, 10 );
        msgKey = Message.SubString( 90, 270 );
        subAccount = Message.SubString( 360, 15 );
        subAccount.TrimRight(' ');
        
        orderID = orderMsg.SubString( 21, 5 );
    }
    else if (recvMsgLength == PATS_BACKEND_REPLY_LENGTH)
    {
        Int32 curStartIndex = 0;
        orderMsg = Message.SubString( curStartIndex, PATS_ORDER_LENGTH );
        orderMsg.TrimRight(' ');
        
        curStartIndex += PATS_ORDER_LENGTH;  //320
        errorCode = Message.SubString( curStartIndex, PATS_ERRORCODE_LENGTH );
        
        curStartIndex += PATS_ERRORCODE_LENGTH;  //324
        msgMarket = Message[curStartIndex];
        
        curStartIndex += 1;  //325
        nidStr = Message.SubString( curStartIndex, PATS_NID_LENGTH );
        
        curStartIndex += PATS_NID_LENGTH;  //335
        UFC::AnsiString tempUserData = Message.SubString( curStartIndex, PATS_USERDATA_LENGTH );
        tempUserData.TrimRight(' ');
        
        curStartIndex += PATS_USERDATA_LENGTH;  //605
        curStartIndex += PATS_ORDERTIME_LENGTH;  //614
        channel = Message.SubString( curStartIndex, PATS_CHANNEL_LENGTH );
        channel.TrimRight(' ');
        
        curStartIndex += PATS_CHANNEL_LENGTH;  //646
        subAccount = Message.SubString( curStartIndex, PATS_SUBACCOUNT_LENGTH );
        subAccount.TrimRight(' ');

        curStartIndex += PATS_SUBACCOUNT_LENGTH;  //661
        UFC::AnsiString tempUserDataLengthStr = Message.SubString( curStartIndex, PATS_USERDATALEN_LENGTH );
        tempUserDataLengthStr.TrimRight(' ');
        int tempUserDataLength = tempUserDataLengthStr.ToInt();
        if (tempUserData.Length() < tempUserDataLength)
            tempUserData.PadThis(tempUserDataLength, ' ');
        else
            tempUserData.SetLength(tempUserDataLength);
        
        if ((subAccount.Length() <= 0) || (tempUserDataLength <= 0))
            msgKey = "";
        else if ((subAccount.Length() <= 0) && (tempUserDataLength > 0))
            msgKey.Printf(",%s", tempUserData.c_str());            
        else if ((subAccount.Length() > 0) && (tempUserDataLength <= 0))
            msgKey = subAccount;
        else
            msgKey.Printf("%s,%s", subAccount.c_str(), tempUserData.c_str());            

        UFC::NameValueMessage orderFieldList("^\n");
        orderFieldList.FromString( orderMsg );
        if (orderFieldList.Get( "oid", orderID ) == FALSE)
        {
            UFC::BufferedLog::Printf( " PATS order miss oid field." );
            return;
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " wrong message length. %d[%s]", recvMsgLength, Message.c_str() );
        return;
    }
        
    Int32 NID = nidStr.ToInt();
    if( msgKey.AnsiPos( "<Proxy:" ) != -1 )
    {
        UFC::AnsiString AE( msgKey.c_str(), 9 );           ///< AE length = 8
        UFC::AnsiString PMFData(  msgKey.c_str() + 9, 94 ); ///< PMF Data Length = 94 
        UFC::AnsiString Proxy( msgKey.c_str() + 9 + 94 ); ///< Proxy length = 25
        userKey.Printf( "%s%s%s", AE.c_str(), Proxy.c_str(), PMFData.c_str() );
    }
    else
    {
        if( ( recvMsgLength == MSG_LENGTH ) && ( subAccount.Length() > 0 ) )
            userKey.Printf( "%s,%s", subAccount.c_str(), msgKey.c_str() );            
        else
            userKey = msgKey;
    }
    UFC::BufferedLog::Printf( " NID:[%010d] OrderID:[%s] ErrorCode:[%s] Market:[%c] Key:[%s]", NID, orderID.c_str(), errorCode.c_str(), msgMarket, userKey.c_str() );
        
    MTree Data;
    
    Data.append( "NID", NID );
    if (userKey.Length() > 0) Data.append( "KEY", userKey );
    if (recvMsgLength == MSG_LENGTH)
        Data.append( "ORDER", Message.SubString( 0, 75 ) );
    else if (recvMsgLength == PATS_BACKEND_REPLY_LENGTH)
        Data.append( "ORDER", orderMsg );
    Data.append( "OID", orderID );
    if (channel.Length() > 0) Data.append( "CHANNEL", channel );
    
    if( errorCode.AnsiCompare( "0000" ) == 0 ) ///< Pass backend
        Data.append( "CODE", 1 );
    else
    {
        UFC::AnsiString MSG;
        if( ErrorCodeTable.GetValue( "Error", errorCode, MSG ) )
            UFC::BufferedLog::Printf( " ErrorCode:[%s] %s", errorCode.c_str(), MSG.c_str() );
        else
            MSG.Printf( "ErrorCode:[%s]", errorCode.c_str() );
        Data.append( "MSG", MSG );
        Data.append( "CODE", 0 );
    }
    if( msgMarket == 'F' )
    {
        Data.append( "OFFHOUR", 0 );
        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, Data ); 
    }
    else if( msgMarket == 'f' )
    {
        Data.append( "OFFHOUR", 1 );
        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, Data ); 
    }
    else if( msgMarket == 'O' )
    {
        Data.append( "OFFHOUR", 0 );
        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, Data ); 
    }
    else if( msgMarket == 'o' )
    {
        Data.append( "OFFHOUR", 1 );
        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, Data ); 
    }
    else if( msgMarket == 'P' )
        FMessageObject->Send( "RESULT.PATS", UFC::Hostname, Data ); 
    else
        UFC::BufferedLog::Printf( " wrong Market Character[%c].", msgMarket );
}
//------------------------------------------------------------------------------
void SpeedyFIFOConnection::OnMessage( const UFC::AnsiString& SendMsg, Market mkt, MTree* Data )
{
    int writeCount = FSendPipe->Write( SendMsg.c_str(), SendMsg.Length() );
    
    if( writeCount < 0 )
    {
        UFC::BufferedLog::Printf( " SpeedyFIFOConnection::OnMessage() Write to FIFO Pipe Failed. ErrorNo:%d", errno );
        Data->append( "CODE", 0 );
        Data->append( "MSG", "Can not send this order to backend" );
        if( mkt == mFutures )
            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
        else if( mkt == mOptions )
            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
        else if( mkt == mPATSFutures )
            FMessageObject->Send( "RESULT.PATS", UFC::Hostname, *Data );
    }
    else
    {
        Int32 networkID = 0;
        if( Data->get( "NID", networkID ) == TRUE )
        {
            if( networkID > 0 ) ProcessedNIDSet.Add( networkID );
        }
        UFC::BufferedLog::Printf( " SpeedyFIFOConnection::OnMessage() Write %d bytes to FIFO Pipe. NID[%d]", writeCount, networkID );
    }  //SpeedyFIFOConnection::OnMessage()
}
//------------------------------------------------------------------------------






