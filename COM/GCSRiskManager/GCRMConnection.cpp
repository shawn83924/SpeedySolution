#include "GCRMConnection.h"
#include "iniFile.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL RecoverMode;
extern UFC::AnsiString AppName;
//------------------------------------------------------------------------------
UFC::PHashedSet<UFC::AnsiString>  FUSSymbolTable;
//------------------------------------------------------------------------------
BOOL IsUSDSym( const UFC::AnsiString& Symbol )
{
    UFC::AnsiString Sym( Symbol.c_str(), 3 );
    
    return FUSSymbolTable.Exists( Sym );
}
//------------------------------------------------------------------------------
GCRMConnection::GCRMConnection( )
:FMarginLevel( 2 )//<Default Group margin
,FToSpeedySubject( "Risk") 
,FToSpeedyKey( "ToSpeedy") 
,FFutOrderSubject( AppName + ".FUT" )
,FOptOrderSubject( AppName + ".OPT")
{    
    LoadSetting();
    UFC::AnsiString LogName;

    UFC::BufferedLog::SetDebugMode( DumpOnly );
    LogName.Printf( "../log/%s.%s.log", AppName.c_str(), UFC::GetDateString().c_str() );
    UFC::BufferedLog::Printf( " LogName:[%s]", LogName.c_str() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );
       
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   GrandCathay Risk control module " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    UFC::BufferedLog::Printf( "   AS400 To Speedy [%s][%s]", FToSpeedySubject.c_str(), FToSpeedyKey.c_str() );
    UFC::BufferedLog::Printf( "                       " );
    UFC::BufferedLog::Printf( "   Default Margin Level[%d]", FMarginLevel );
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    
    FMessageObject = new MessageObject( AppName, "1.0.0", "GrandCathaySec RM connection." );
    FMessageObject->AddListener( FFutOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FOptOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FToSpeedySubject, FToSpeedyKey, this );
    FMessageObject->Start( );
}
//------------------------------------------------------------------------------
void GCRMConnection::LoadSetting( void )
{
    UFC::AnsiString FileName,Name,Value;
    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    try
    {
        UFC::UiniFile   Config( FileName );
        UFC::Section*   IniSection;

        if( Config.GetValue( "Setting", "DefaultMargin", Value ) )
        {
            FMarginLevel = Value.ToInt();        
            if( FMarginLevel < 1 || FMarginLevel >3 )
                FMarginLevel = 2;
        }                
        Config.GetValue( "ToSpeedy", "Subject",  FToSpeedySubject );
        Config.GetValue( "ToSpeedy", "Key",  FToSpeedyKey );
        FUSSymbolTable.Clear();
        if( (IniSection = Config.GetSection( "Currency")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue(i,Name,Value) && Value == "USD" )
                {                    
                    FUSSymbolTable.Add( Name );
                    UFC::BufferedLog::Printf( " Contract[%s] use currency USD.", Name.c_str() );
                }
            }
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.", FileName.c_str() );
        exit(1);
    }
}
//------------------------------------------------------------------------------
GCRMConnection::~GCRMConnection( void )
{
    
}
//------------------------------------------------------------------------------
bool GCRMConnection::IsReplacePx( const UFC::AnsiString& Order )
{
    if( Order.SubString( 2, 2 ).ToInt() == 6 )
        return true;
    return false;
}
//---------------------------------------------------------------------------
void GCRMConnection::UserDataToAE( const UFC::AnsiString& UserData, UFC::AnsiString& AE )
{
    UFC::AnsiString  Group,User;
    UFC::PStringList Strs;
        
    Strs.SetStrings( UserData,";\n");
    if( Strs.ItemCount() >= 2 )
    {
        Group = Strs[ 0 ];
        User  = Strs[ 1 ];
        Group.TrimRight(' ');
        User.TrimRight(' ');
        Group.PadThis( 4,' ');
        User.PadThis( 3,' ');
        AE = Group + User;
    }
    else
    {
        AE = "       ";
        AE.PadThis( 7,' ');
    }    
}
//---------------------------------------------------------------------------
void GCRMConnection::GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data ( Data:  Group;AE )
    Int32 SplitPos = Key.AnsiPos(',');

    if( SplitPos !=  -1 )
    {
        UFC::AnsiString  UserData( Key.c_str() + SplitPos + 1 );//< Skip ,
        
        UserDataToAE( UserData,  AE );        
    }
    else
    {
        AE = "       ";
        AE.PadThis( 7,' ');
    }        
}
//------------------------------------------------------------------------------
void GCRMConnection::UseMargin( UFC::AnsiString& AE )
{
    if( FMarginLevel == 2 ) ///< Default: Group Margin
    {
        AE.SetLength( 4 );
        AE.PadThis( 7,' ');
        UFC::BufferedLog::Printf( " Use Group margin. AE[%s] ", AE.c_str() );
    }
    else
        UFC::BufferedLog::Printf( " Use AE margin. AE[%s] ", AE.c_str() );
}
//------------------------------------------------------------------------------
void GCRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    if( Subject == FFutOrderSubject|| Subject == FOptOrderSubject )
    {
        UFC::AnsiString Key, Order, AE;
        Int32 NID;
        
        if( Data->get( "ORDER", Order ) == TRUE &&  Data->get( "NID", NID ) == TRUE && Data->get( "KEY", Key ) == TRUE )
        {
            UFC::BufferedLog::Printf( " Recv from Speedy Gateway:[%s]", Order.c_str() );
            UFC::AnsiString Symbol( Order.c_str() + 34, 20 );            
            
            Symbol.TrimRight();
            GetAE( Key, AE );
            UseMargin( AE );           
            if( IsReplacePx( Order ) ) ///< Skip Replace PX
            {
                Data->append( "CODE", 1 );
                if( Subject == FFutOrderSubject )
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                else
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                return;
            }
            if( IsLockedSym( Symbol ) == TRUE  ) ///< Symbol locked.
            {
                UFC::AnsiString Msg;
                
                UFC::BufferedLog::Printf( " Symbol[%s] Locked", Symbol.c_str() );
                UFC::BufferedLog::DebugPrintf( " Speedy RM Reject Order:[%s]", Order.c_str() );
                Msg.Printf( " Symbol[%s] Locked", Symbol.c_str() );
                Data->append( "CODE", 0 );
                Data->append( "MSG", Msg );
                if( Subject == FFutOrderSubject )
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                else
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                return;
            }            
            if( Subject == FFutOrderSubject ) ///< Has margin.
            {
                if( AccountCanPlaceOrder( AE, Symbol ) )
                {
                    UFC::BufferedLog::DebugPrintf( " AE[%s] place order OK.", AE.c_str() );
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( " AE[%s] margin not enough.", AE.c_str() );
                    Data->append( "CODE", 0 );
                    Data->append( "MSG", "Exceed Margin." );
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                }
            }
            else if( Subject == FOptOrderSubject )
            {
                if( AccountCanPlaceOrder( AE, Symbol ) )
                {
                    UFC::BufferedLog::DebugPrintf( " AE[%s] place order OK.", AE.c_str() );
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( " AE[%s] margin not enough.", AE.c_str() );
                    Data->append( "CODE", 0 );
                    Data->append( "MSG", "Exceed Margin." );
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                }
            }
        }
        else
            UFC::BufferedLog::Printf( " Missing ORDER KEY or NID field." );

    }
    else if( Subject == FToSpeedySubject ) ///< Recv Margin from Backend.
    {
        UFC::AnsiString RiskInfo;
        if( Data->get( "Data", RiskInfo ) && RiskInfo.Length() >= 33 )
        {
            UFC::AnsiString Dep( RiskInfo.c_str() + 7, 7 );
            UFC::AnsiString User( RiskInfo.c_str() + 14, 7 );            
            UFC::AnsiString Curr( RiskInfo.c_str() + 21, 3 );
            UFC::AnsiString AE;            
            UFC::Int32      CanUse = UFC::AnsiString( RiskInfo.c_str() + 24, 9 ).ToInt();

            Dep.TrimRight( ' ' );
            Dep.PadThis(4,' ');
            User.TrimRight( ' ' );            
            User.PadThis(3,' ');
            AE = Dep + User;
            UFC::BufferedLog::Printf( " AE[%s] Currency[%s] CanUse[%d]", AE.c_str(), Curr.c_str(), CanUse);            
            SetAccountInfo( AE, Curr, CanUse );
        }
    }
}
//-------------------------------------------------------------------------------
BOOL GCRMConnection::AccountCanPlaceOrder( const UFC::AnsiString& AE, const UFC::AnsiString& Symbol )
{
    if( IsUSDSym( Symbol ) == TRUE )
    {
        UFC::BufferedLog::DebugPrintf( " AE[%s] place USD currency order[%s].", AE.c_str(), Symbol.c_str() );
        return !(FUSDLockAE.Exists( AE ));
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " AE[%s] place TWA currency order[%s].", AE.c_str(), Symbol.c_str() );
        return !(FTWALockAE.Exists( AE ));
    }
}
//------------------------------------------------------------------------------
BOOL GCRMConnection::IsLockedSym( const UFC::AnsiString& Symbol )
{
    UFC::AnsiString Sym( Symbol.c_str(), 3 );
    
    return FLockSymbolTable.Exists( Sym );
}
//-------------------------------------------------------------------------------
void  GCRMConnection::SetAccountInfo( const UFC::AnsiString& AE, const UFC::AnsiString& Currency, int CanUse )
{
    UFC::PLockObject Lock(  FAccountLimitCS );
    
    if( Currency == "TWA" )
    {
        if( CanUse > 0 )
        {
            UFC::BufferedLog::Printf( " AE[%s] can place Currency[TWA] order.", AE.c_str() );
            FTWALockAE.Delete( AE );
        }
        else
        {
            UFC::BufferedLog::Printf( " AE[%s] can NOT place Currency[TWA] order.", AE.c_str() );
            FTWALockAE.Add( AE );
        }
    }
    else if( Currency == "USA" )
    {
        if( CanUse > 0 )
        {
            UFC::BufferedLog::Printf( " AE[%s] can place Currency[USA] order.", AE.c_str() );
            FUSDLockAE.Delete( AE );
        }
        else
        {
            UFC::BufferedLog::Printf( " AE[%s] can NOT place Currency[USA] order.", AE.c_str() );
            FUSDLockAE.Add( AE );
        }
    }
    else ///< Not currency. Lock/Unlock product.
    {
        if( CanUse > 0 )
        {  
            UFC::BufferedLog::Printf( " Symbol[%s] unlocked.]",  Currency.c_str() );
            FLockSymbolTable.Delete( Currency );
        }
        else
        {
            UFC::BufferedLog::Printf( " Symbol[%s] locked.]", Currency.c_str() );
            FLockSymbolTable.Add( Currency );
        }
    }    
}
//------------------------------------------------------------------------------


