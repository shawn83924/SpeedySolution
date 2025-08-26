#pragma hdrstop
#include "SocketSend.h"
//---------------------------------------------------------------------------
const char* TSC_FLOWENGINE_HEARTBEAT = "*ALIVE";
const int TSC_FLOWENGINE_HEARTBEAT_LENGTH = 6;
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
Setting::Setting( UFC::Section *iniSection )
:HostAddress( "127.0.0.1" ),
 Port( 5678 ),
 HBInterval( 2 ),
 ListenPort( 11111 ),
 ConnectInterval( 10 )
{
    UFC::AnsiString Value;

    WebID = iniSection->GetSectionName();
    if( iniSection->GetValue("IP", Value))
        HostAddress = Value;
    if( iniSection->GetValue("Port", Value))
        Port = Value.ToInt();
    if( iniSection->GetValue("HeartBeatInterval", Value))
        HBInterval = Value.ToInt();
    if( iniSection->GetValue("LocalIP", Value))
        LocalIP = Value;
    if( iniSection->GetValue("ListenPort", Value))
        ListenPort = Value.ToInt();
    if( iniSection->GetValue("ConnectInterval", Value))
        ConnectInterval = Value.ToInt();

}
//---------------------------------------------------------------------------
// Class SocketSend
//
//---------------------------------------------------------------------------
SocketSend::SocketSend(Setting* SettingPtr, bool FirstTimeRunning, MessageObject* pSigo, UFC::PtrQueue<UFC::AnsiString>* FJobQueue, BOOL Test)
:PThread( NULL, FALSE )
,FStat( st_DISCONNECT )
,SStat( st_DISCONNECT )
,pSet( SettingPtr )
,PInt( SettingPtr->WebID )
,FMessageObject( pSigo )
,FSocket( SettingPtr->HostAddress, SettingPtr->Port, FALSE)
,TestingMode( Test )
{
    if( pSet == NULL )
        throw( UFC::Exception( "pSet = NULL PTR\n" ) );
    memset( FAliveMessage, 0x20, 260 );
    memcpy( FAliveMessage, "*ALIVE",6 );
    if(TestingMode == FALSE)
        AsciiToEbcdic((unsigned char*)FAliveMessage,260);
    memset( FOkMessage, 0x20, 260 );
    memcpy( FOkMessage,"*OK",3 );
    if(TestingMode == FALSE)
        AsciiToEbcdic((unsigned char*)FOkMessage,260);

    if( FirstTimeRunning == true)
        PInt = 1;
    JobQueue = FJobQueue;


}
//---------------------------------------------------------------------------
bool SocketSend::CreateSocketObject(void)
{
    char Buffer[260];

    try
    {
        if( !FSocket.IsConnect() )
        {
            UFC::BufferedLog::Printf( " Try to connect to server %s:%d",(pSet->HostAddress).c_str(), pSet->Port);
            FSocket.Connect();
            try
            {
                memset( Buffer,0,260);
                int temp = FSocket.RecvBuffer((UInt8*)Buffer,260);

                if(temp > 0 )
                {
                    UFC::BufferedLog::DebugPrintf( " Received server recover ack:[%s]",  Buffer);
                    FStat = st_CONNECT;
                }
            }
            catch(...)
            {
                  UFC::BufferedLog::DebugPrintf( " Connect fail!" );
                  FSocket.Disconnect();
                  FStat = st_DISCONNECT;
            }
        }
        else
           UFC::BufferedLog::DebugPrintf( " Socket is connected" );

    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Connect to Server %s:%d failed!",(pSet->HostAddress).c_str(), pSet->Port);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void SocketSend::DequeueFunc(void)
{
    UFC::AnsiString*         SendData;

    while((SendData =JobQueue->Dequeue(pSet->HBInterval))!=NULL)
    {
        SendRejectOrderMessage( *SendData );
        delete SendData;
    }
}
//---------------------------------------------------------------------------
void SocketSend::Recover( UFC::PClientSocket * Socket)
{
    char Buffer[260];

    try
    {
        memset( Buffer,0,260);
        int temp = Socket->RecvBuffer((UInt8*)Buffer,260);

        if(temp > 0 )
        {
            UFC::BufferedLog::Printf( " Received server recover ack." );
            FStat = st_CONNECT;
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Recover fail!" );
        FStat = st_DISCONNECT;
    }
}
//---------------------------------------------------------------------------
void SocketSend::HeartBeat(void)
{
    try
    {
        FSocket.SendBuffer( FAliveMessage, 260);
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Send HeartBeat Failed.");
        FSocket.Disconnect();
        FStat = st_DISCONNECT;
    }
}
//---------------------------------------------------------------------------
void SocketSend::SendRejectOrderMessage( const UFC::AnsiString& OrderString )
{
    UFC::AnsiString RejectMessage;
    UFC::AnsiString DateString;
    UFC::AnsiString TimeString;
    UFC::AnsiString YYMMDD;
    MTree           Data;
    int             NetworkID = RejectNID( OrderString );
    int             OrderType = GetOrderType( OrderString );  ///< 01:Buy  02:Sell   03:CancelReplace   04:Cancel

    GetYYYYMMDD( DateString );
    GetYYMMDD( YYMMDD );
    GetHHMMSS( TimeString );
    Data.append( "NID", NetworkID );
    RejectMessage.Printf("NITOLOT  30%02d00%06s889404   0000              000000000 00%06s%06s000005((S    %06d%06d1 AS400 is out of order, please try later.                    006      33002                                          006           ))%08s%06s                 ",
                                                                                      OrderType,
                                                                                      TimeString.c_str(),
                                                                                      YYMMDD.c_str(),
                                                                                      TimeString.c_str(),
                                                                                      GetClOrderID(OrderString),
                                                                                      GetClOrderID(OrderString),
                                                                                      DateString.c_str(),
                                                                                      TimeString.c_str()
                                                                                      );
    Data.append("REPLY",RejectMessage);
    FMessageObject->Send("REPLY.STOCK", UFC::Hostname, Data);
}
//---------------------------------------------------------------------------
bool SocketSend::CheckConnction(void)
{
    if( FStat != st_CONNECT )
        return CreateSocketObject();
    return true;
}
//---------------------------------------------------------------------------
void SocketSend::Work( UFC::AnsiString* SendData )
{
    char Buffer[260];
    memset( Buffer, 0, 260 );
    memcpy( Buffer,SendData->c_str(),SendData->Length());
    UFC::BufferedLog::DebugPrintf( " Send:[%s]", SendData->c_str() );
    if(TestingMode == FALSE)
        AsciiToEbcdic((unsigned char*)Buffer,260);
    FSocket.SendBuffer( Buffer, 260 );

    try
    {
        memset( Buffer,0,260);
        int temp = FSocket.RecvBuffer((UInt8*)Buffer, 260);

        if(temp > 0 )
        {
            UFC::BufferedLog::DebugPrintf( " Received ack:[%s]",  Buffer);
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Received fail!" );
        FStat = st_DISCONNECT;
        FSocket.Disconnect();
    }

}
//---------------------------------------------------------------------------
void SocketSend::Execute(void)
{
    ///< 2 Thread handle up to 2 connectoion
    FServer = new UFC::MultiplexingServer(pSet->ListenPort, 2,2);
    FServer->SetListener( this );
    FServer->EnableHeartbeat( false );
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
            if( (SendData = JobQueue->Dequeue( pSet->HBInterval )) != NULL )
            {
                try
                {
                  if(FStat == st_CONNECT && SStat == st_CONNECT )
                  {
                    Work( SendData );
                    delete SendData;
                  }
                  else
                  {
                    if( SendData!=NULL )
                    {
                      SendRejectOrderMessage( *SendData );
                      delete SendData;
                    }
                  }
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
void SocketSend::OnMessage( const UFC::AnsiString& Subject,const UFC::AnsiString& Key, MTree* Data )
{
    MString OrderString;

    if( Data->get("ORDER",OrderString) == TRUE )
    {
        if( FStat == st_CONNECT && SStat == st_CONNECT )
        {
            bool Mark = JobQueue->Inqueue( new UFC::AnsiString(OrderString) );
        }
        else
            SendRejectOrderMessage( OrderString );
    }
}
//---------------------------------------------------------------------------
// Functions implement interface SocketServerListener
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

    UFC::BufferedLog::Printf(" Flow Engine: Client( AS/400 ) connect from IP:%s Port:%d",
                             ClientSocket->GetIPAddress().c_str(),
                             ClientSocket->GetPort());
    SStat = st_CONNECT;
    memset( Buffer,0x20,260 );
    if( sprintf(Temp,"*%06d",PInt.ToInt32()) )
    {
        memcpy(Buffer,Temp,7);
        if(TestingMode == FALSE)
            AsciiToEbcdic((unsigned char*)Buffer, 260);
        ClientSocket->SendBuffer(Buffer,260);
    }
}
//---------------------------------------------------------------------------
void SocketSend::OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
     UFC::BufferedLog::Printf(" Client(AS/400) Disconnected(IP:%s Port:%d)",
                               ClientSocket->GetIPAddress().c_str(),
                               ClientSocket->GetPort());
     SStat = st_DISCONNECT;
}
//---------------------------------------------------------------------------
BOOL SocketSend::OnClientWrite( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
    char Buffer[ 261 ];

    try
    {
        memset( Buffer, 0, 261 );
        if( ClientSocket->RecvBuffer((UInt8*)Buffer,260) > 0 )
        {
            Buffer[260] = 0;

            if( strncmp(Buffer, TSC_FLOWENGINE_HEARTBEAT, TSC_FLOWENGINE_HEARTBEAT_LENGTH) == 0 )
            {
                UFC::BufferedLog::DebugPrintf( " Received Heartbeat." );
            }
            else
            {
                UFC::BufferedLog::DebugPrintf(" OnClientWrite:[%s]", Buffer);
                ClientSocket->SendBuffer( FOkMessage, 260 );///< Send the ack *OK back.
                OnStockExecution( Buffer );
                PInt += 1;
            }
        }
    }
    catch(UFC::SocketException &ex)
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
void SocketSend::OnStockExecution( char* Buffer )
{
    MTree                    MyTree;
    int                      NetworkID;

    if( strncmp( Buffer + 1, "ITOMAH", 6 ) == 0 ) ///< Filled
    {
        UFC::BufferedLog::DebugPrintf( " It's a filled." );
        if( CheckFillWID( Buffer ) )
        {
            MyTree.append("FILL_ORDER", Buffer );
            FMessageObject->Send("ORDER.FILL.STOCK", UFC::Hostname, MyTree);
        }
        else
          UFC::BufferedLog::DebugPrintf( " CheckWebID return false." );

    }
    else if( strncmp( Buffer + 1, "ITOLOT", 6 ) == 0 ) ///< confirm
    {
        if( CheckConfirmWID( Buffer ) )
        {
            NetworkID = GetConfirmNID( Buffer );
            MyTree.append("REPLY",Buffer );
            MyTree.append("NID", NetworkID);
            FMessageObject->Send("REPLY.STOCK", UFC::Hostname, MyTree);
        }
    }
    else
        UFC::BufferedLog::Printf( " Receive the Data with format != { ITOLOT,ITOMAH }");
}
//---------------------------------------------------------------------------
// Check the Web ID of the Confirm/Reject execution report
//---------------------------------------------------------------------------
bool SocketSend::CheckConfirmWID( char* Buffer )
{
    UFC::AnsiString  WID( Buffer + 159, 3 );

    UFC::BufferedLog::DebugPrintf( " Received confirm WebID:[%s]", WID.c_str() );
    if( WID == pSet->WebID || WID == "LIV" )
        return true;
    return false;
}
//---------------------------------------------------------------------------
// Fetch the Netwiork ID from the order confirm message.
//---------------------------------------------------------------------------
Int32 SocketSend::GetConfirmNID( char* Buffer)
{
    UFC::AnsiString  NID( Buffer + 91, 6 );
    return NID.ToInt();
}
//---------------------------------------------------------------------------
// Check the Web ID of the fill execution report
//---------------------------------------------------------------------------
bool SocketSend::CheckFillWID( char* Buffer)
{
    UFC::AnsiString  WID( Buffer + 101, 3 );

    UFC::BufferedLog::DebugPrintf( " Received Execution WebID:[%s]", WID.c_str());
    if( WID == pSet->WebID || WID == "LIV" )
        return true;
    return false;
}
//---------------------------------------------------------------------------
// Fetch the Netwiork ID from the order.
//---------------------------------------------------------------------------
int SocketSend::RejectNID( const UFC::AnsiString& Buffer )
{
    UFC::AnsiString  NID( Buffer.c_str() + 67, 6 );
    UFC::BufferedLog::DebugPrintf(" Reject NID = [%s]", NID.c_str());
    return NID.ToInt();
}
//---------------------------------------------------------------------------
// Fetch the order type from the order.
//--------------------------------------------------------------------------
int SocketSend::GetOrderType( const UFC::AnsiString& Buffer )
{
    UFC::AnsiString OrderType( Buffer.c_str() + 11, 2);
    UFC::BufferedLog::DebugPrintf(" OrderType = [%s]", OrderType.c_str());
    return OrderType.ToInt();
}
//---------------------------------------------------------------------------
// Fetch the order type from the order.
//--------------------------------------------------------------------------

int SocketSend::GetClOrderID( const UFC::AnsiString& Buffer )
{
  UFC::AnsiString OrderType( Buffer.c_str() + 67, 6 );
  UFC::BufferedLog::DebugPrintf(" GetClOrderID = [%s]", OrderType.c_str());
  return OrderType.ToInt();

}
