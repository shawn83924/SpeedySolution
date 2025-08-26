#include "PHashedSet.h"
#include "List.h"
#include "FileStream.h"
#include "Sigo.h"
#include "MessageStore.h"
#include "AnsiString.h"
#include "iniFile.h"
#include "MTree.h"
#include "TandemAdapter.h"
//------------------------------------------------------------------------------
extern UFC::UiniFile* pConfig;
UFC::AnsiString AccountFile;
//------------------------------------------------------------------------------
TandemAdapter::TandemAdapter(bool NeedRecover, UFC::AnsiString TandemIP, UInt32 TandemPort, TMessageStore* ptrMessageStore)
: FRemoteIP(TandemIP), FPort(TandemPort)
{
    LoadConfig();
//    LoadAccount();
    FMessageStore = ptrMessageStore;
    FCommandListener = new TCommandListener(FRemoteIP, FPort, FClientHBInterval, FServerHBInterval, FMessageStore);
}

//------------------------------------------------------------------------------
void TandemAdapter::LoadConfig( void )
{
    UFC::AnsiString Tag, Value, Account, BrokerID, SubAccount, Buffer, Key;
    UFC::PStringList StringList;
    
//    pConfig->GetValue("Tandem", "IP", FRemoteIP);
//    pConfig->GetValue("Tandem", "Port", Value);
//    FPort = Value.ToInt();
    pConfig->GetValue( "Setting", "AppName", FAppName );
    
    pConfig->GetValue("Setting", "ServerHBInterval", Value);
    FServerHBInterval = Value.ToInt();
    pConfig->GetValue("Setting", "ClientHBInterval", Value);
    FClientHBInterval = Value.ToInt();
    
//    pConfig.GetValue( "Account", "Filename", AccountFile);
    
    UFC::BufferedLog::DebugPrintf( " AppName           :[%s]", FAppName.c_str() );
    UFC::BufferedLog::DebugPrintf( " Tandem IP         :[%s]", FRemoteIP.c_str() );
    UFC::BufferedLog::DebugPrintf( "        Port       :[%d]", FPort );
    UFC::BufferedLog::DebugPrintf( " ServerHBInterval  :[%d]", FServerHBInterval );
    UFC::BufferedLog::DebugPrintf( " ClientHBInterval  :[%d]", FClientHBInterval );
//    UFC::BufferedLog::DebugPrintf( " Account File      :[%s]", AccountFile.c_str() );
}

//------------------------------------------------------------------------------
void TandemAdapter::LoadAccount( void )
{
    UFC::FileStreamEx* File = new UFC::FileStreamEx(AccountFile, "r");    
    UFC::AnsiString Buffer;
    
    while(File->ReadLine( Buffer ))
    {           
        AddToTable(Buffer);
    }    
}
//------------------------------------------------------------------------------
void TandemAdapter::AddToTable( UFC::AnsiString Buffer )
{
    UFC::PStringList StringList;
    Buffer = Buffer.SubString( 0 , Buffer.Length() - 1 );
    StringList.SetStrings( Buffer, "," );
    int Count = StringList.ItemCount();
    UFC::BufferedLog::DebugPrintf( " Count:[%d]", Count );    
    if( Count == 3 )
    {
        UFC::AnsiString Key( StringList[0] + StringList[1] + StringList[2] );
        FAccountSet.Add( Key );
        UFC::BufferedLog::DebugPrintf( " BrokerID:[%s] Account[%s] SubAccount[%s]", StringList[0].c_str(), StringList[1].c_str(), StringList[2].c_str() );
    }
    else
    {
        UFC::AnsiString Key( StringList[0] + StringList[1] + "   " );
        FAccountSet.Add( Key );
        UFC::BufferedLog::DebugPrintf( " BrokerID:[%s] Account[%s] SubAccount[%s]", StringList[0].c_str(), StringList[1].c_str(), "   " );
    }        
}
//------------------------------------------------------------------------------
void TandemAdapter::Run( void )
{
//    FMessageObject->Start();
//    FMessageStore->Start();
}
//------------------------------------------------------------------------------
