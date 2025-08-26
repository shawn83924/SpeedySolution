#include "MessageDispatcher.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
const UFC::AnsiString CONFIG_FILE( "../cfg/SpeedyFIFORiskManager.cfg" );
//------------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher( void )
:MessageListener()
,FMBusIP("127.0.0.1")
,FMBusPort(12345)
,FSendFIFOName("")
,FRecvFIFOName("")
,FSubSendFIFOName("")
,FSubRecvFIFOName("")
,FPatsSendFIFOName("")
,FPatsRecvFIFOName("")
,FDefaultConnection(0)
,FDefaultSubConnection(0)
,FPATSConnection(0)
{    
    UFC::BufferedLog::Printf( " PIPE_BUF:[%d]", PIPE_BUF );
    LoadConfig();
    FMessageObject = new MessageObject( "SpeedyFIFORiskManager", "1.0.0", "Speedy FIFO Connection for RM module", FMBusPort );
    FMessageObject->SetHost(FMBusIP);
    if( ( FSendFIFOName.Length() > 0 ) && ( FRecvFIFOName.Length() > 0 ) )
    {
        FMessageObject->AddListener( "SpeedyFIFORiskManager.FUT", UFC::Hostname, this );
        FMessageObject->AddListener( "SpeedyFIFORiskManager.OPT", UFC::Hostname, this );
        FDefaultConnection = new SpeedyFIFOConnection( FSendFIFOName, FRecvFIFOName, FMessageObject, MSG_LENGTH, true );
        UFC::BufferedLog::Printf( " Open Default FIFO Connection send:[%s] recv:[%s]\n", FSendFIFOName.c_str(), FRecvFIFOName.c_str() );
        
        if( ( FSubSendFIFOName.Length() > 0 ) && ( FSubRecvFIFOName.Length() > 0 ) )
        {
            FDefaultSubConnection = new SpeedyFIFOConnection( FSubSendFIFOName, FSubRecvFIFOName, FMessageObject, MSG_LENGTH, true );
            UFC::BufferedLog::Printf( " Open Default Sub FIFO Connection send:[%s] recv:[%s]\n", FSubSendFIFOName.c_str(), FSubRecvFIFOName.c_str() );
        }
    }
    
    if( ( FPatsSendFIFOName.Length() > 0 ) && ( FPatsRecvFIFOName.Length() > 0 ) )
    {
        FMessageObject->AddListener( "SpeedyFIFORiskManager.PATS", UFC::Hostname, this );
        FPATSConnection = new SpeedyFIFOConnection( FPatsSendFIFOName, FPatsRecvFIFOName, FMessageObject, PATS_BACKEND_REPLY_LENGTH, true );
        UFC::BufferedLog::Printf( " Open Pats FIFO Connection send:[%s] recv:[%s]\n", FPatsSendFIFOName.c_str(), FPatsRecvFIFOName.c_str() );
    }    

    FMessageObject->Start();
    FMessageObject->WaitForConnected();
}
//------------------------------------------------------------------------------
void MessageDispatcher::LoadConfig( void )
{   ///< Load Default Pipe setting.
    UFC::UiniFile Config( CONFIG_FILE );
    UFC::AnsiString fieldValue;
    if( Config.GetValue( "Setting", "SendFIFOName", fieldValue ) == TRUE )
    {
        FSendFIFOName = fieldValue;
        if( Config.GetValue( "Setting", "RecvFIFOName", fieldValue ) == TRUE )
        {
            FRecvFIFOName = fieldValue;
            UFC::BufferedLog::Printf( " Default FIFO send:[%s] recv:[%s]", FSendFIFOName.c_str(), FRecvFIFOName.c_str() );
        }
        else
            UFC::BufferedLog::Printf( " Default recv FIFO name not setting." );
    }
    else
        UFC::BufferedLog::Printf( " Default send FIFO name not setting." );
    
    if( Config.GetValue( "Setting", "SubSendFIFOName", fieldValue ) == TRUE )
    {
        FSubSendFIFOName = fieldValue;
        if( Config.GetValue( "Setting", "SubRecvFIFOName", fieldValue ) == TRUE )
        {
            FSubRecvFIFOName = fieldValue;
            UFC::BufferedLog::Printf( " Default sub FIFO send:[%s] recv:[%s]", FSubSendFIFOName.c_str(), FSubRecvFIFOName.c_str() );
        }
        else
            UFC::BufferedLog::Printf( " Default sub recv FIFO name not setting." );
    }
    else
        UFC::BufferedLog::Printf( " Default sub send FIFO name not setting." );

    if( Config.GetValue( "Setting", "PATSSendFIFOName", fieldValue ) == TRUE )
    {
        FPatsSendFIFOName = fieldValue;
        if( Config.GetValue( "Setting", "PATSRecvFIFOName", fieldValue ) == TRUE )
        {
            FPatsRecvFIFOName = fieldValue;
            UFC::BufferedLog::Printf( " Default PATS FIFO send:[%s] recv:[%s]", FPatsSendFIFOName.c_str(), FPatsRecvFIFOName.c_str() );
        }
        else
            UFC::BufferedLog::Printf( " Default PATS recv FIFO name not setting." );
    }
    else
        UFC::BufferedLog::Printf( " Default PATS send FIFO name not setting." );
        

    if( ( FSendFIFOName.Length() <= 0 ) || ( FRecvFIFOName.Length() <= 0 ) ) return;
    ///< Load other pipes.
    UFC::AnsiString SendFIFOName, RecvFIFOName;
    UFC::AnsiString SubSendFIFOName, SubRecvFIFOName;
    for( register int i = 1; i < Config.SectionCount(); i++ )
    {
        UFC::AnsiString SectionName, Branch, Temp;
        SectionName.Printf( "Pipe%d", i );
        UFC::Section* pSection = Config.GetSection( i );
        if( pSection != NULL )
        { 
	    SpeedyFIFOConnection* Connection = NULL;
	    SpeedyFIFOConnection* SubConnection = NULL;
	    
            pSection->GetValue( "SendFIFOName", SendFIFOName );
            pSection->GetValue( "RecvFIFOName", RecvFIFOName );
            UFC::BufferedLog::Printf( " %s send FIFO:[%s]", SectionName.c_str(), SendFIFOName.c_str() );
            UFC::BufferedLog::Printf( " %s recv FIFO:[%s]", SectionName.c_str(), RecvFIFOName.c_str() );
            Connection = new SpeedyFIFOConnection( SendFIFOName, RecvFIFOName, FMessageObject, MSG_LENGTH );
            
	    if( pSection->GetValue( "SubSendFIFOName", SubSendFIFOName ) &&
		pSection->GetValue( "SubRecvFIFOName", SubRecvFIFOName ) )
	    {
		UFC::BufferedLog::Printf( " %s Sub send FIFO:[%s]", SectionName.c_str(), SubSendFIFOName.c_str() );
		UFC::BufferedLog::Printf( " %s Sub recv FIFO:[%s]", SectionName.c_str(), SubRecvFIFOName.c_str() );
		SubConnection = new SpeedyFIFOConnection( SubSendFIFOName, SubRecvFIFOName, FMessageObject, MSG_LENGTH );
	    }
	    
            if( pSection->GetValue( "Branch", Temp ) == FALSE )
            {
                exit(0);
            }
            else
            {
                UFC::PStringList StringList;
                StringList.SetStrings( Temp, "," );
                for( register int i = 0; i < StringList.ItemCount(); i++ )
                {
                    UFC::BufferedLog::DebugPrintf( " Branch %d:[%s]", i + 1, StringList.GetItem(i).c_str() );
                    FTable.Add( StringList.GetItem(i), Connection );
		    if( SubConnection != NULL )
			FSubTable.Add( StringList.GetItem(i), SubConnection );
                }
            }
            Connection->Start();
        }
        else
            continue;
    }  //for( register int i = 1; i < Config.SectionCount(); i++ )
}
//------------------------------------------------------------------------------
bool MessageDispatcher::GetDataFromOrderKey( const UFC::AnsiString& OrderKey, UFC::AnsiString& AccountExecutive, UFC::AnsiString& UserDefine, UFC::AnsiString& SubBrokerNo, UFC::AnsiString& SubAccount )
{
    AccountExecutive = "";
    UserDefine = "";
    SubBrokerNo = "";
    SubAccount = "";
    int orderKeyLen = OrderKey.Length();
    if (orderKeyLen <= 0) return false;
    
    Int32 commaPosition = OrderKey.AnsiPos(',');
    if (commaPosition < 0)
        AccountExecutive = OrderKey;
    else
    {
        if (orderKeyLen <= 1) return false;
        
        if (commaPosition == 0)
            AccountExecutive = "";
        else
            AccountExecutive = OrderKey.SubString(0, commaPosition);
        
        int userDefineLen = orderKeyLen - (commaPosition + 1);
        if (userDefineLen > 0)
        {
            UserDefine = OrderKey.SubString(commaPosition + 1, userDefineLen);
            if (UserDefine.Length() >= 4) 
            {
                SubBrokerNo = UserDefine.SubString(0, 4);
                SubBrokerNo.TrimRight(' ');
            }
            
            if (UserDefine.Length() >= 10) 
            {
                SubAccount = UserDefine.SubString(4, 6);
                SubAccount.TrimRight(' ');
            }            
        } 
    }
    return true;
}  
//------------------------------------------------------------------------------
bool MessageDispatcher::GetSubAccountFromOrderKey( const UFC::AnsiString& OrderKey, UFC::AnsiString& SubAccount, UFC::AnsiString& UserDefine )
{
    SubAccount = "";
    UserDefine = "";
    int orderKeyLen = OrderKey.Length();
    if (orderKeyLen <= 0) return false;
    
    Int32 commaPosition = OrderKey.AnsiPos(',');
    if (commaPosition < 0)
        SubAccount = OrderKey;
    else
    {
        if (orderKeyLen <= 1) return false;
        
        if (commaPosition == 0)
            SubAccount = "";
        else
            SubAccount = OrderKey.SubString(0, commaPosition);
        
        int userDefineLen = orderKeyLen - (commaPosition + 1);
        if (userDefineLen > 0)
            UserDefine = OrderKey.SubString(commaPosition + 1, userDefineLen);
    }
    return true;
}  
//------------------------------------------------------------------------------
void MessageDispatcher::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    UFC::AnsiString OrderTime, GIGO, UserData, Order, SendMsg, SubAccount, channel, userDefine, accountExecutive, subBrokerNo;
    Int32 NID;
    Int32 IsOffHour = 0;
        
    UFC::GetTimeString( OrderTime, FALSE );        
    OrderTime.PadThis( 9, '0' ); ///< HHMMSGIGOSmmm    
    Data->get( "ORDER", Order ); ///< R010 Length 75 
    Data->get( "NID", NID );     ///< Integer 10    
    Data->get( "KEY", GIGO ); 
    Data->get( "OFFHOUR", IsOffHour );
    if( Data->get( "CHANNEL", channel ) == FALSE ) 
        channel = "                ";
    else
        channel.PadThis(PATS_CHANNEL_LENGTH, ' ' );
    if( channel.Length() > PATS_CHANNEL_LENGTH ) channel.SetLength(PATS_CHANNEL_LENGTH);
    
    UFC::BufferedLog::DebugPrintf( " Receive order from SpeedyGateway [%s][%s]", Subject.c_str(), Key.c_str() );
    UFC::BufferedLog::DebugPrintf( " Order   [%s]", Order.c_str() );
    UFC::BufferedLog::DebugPrintf( " NID     [%d]", NID );    
    UFC::BufferedLog::DebugPrintf( " Key     [%s]", GIGO.c_str() );
    UFC::BufferedLog::DebugPrintf( " Channel [%s]", channel.c_str() );
    
    if( GIGO.AnsiPos( "<Proxy:" ) != -1 )
    {
        UFC::AnsiString AE( GIGO.c_str(), 8 );          ///< AE length = 8
        UFC::AnsiString Proxy( GIGO.c_str() + 9, 25 );  ///< Proxy length = 25
        UFC::AnsiString PMFData(  GIGO.c_str() + 9 + 25 );
	
        PMFData.PadThis( 94, ' ' );    
        UserData.Printf( "%s,%s%s", AE.c_str(), PMFData.c_str(), Proxy.c_str() );
    }
    else
        UserData = GIGO;
    
    SpeedyFIFOConnection *Connection = NULL;
    if( ( Subject == "SpeedyFIFORiskManager.FUT" ) || ( Subject == "SpeedyFIFORiskManager.OPT" ) )
    {
        GetDataFromOrderKey( UserData, accountExecutive, userDefine, subBrokerNo, SubAccount );
        userDefine.PadThis( TAIFEX_USERDATA_LENGTH, ' ' );
        UFC::BufferedLog::DebugPrintf( " AE[%s] SubBrokerNo[%s] SubAccount[%s]", accountExecutive.c_str(), subBrokerNo.c_str(), SubAccount.c_str() );
        UFC::AnsiString BrokerID( Order.SubString( 14, 7 ) );
        
        if( ( subBrokerNo.Length() <= 0 ) || ( subBrokerNo == "    " ) )
            Connection = FTable.GetObjectByKey( BrokerID );
        else
            Connection = FSubTable.GetObjectByKey( BrokerID );
    } 
    
    if( Subject == "SpeedyFIFORiskManager.FUT" )
    {
        char TS = 'F';
        
        if( IsOffHour == true )
            TS = 'f';
        accountExecutive.PadThis(15, ' ');
        SendMsg.Printf( "%s%c%010d%270s%9s%15s%c", Order.c_str(), TS, NID, userDefine.c_str(), OrderTime.c_str(), accountExecutive.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send Futures Pipe:[%s] %d", SendMsg.c_str(), SendMsg.Length() );
        if( Connection == NULL )
	{
	    if( ( subBrokerNo.Length() <= 0 ) || ( subBrokerNo == "       " ) )
            {
                if( FDefaultConnection != 0 )
                    FDefaultConnection->OnMessage( SendMsg, mFutures, Data );
                else
                    UFC::BufferedLog::Printf( " No FUT Default Connection." );
            }
	    else
            {
                if( FDefaultSubConnection != 0 )
    		    FDefaultSubConnection->OnMessage( SendMsg, mFutures, Data );
                else
                {
                    UFC::BufferedLog::Printf( " No FUT SubDefault Connection." );
                    if( FDefaultConnection != 0 )
                        FDefaultConnection->OnMessage( SendMsg, mFutures, Data );
                    else
                        UFC::BufferedLog::Printf( " No FUT Default Connection." );
                }
            }  
	}
        else 
            Connection->OnMessage( SendMsg, mFutures, Data );
    }
    else if( Subject == "SpeedyFIFORiskManager.OPT" )
    {
        char TS = 'O';
        
        if( IsOffHour == true )
            TS = 'o';
        accountExecutive.PadThis(15, ' ');
        SendMsg.Printf( "%s%c%010d%270s%9s%15s%c", Order.c_str(), TS, NID, userDefine.c_str(), OrderTime.c_str(), accountExecutive.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send Options Pipe:[%s] %d", SendMsg.c_str(), SendMsg.Length() );        
        if( Connection == NULL )
	{
	    if( ( subBrokerNo.Length() <= 0 ) || ( subBrokerNo == "       " ) )
            {
                if( FDefaultConnection != 0 )
                    FDefaultConnection->OnMessage( SendMsg, mOptions, Data );
                else
                    UFC::BufferedLog::Printf( " No OPT Default Connection." );
            }
	    else
            {
                if( FDefaultSubConnection != 0 )
    		    FDefaultSubConnection->OnMessage( SendMsg, mOptions, Data );
                else
                {
                    UFC::BufferedLog::Printf( " No OPT SubDefault Connection." );
                    if( FDefaultConnection != 0 )
                        FDefaultConnection->OnMessage( SendMsg, mOptions, Data );
                    else
                        UFC::BufferedLog::Printf( "No OPT Default Connection." );
                }
            }
	}
        else 
            Connection->OnMessage( SendMsg, mOptions, Data );
    }
    else if( Subject == "SpeedyFIFORiskManager.PATS" )
    {
        Order.PadThis(PATS_ORDER_LENGTH, ' ');
        Int32 userDataLength = UserData.Length();
        Int32 commaPosition = UserData.AnsiPos(',');
        UFC::AnsiString tempUserData;
        if (commaPosition < 0)
        {
            SubAccount = UserData;
            tempUserData = "";
        }
        else
        {
            if (commaPosition == 0)
                SubAccount = "";
            else
                SubAccount = UserData.SubString(0, commaPosition);
            tempUserData = UserData.SubString(commaPosition + 1, userDataLength - (commaPosition + 1));            
        }
        SubAccount.PadThis(PATS_SUBACCOUNT_LENGTH, ' ');
        
        int tempUserDataLength = tempUserData.Length();
        if (tempUserDataLength <= 0) tempUserDataLength = patsDefaultUserDataLength;
        UFC::AnsiString tempUserDataLengthStr;
        tempUserDataLengthStr.Printf("%d", tempUserDataLength);
        tempUserDataLengthStr.PadThis(PATS_USERDATALEN_LENGTH, ' ');
        tempUserData.PadThis(PATS_USERDATA_LENGTH, ' ');
        SendMsg.Printf( "%s%c%010d%s%s%s%s%s%c", Order.c_str(), 'P', NID, tempUserData.c_str(), OrderTime.c_str(), channel.c_str(), SubAccount.c_str(), tempUserDataLengthStr.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send PATS Pipe:[%s] %d", SendMsg.c_str(), SendMsg.Length() );
        if( Connection == NULL )
        {
            if (FPATSConnection != 0) 
                FPATSConnection->OnMessage( SendMsg, mPATSFutures, Data );
            else
                UFC::BufferedLog::Printf( " No PATS Connection" );
        }
        else 
            Connection->OnMessage( SendMsg, mPATSFutures, Data );
    }
    else
    {
        UFC::BufferedLog::Printf( " Receive data from unknown subject:[%s]", Subject.c_str() );
        return;
    }
}
//------------------------------------------------------------------------------





