#include "SocketSend.h"
//---------------------------------------------------------------------------
unsigned char a2eG[] =
{
      0,  1,  2,  3, 55, 45, 46, 47, 22,  5, 37, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
     64, 79,127,123, 91,108, 80,125, 77, 93, 92, 78,107, 96, 75, 97,
    240,241,242,243,244,245,246,247,248,249,122, 94, 76,126,110,111,
    124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
    215,216,217,226,227,228,229,230,231,232,233, 74,224, 90, 95,109,
    121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
    151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,  7,
     32, 33, 34, 35, 36, 21,  6, 23, 40, 41, 42, 43, 44,  9, 10, 27,
     48, 49, 26, 51, 52, 53, 54,  8, 56, 57, 58, 59,  4, 20, 62,225,
     65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
     88, 89, 98, 99,100,101,102,103,104,105,112,113,114,115,116,117,
    118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
    159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
    184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
    220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};
//---------------------------------------------------------------------------
void AsciiToEbcdic( unsigned char* AsciiStr, int size )
{
    for( register int i = 0; i < size; i++ )
        *(AsciiStr+i) = a2eG[ *(AsciiStr+i) ];
}
//---------------------------------------------------------------------------
// Class Setting
//---------------------------------------------------------------------------
Setting::Setting(  UFC::Section* pSet  )
{
    UFC::AnsiString Value;

    WebID = pSet->GetSectionName();

    pSet->GetValue( "IP", IPAddress );
    
    pSet->GetValue( "Port", Value);
    Port = Value.ToInt();
    
    pSet->GetValue( "HeartBeatInterval", Value);
    HBInterval = Value.ToInt();
    
    pSet->GetValue( "ListenPort", Value);
    ListenPort = Value.ToInt();
}
//---------------------------------------------------------------------------
// Class SocketSend
//
//---------------------------------------------------------------------------
SocketSend::SocketSend(Setting* Set, bool FirstTimeRunning, MessageObject* pSigo, UFC::PtrQueue<UFC::AnsiString>* JobQueue)
:UFC::PThread( NULL, FALSE )
,FClientStat( st_DISCONNECT )
,FServerStat( st_DISCONNECT )
,pSet( Set )
,MessageSeq( Set->WebID )
,FMessageObject( pSigo )
,FSocket( Set->IPAddress, Set->Port, FALSE)
,FJobQueue( JobQueue )
{
    memset( FAliveMessage, 0x20, 260 );
    memcpy( FAliveMessage, "*ALIVE", 6 );
    AsciiToEbcdic( (unsigned char*)FAliveMessage, 260 );
    memset( FOkMessage, 0x20, 260 );
    memcmp( FOkMessage, "*OK", 3 );
    AsciiToEbcdic( (unsigned char*)FOkMessage, 260 );
    
    if( FirstTimeRunning == true )
        MessageSeq = 1;

    if( MessageSeq.ToInt32() == 0 )
        MessageSeq = 1;

    UFC::BufferedLog::Printf(" Message sequence = [%d]", MessageSeq.ToInt32());
}
//---------------------------------------------------------------------------
bool SocketSend::CreateSocketObject(void)
{
    char Buffer[512];

    try
    {
        if( !FSocket.IsConnect() )
        {
            UFC::BufferedLog::Printf( " Try to connect to server %s:%d", (pSet->IPAddress).c_str(), pSet->Port);
            FSocket.Connect( pSet->IPAddress, pSet->Port, pSet->HBInterval );

            try
            {
                memset( Buffer ,0 , 512 );
                int ReadByte = FSocket.RecvBuffer((UInt8*)Buffer, 512);
                UFC::BufferedLog::DebugPrintf( " Receive %d byte(s).", ReadByte );
                if( ReadByte > 0 )
                {
                    UFC::BufferedLog::DebugPrintf( " Server connection information:[%s]",  Buffer );
                    UFC::BufferedLog::DebugPrintf( " Change Client Socket to connection state.");
                    FClientStat = st_CONNECT;
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf(" Flow engine doesn't receive ack. The connection will be closed.");
                    FSocket.Disconnect();
                }
            }
            catch(...)
            {
                  UFC::BufferedLog::DebugPrintf( " Order line connect fail!" );
                  FSocket.Disconnect();
                  FClientStat = st_DISCONNECT;
            }
        }
        else
           UFC::BufferedLog::DebugPrintf( " Client socket is connected" );
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Client connect to AS/400 %s:%d failed!",(pSet->IPAddress).c_str(), pSet->Port);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void SocketSend::DequeueFunc(void)
{
    UFC::AnsiString*         SendData;

    while( (SendData = FJobQueue->Dequeue( pSet->HBInterval ) )!=NULL )
    {
        SendRejectOrderMessage( *SendData );
        delete SendData;
    }
}
//---------------------------------------------------------------------------
void SocketSend::HeartBeat(void)
{
    try
    {
        FSocket.SendBuffer( FAliveMessage, 260 );
    }
    catch(UFC::SocketException& e)
    {
        UFC::BufferedLog::Printf( " Heartbeat socket exception:[%s]", e.what() );
        FSocket.Disconnect( true );
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Send HeartBeat Failed.");
        FSocket.Disconnect( true );
    }
}
//---------------------------------------------------------------------------
void SocketSend::SendRejectOrderMessage( const UFC::AnsiString& Order )
{
    int  NetworkID = Order.SubString( 89, 8 ).ToInt();
    int  OrderQty = Order.SubString( 74, 4 ).ToInt();
    char MarketKind = Order[97];
    char PositionEffect = Order[79];
    char Side = Order[63];
    char IsMarket = Order[64];
    char OrderCondition = Order[78];
            
    UFC::AnsiString RejectMessage;
    UFC::AnsiString Today;
    UFC::AnsiString Now;
    UFC::AnsiString TransationKind = Order.SubString( 11, 2 );
    UFC::AnsiString BrokerID = Order.SubString( 23, 7 );
    UFC::AnsiString Account = Order.SubString( 41, 7 );
    UFC::AnsiString Symbol = Order.SubString( 43, 20 );
    UFC::AnsiString WebID = Order.SubString( 86, 3 );   
    MTree           Data;

    GetYYYYMMDD( Today );
    GetHHMMSS( Now );
    
    
    UFC::BufferedLog::DebugPrintf( " Reject Order. NID = [%d] Market = [%s]", NetworkID, (MarketKind == '1' || MarketKind == '4')?"Futures":"Options" );
    Data.append( "NID", NetworkID );
    
    if( MarketKind == '1' || MarketKind == '4' ) ///< Futures market
    {
        RejectMessage.Printf( "NITOFUO  30%2s01%6s88%7s00000%7s0%20s%c%c+00000000%04d%c%cABCD123412341234567812345678((%3s%8d%c1234560%3s123456781234567Broken connection between gateway and backend system.       12311123456           ))%8s%6s1234567890 12345678", 
                                                                TransationKind.c_str(), 
                                                                Now.c_str(), 
                                                                BrokerID.c_str(), 
                                                                Account.c_str(), 
                                                                Symbol.c_str(),
                                                                Side,
                                                                IsMarket,
                                                                OrderQty,
                                                                OrderCondition,
                                                                PositionEffect,
                                                                WebID.c_str(),
                                                                NetworkID,
                                                                MarketKind,
                                                                WebID.c_str(),
                                                                Today.c_str(),
                                                                Now.c_str()
                                                                );
        UFC::BufferedLog::DebugPrintf( " Length of reject message = [%d]", RejectMessage.Length() );
        Data.append("REPLY",RejectMessage);
        FMessageObject->Send( "REPLY.FUT", UFC::Hostname, Data );
    }
    else if( MarketKind == '2' || MarketKind == '3' ) ///< Option market
    {
        RejectMessage.Printf( "NITOFUO  93%2s01%6s88%7s00000%7s0%20s%c%c+00000000%04d%c%cABCD123412341234567812345678((%3s%8d%c1234560%3s123456781234567Broken connection between gateway and backend system.       12311123456           ))%8s%6s1234567890 12345678", 
                                                                TransationKind.c_str(), 
                                                                Now.c_str(), 
                                                                BrokerID.c_str(), 
                                                                Account.c_str(), 
                                                                Symbol.c_str(),
                                                                Side,
                                                                IsMarket,
                                                                OrderQty,
                                                                OrderCondition,
                                                                PositionEffect,
                                                                WebID.c_str(),
                                                                NetworkID,
                                                                MarketKind,
                                                                WebID.c_str(),
                                                                Today.c_str(),
                                                                Now.c_str()
                                                                );
        UFC::BufferedLog::DebugPrintf( " Length of reject message = [%d]", RejectMessage.Length() );
        Data.append( "REPLY", RejectMessage );
        FMessageObject->Send( "REPLY.OPT", UFC::Hostname, Data );
    }
}
//---------------------------------------------------------------------------
bool SocketSend::CheckConnction(void)
{
    if( FClientStat != st_CONNECT )
        return CreateSocketObject();
    
    return true;
    
}
//---------------------------------------------------------------------------
void SocketSend::Work( UFC::AnsiString* SendData )
{
    char Buffer[260];

    memset( Buffer, 0, 260 );
    memcpy( Buffer, SendData->c_str(), SendData->Length() );
    UFC::BufferedLog::Printf( " Send:[%s]", SendData->c_str() );
    UFC::BufferedLog::FlushToFile();
    AsciiToEbcdic( (unsigned char*)Buffer, 260 );
    FSocket.SendBuffer( Buffer, 260 );
    try
    {
        memset( Buffer, 0, 260);
        int temp = FSocket.RecvBuffer((UInt8*)Buffer, 260);

        if(temp > 0 )
        {
            UFC::BufferedLog::DebugPrintf( " Received ack:[%s]",  Buffer);
        }
        else
        {
            UFC::BufferedLog::DebugPrintf(" AS/400 doesn't send back \"*OK\"");

        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Received fail!" );
        FClientStat = st_DISCONNECT;
        FSocket.Disconnect();
    }

}
//---------------------------------------------------------------------------
void SocketSend::Execute(void)
{
    ///< 2 Thread handle up to 2 connectoion
    FServer = new UFC::MultiplexingServer(pSet->ListenPort, 2, 2 );
    FServer->EnableHeartbeat(false);
    FServer->SetListener( this );
    try
    {
       FServer->Run();
       CreateSocketObject();
    }
    catch(UFC::SocketException &ex)
    {
        UFC::BufferedLog::Printf( " Socket Exception: %s",ex.what());
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Unknoewn Exception.");
    }

    UFC::AnsiString* SendData;
    while( TRUE )
    {
        if( CheckConnction() )
        {
            if( (SendData = FJobQueue->Dequeue( pSet->HBInterval )) != NULL )
            {
                try
                {
                    Work( SendData );
                    delete SendData;
                }
                catch(...)
                {
                    if( SendData!=NULL )
                    {
                        SendRejectOrderMessage( *SendData );
                        delete SendData;
                    }
                }
            }
            else
                HeartBeat();
        }
        else
            DequeueFunc();
    }
}
//---------------------------------------------------------------------------
void SocketSend::OnListen( UFC::MultiplexingServer * ServSocket )
{
    UFC::BufferedLog::Printf(" Listening Port: [%d]...",ServSocket->GetPort());
}
//---------------------------------------------------------------------------
void SocketSend::OnClientConnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
    char        Buffer[260];
    char        Temp[8];

    UFC::BufferedLog::Printf(" ReportSocket: Client connect from IP:%s Port:%d",
                             ClientSocket->GetPeerIPAddress().c_str(),
                             ClientSocket->GetPeerPort() );
    FServerStat = st_CONNECT;
    memset( Buffer, 0x20, 260 );
    if( sprintf( Temp, "*%06d", MessageSeq.ToInt32()) )
    {
        memcpy( Buffer, Temp, 7 );
        AsciiToEbcdic( (unsigned char*)Buffer, 260 );
        ClientSocket->SendBuffer( Buffer, 260 );
    }
}
//---------------------------------------------------------------------------
void SocketSend::OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
     UFC::BufferedLog::Printf(" Client Disconnected(IP:%s Port:%d)",
                               ClientSocket->GetPeerIPAddress().c_str(),
                               ClientSocket->GetPeerPort());
     FServerStat = st_DISCONNECT;
}
//---------------------------------------------------------------------------
void SocketSend::OnConfirmExecution( const UFC::AnsiString& Confirm )
{
    MTree                    Data;
    int                      NetworkID = Confirm.SubString( 113, 8 ).ToInt();
    char                     MarketKind = Confirm[121];
    UFC::AnsiString          WebID = Confirm.SubString( 110, 3 );
    
    UFC::BufferedLog::DebugPrintf( " OnConfirmExecution: WebID = [%s] NID = [%d] Market = %s", WebID.c_str(), NetworkID, ( MarketKind == '1' || MarketKind == '4' )?"Futures":"Options" );
    
    if( WebID == pSet->WebID || WebID == "LIV" )
    {
        Data.append( "REPLY", Confirm );
        Data.append( "NID", NetworkID );
        if( MarketKind == '1' || MarketKind == '4' ) ///< Futures
        {    
            UFC::BufferedLog::DebugPrintf(" FUT confirm");    
            FMessageObject->Send( "REPLY.FUT", UFC::Hostname, Data );
        }
        else if( MarketKind == '2' || MarketKind == '3' ) ///<Options
        {
            UFC::BufferedLog::DebugPrintf(" OPT confirm");
            FMessageObject->Send( "REPLY.OPT", UFC::Hostname, Data );        
        }    
        else
            UFC::BufferedLog::Printf( " Receive unhandled confirm message." );
    }
}
//---------------------------------------------------------------------------
void SocketSend::OnFilledExecution( const UFC::AnsiString& Filled )
{
    MTree                    Data;
    int                      NetworkID = Filled.SubString( 155, 8 ).ToInt();
    char                     MarketKind = Filled[23];
    UFC::AnsiString          WebID = Filled.SubString( 152, 3 );

    UFC::BufferedLog::DebugPrintf( " OnFilledExecution: WebID = [%s] NID = [%d] Market = %s", WebID.c_str(), NetworkID, ( MarketKind == '1' || MarketKind == '4' )?"Futures":"Options" );
    
    if( WebID == pSet->WebID || WebID == "LIV" )
    {
        Data.append( "FILL_ORDER", Filled );
        Data.append( "NID", NetworkID );  
        if( MarketKind == '1' || MarketKind == '4' ) ///< Futures
        {
            UFC::BufferedLog::DebugPrintf(" FUT Filled" );      
            FMessageObject->Send( "ORDER.FILL.FUT", UFC::Hostname, Data );            
        }
        else if( MarketKind == '2' || MarketKind == '3' ) ///< Options
        {
            UFC::BufferedLog::DebugPrintf(" OPT Filled" );
            FMessageObject->Send( "ORDER.FILL.OPT", UFC::Hostname, Data );
        }
        else
            UFC::BufferedLog::Printf( " Receive unhandled filled message." );
    }
}
//---------------------------------------------------------------------------
void SocketSend::OnQuoteExecution( const UFC::AnsiString& Confirm )
{
    MTree                    Data;
    int                      NetworkID = Confirm.SubString( 124, 8 ).ToInt();
    char                     MarketKind = Confirm[132];
    UFC::AnsiString          WebID = Confirm.SubString( 121, 3 );
    
    if( WebID == pSet->WebID || WebID == "LIV" )
    {
        Data.append( "REPLY", Confirm );
        Data.append( "NID", NetworkID );
        
        if( MarketKind == '1' ) ////< Future single
        {            
            UFC::BufferedLog::DebugPrintf( " Future quote confirm." );            
            FMessageObject->Send( "REPLY.FUT", UFC::Hostname, Data );
        }
        else if( MarketKind == '2' ) ///< Option single
        {
            UFC::BufferedLog::DebugPrintf( " Option quote confirm." );
            FMessageObject->Send( "REPLY.OPT", UFC::Hostname, Data );
        }
        else
            UFC::BufferedLog::Printf( " Receive unhandled quote confirm message." );
    }
}
//---------------------------------------------------------------------------
BOOL SocketSend::OnClientWrite( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
    char Buffer[265];
    UFC::AnsiString MessageType;
    
    try
    {
        memset( Buffer, '\0', 265 );
        int ReadByte = ClientSocket->RecvBuffer((UInt8*)Buffer, 265 );
        
        if( ReadByte  > 0 )
        {
            UFC::BufferedLog::Printf( " Receive %d byte(s):[%s]", ReadByte, Buffer );
            UFC::BufferedLog::FlushToFile();
            
            UFC::AnsiString    Data( Buffer );
            Data.TrimRight();
            
            if( Data == "*ALIVE" )
            {
                UFC::BufferedLog::DebugPrintf( " Received Heartbeat." );
            }
            else
            {
                ClientSocket->SendBuffer( FOkMessage, 260 );///< Send the ack *OK back.
                UFC::BufferedLog::DebugPrintf(" Receive:[%s]", Data.c_str() );
                MessageType = Data.SubString( 1, 6 );
                
                if( MessageType == "ITOFUO" ) ///< Confirm 
                    OnConfirmExecution( Data );
                else if( MessageType == "ITOTRD" ) ///< Filled
                    OnFilledExecution( Data );
                else if( MessageType == "ITOQUO" ) ///< Quote
                    OnQuoteExecution( Data );
                else if( MessageType == "ITOAUO" ) ///< Quote Request Reply, ignore it.
                    UFC::BufferedLog::DebugPrintf( " Receive quote request confirm. Drop it." );
                else
                    UFC::BufferedLog::Printf( " Receive unknown message." );            
                
                MessageSeq += 1;
            }
        }
    }
    catch( UFC::SocketException &ex )
    {
        UFC::BufferedLog::Printf( "Socket Exception:%s", ex.what());
        return false;
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " OnClientWrite throw Unknown Exception!");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
