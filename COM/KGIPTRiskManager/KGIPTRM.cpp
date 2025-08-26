#include "KGIPTRM.h"
#include "iniFile.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL RecoverMode;
extern UFC::AnsiString AppName;
//------------------------------------------------------------------------------
UFC::PHashMap<UFC::AnsiString,UFC::AnsiString* >  FCurrencySymbolTable;
//------------------------------------------------------------------------------
void SymbolCurrency( const UFC::AnsiString& Symbol, UFC::AnsiString& Currency )
{
    UFC::AnsiString  PID( Symbol.c_str(), 3 );
    UFC::AnsiString* SymCurr;
    
    if( FCurrencySymbolTable.GetObjectByKey( PID, SymCurr ) == TRUE )    
        Currency = *SymCurr; ///< Get currency from symbol table. 
    else
        Currency = "TWD";    ///< Not found! Use TWD. 
}
//------------------------------------------------------------------------------
KGIPTRiskManager::KGIPTRiskManager( )
:FTimeOutSec( 30 )
,FCountDown( 30 ) 
,FToSpeedySubject( "KGIDelta" ) 
,FToSpeedyKey( UFC::Hostname ) 
,FNodeName( "Data" )
,FFutOrderSubject( AppName + ".FUT" )
,FOptOrderSubject( AppName + ".OPT")
{    
    
    UFC::AnsiString LogName;

    UFC::BufferedLog::SetDebugMode( DumpOnly );
    LogName.Printf( "../log/%s.%s.log", AppName.c_str(), UFC::GetDateString().c_str() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );
    
    UFC::BufferedLog::Printf( "__________________ Setting ___________________" );
    UFC::BufferedLog::Printf( " LogName:[%s]", LogName.c_str() );
        
    LoadSetting();
       
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   KGI PT Risk control module " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    UFC::BufferedLog::Printf( "   AS400 To Speedy [%s][%s]", FToSpeedySubject.c_str(), FToSpeedyKey.c_str() );
    UFC::BufferedLog::Printf( "                       " );
    UFC::BufferedLog::Printf( "   Timeout between AS400 [%d]sec", FTimeOutSec );
    UFC::BufferedLog::Printf( "______________________________________________" );
    ///< Create the connection object.
    
    FMessageObject = new MessageObject( AppName, "1.0.0", "GrandCathaySec RM connection." );
    FMessageObject->AddListener( FFutOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FOptOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FToSpeedySubject, FToSpeedyKey, this );
    FMessageObject->Start( );
}
//------------------------------------------------------------------------------
KGIPTRiskManager::~KGIPTRiskManager( void )
{
    
}
//------------------------------------------------------------------------------
void KGIPTRiskManager::LoadSetting( void )
{
    UFC::AnsiString FileName,Name,Currency,Value;
    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    try
    {
        UFC::UiniFile   Config( FileName );
        UFC::Section*   IniSection;

        Config.GetValue( "Setting", "Subject",  FToSpeedySubject );
        Config.GetValue( "Setting", "Key",  FToSpeedyKey );
        Config.GetValue( "Setting", "NodeName", FNodeName );
        if( Config.GetValue( "Setting", "TimeoutSec", Value ) == TRUE )
        {
            FTimeOutSec = Value.ToInt();
            if( FTimeOutSec <= 5 )
                FTimeOutSec = 5;
            FCountDown = FTimeOutSec;
        }
        AccountInfo* AccInfo; 
        
        if( (IniSection = Config.GetSection( "AEPrefix" )) != NULL )
        {
            UFC::BufferedLog::Printf( " %d prefix setting.", IniSection->ItemCount() );
            for( int i = 0; i < IniSection->ItemCount(); i++)
            {
                if( IniSection->GetNameValue( i, Name, Value ) && 
                    Name.Length() >= 3 && 
                    Value.Length() >= 7 )
                {   
                    UFC::AnsiString Perfix( Name.c_str(), 3 );
                    UFC::AnsiString Account( Value.c_str(), 7 );                    
                    
                    UFC::BufferedLog::Printf( " AE with prefix[%s] can use account[%s].", Perfix.c_str(), Account.c_str());
                    Name.SetLength( 3 );
                    if( Value.Length() >= 9 )
                    {
                        if( Value[7] == ',' && Value[8] == 'N' )
                        {
                            UFC::AnsiString BypassKey;
                            
                            Value.SetLength( 7 );
                            BypassKey.Printf( "%s%s", Name.c_str(), Value.c_str() );
                            FBypassPrefixSet.Add( BypassKey );
                            UFC::BufferedLog::Printf( "   - AE with prefix[%s] bypass check delta.", Name.c_str() );
                            UFC::BufferedLog::Printf( "   - Add bypass Key[%s] ", BypassKey.c_str());
                        }
                    }
                    Value.SetLength( 7 );
                    UFC::PHashedSet<UFC::AnsiString>* AccSet;
                    
                    if( FPrefixAccMap.GetObjectByKey( Name, AccSet ) == false )
                    {
                        AccSet = new UFC::PHashedSet<UFC::AnsiString>();
                        FPrefixAccMap.Add( Name, AccSet );                    
                    }
                    AccSet->Add( Value.c_str() );                    
                    
                }
            }
        }        
        UFC::BufferedLog::Printf( "_____________ Account Mapping ________________" );        
        if( (IniSection = Config.GetSection( "Account" )) != NULL )
        {
            for( int i = 0; i < IniSection->ItemCount(); i++)
            {
                if( IniSection->GetNameValue( i, Name, Value ) &&
                    Name.Length() == 6 &&
                    Value.Length() == 7)
                {                    
                    AccInfo = new AccountInfo( Value );
                    FAccountMap.Add( Name, AccInfo );                    
                    FAccountMap.Add( Value, AccInfo );                    
                    UFC::BufferedLog::Printf( " [%s] maps to [%s].", Name.c_str(), Value.c_str());
                }
            }
        }        
        if( FAccountMap.IsExists( "000000" ) == false )
        {
            AccInfo = new AccountInfo( "0000000" );
            FAccountMap.Add( "000000",  AccInfo );                    
            FAccountMap.Add( "0000000", AccInfo );                                
            UFC::BufferedLog::Printf( " [000000] maps to [0000000]");
        }        
        if( FAccountMap.GetObjectByKey( "000000", AccInfo ) == true )
        {
            FAccountMap.Add( "8888880", AccInfo );                    
            UFC::BufferedLog::Printf( " [000000] maps to [8888880]");
        }
        UFC::BufferedLog::Printf( "_____________ Product Currency _______________" );        
        ///< Load Symbol/Currency table. 
        if( (IniSection = Config.GetSection( "Currency" )) != NULL )
        {
            for( int i = 0; i < IniSection->ItemCount(); i++)
            {
                if( IniSection->GetNameValue( i, Name, Currency ) )
                {                    
                    FCurrencySymbolTable.Add( Name, new UFC::AnsiString(Currency));                    
                    UFC::BufferedLog::Printf( " Contract[%s] use currency[%s].", Name.c_str(),Currency.c_str());
                }
            }
        }
        else
            UFC::BufferedLog::Printf( " No [Currency] setting all contract use currency TWD.");
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.", FileName.c_str() );
        exit(1);
    }
}
//------------------------------------------------------------------------------
bool KGIPTRiskManager::IsCancelReplacePx( const UFC::AnsiString& Order )
{
    int Func = UFC::AnsiString::StrToInt32( Order.c_str() +2, 2 );    
    if( Func == 6 /*Replace Px */|| Func == 3 /*Cancel*/)
        return true;
    return false;
}
//---------------------------------------------------------------------------
void KGIPTRiskManager::GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data ( Data:  Group;AE )
    Int32 SplitPos = Key.AnsiPos(',');    

    if( SplitPos !=  -1 )
    {
        UFC::AnsiString  UserData( Key.c_str() + SplitPos + 1 );//< Skip ,

        if( UserData.AnsiPos( "ClOrdID=") == 0 )///< From Orc
        {
            AE = Key.SubString( 0, SplitPos );
        }   
        else ///< From API
        {
            if( UserData.Length() >= 6 )
                AE = UserData.SubString( 0, 6 );        
            else
                AE = UserData;   
        }
    }
    else
        AE = Key;            
    AE.PadThis( 6,' ');
}
//---------------------------------------------------------------------------
bool KGIPTRiskManager::AECanUseAccount( const UFC::AnsiString& AE, const UFC::AnsiString& Account )
{
    UFC::PHashedSet<UFC::AnsiString>* AccSet;
    UFC::AnsiString  Key( AE.c_str(), 3 );
                    
    if( FPrefixAccMap.GetObjectByKey( Key, AccSet ) == true )
    {
        if( AccSet->Exists( Account ) == true )
            return true; ///< can use this account.           
    }    
    return false;
}
//------------------------------------------------------------------------------
void KGIPTRiskManager::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& , MTree* Data )
{
    UFC::AnsiString Order, Key, AE;
    Int32 NID;    
    
    if( Subject == FToSpeedySubject ) ///< Recv Margin from Backend.
    {
        GetRiskInfo( Data );
        return;
    }    
        
    if( Data->get( "ORDER", Order ) == TRUE &&  Data->get( "NID", NID ) == TRUE && Data->get( "KEY", Key ) == TRUE )
    {
        UFC::BufferedLog::Printf( " Recv from Speedy Gateway[%s]", Order.c_str() );
        UFC::AnsiString Symbol( Order.c_str() + 34, 20 );            
        UFC::AnsiString Account( Order.c_str() + 26, 7 );

        Symbol.TrimRight();
        GetAE( Key, AE ); ///< Data first 6 digi as AE                            
        if( Subject == FFutOrderSubject ) ///< Has margin.
        {
            if( IsCancelReplacePx( Order ) ) ///< Skip Cancel/ Replace PX
                Pass( AE, "RESULT.FUT", Data );
            else
                Check( AE, Account, Symbol, "RESULT.FUT", Data );
        }
        else if( Subject == FOptOrderSubject )
        {
            if( IsCancelReplacePx( Order ) ) ///< Skip Cancel/ Replace PX
                Pass( AE, "RESULT.OPT", Data );
            else
                Check( AE, Account, Symbol, "RESULT.OPT", Data );            
        }
        else
            UFC::BufferedLog::Printf( " unknown subject[%s].", Subject.c_str() );    
    }
    else
        UFC::BufferedLog::Printf( " Missing ORDER KEY or NID field." );    
}
//-------------------------------------------------------------------------------
void KGIPTRiskManager::Pass( const UFC::AnsiString& AE, const UFC::AnsiString& Subject, MTree* Data )
{
    UFC::BufferedLog::DebugPrintf( " AE[%s] place order OK.", AE.c_str() );
    Data->append( "CODE", 1 );
    FMessageObject->Send( Subject, UFC::Hostname, *Data );
}
//-------------------------------------------------------------------------------
void KGIPTRiskManager::Check( const UFC::AnsiString& AE, const UFC::AnsiString& Account, const UFC::AnsiString& Symbol, const UFC::AnsiString& Subject, MTree* Data )
{
    UFC::AnsiString Reason;
    
    UFC::BufferedLog::Printf( " ------------ Begin Check ------------" );
    if( AECanUseAccount( AE, Account ) == false ) ///< Can AE use this TAIFEX account ?
    {
        UFC::BufferedLog::Printf( " [%s] has no rights to use [%s].", AE.c_str(), Account.c_str() );
        Reason.Printf( "[%s] no rights to use [%s].", AE.c_str(), Account.c_str() );
        Data->append( "CODE", 0 );
        Data->append( "MSG", Reason );
        FMessageObject->Send( Subject, UFC::Hostname, *Data );        
    }
    else
    {
        if( AccountCanPlaceOrder( AE, Account, Symbol, Reason ) == false )        
        {
            UFC::BufferedLog::Printf( " AE[%s] margin not enough.", AE.c_str() );
            Data->append( "CODE", 0 );
            Data->append( "MSG", Reason  );
            FMessageObject->Send( Subject, UFC::Hostname, *Data );
        }
        else
            Pass( AE, Subject, Data );
    }
    UFC::BufferedLog::Printf( " ------------- End Check -------------" );
}
//-------------------------------------------------------------------------------
//  Filed    Length   
//-------------------------------------------------------------------------------
//  AE       6        AE
//  Flag     1        Y/N    
//  Currency 3        
//-------------------------------------------------------------------------------
void KGIPTRiskManager::GetRiskInfo( MTree* Data )
{
    UFC::AnsiString RiskInfo;
    
    if( Data->get( FNodeName, RiskInfo ) && RiskInfo.Length() >= 10 )
    {
        UFC::AnsiString AE( RiskInfo.c_str() , 6 );
        char            Flag = RiskInfo[ 6 ];
        UFC::AnsiString Curr( RiskInfo.c_str() + 7, 3 );

        FCountDown = FTimeOutSec;///< Reset count down.        
        if( Flag == 'Y' || Flag == 'y' ) ///< Y <-- Can't place Order.
        {
            SetAccountInfo( AE, Curr, FALSE );
            UFC::BufferedLog::Printf( " AE[%s] Currency[%s] Can't place order[%c]", AE.c_str(), Curr.c_str(), Flag );            
        }
        else if( Flag == 'N' || Flag == 'n' ) ///< N <-- Can place Order.
        {
            SetAccountInfo( AE, Curr, TRUE );
            UFC::BufferedLog::Printf( " AE[%s] Currency[%s] Can place order[%c]", AE.c_str(), Curr.c_str(), Flag );            
        }
    } 
}
//-------------------------------------------------------------------------------
BOOL KGIPTRiskManager::AccountCanPlaceOrder( const UFC::AnsiString& AE, const UFC::AnsiString& Account, const UFC::AnsiString& Symbol, UFC::AnsiString& Reason )
{
    UFC::AnsiString Currency;
    AccountInfo*    AccInfo;
    
    ///< Get Currency from Symbol.
    SymbolCurrency( Symbol, Currency );
    UFC::BufferedLog::DebugPrintf( " AE[%s] Account[%s] place an [%s]order Currency[%s].", AE.c_str(), Account.c_str(), Symbol.c_str(), Currency.c_str() );
    
    UFC::PLockObject Lock(  FAccountLimitCS );    
    
    ///< Has currency margin ?
    if( FAccountMap.GetObjectByKey( Account, AccInfo ) == true ) ///< Account exists ?
    {
        if( AccInfo->CanPlaceCurrency( Currency ) == false) ///< Currency margin exists ?
        {
            UFC::BufferedLog::Printf( " AE[%s] Account[%s] Currency[%s] no margin.", AE.c_str(), Account.c_str(), Currency.c_str() );
            Reason.Printf( "Exceed currency[%s] Margin.", Currency.c_str());
            return FALSE;
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " AE[%s] Can not use Account[%s]", AE.c_str(), Account.c_str() );
        Reason.Printf( "Can not use Account[%s].", Account.c_str());
        return FALSE;
    }
    ///< AE can palce order of the given currency?
    if( AcceptAE( Currency, Account, AE, Reason ) == FALSE )
        return FALSE;
    return TRUE;
}
//-------------------------------------------------------------------------------
bool  KGIPTRiskManager::IsDepartment( const UFC::AnsiString& AE )
{
    if(  AE.Length() >= 6 &&  AE[3]== ' ' && AE[4]== ' ' && AE[5]==' ')
        return true;
    return false;
}
//-------------------------------------------------------------------------------
void  KGIPTRiskManager::SetAccountInfo( const UFC::AnsiString& AE, const UFC::AnsiString& Currency, BOOL AcceptOrd )
{
    AccountInfo* AccInfo;
    UFC::PLockObject Lock(  FAccountLimitCS );
    
    if( FAccountMap.GetObjectByKey( AE, AccInfo ) == true ) ///< AE is a account.
    {
        if( AcceptOrd == TRUE )///< Turn on Currency margin
        {
            AccInfo->AddCurrency( Currency );        
            UFC::BufferedLog::Printf( " Can place Currency[%s] order.", Currency.c_str() );
        }
        else///< Turn off Currency margin
        {
            AccInfo->DelCurrency( Currency );
            UFC::BufferedLog::Printf( " Can NOT place Currency[%s] order.", Currency.c_str() );                        
        }
    }
    else if( IsDepartment( AE ) == true  ) ///< AE is a Department.
    {
        UFC::AnsiString  Dep( AE.c_str(), 3 );    
        
        if( FDepartmentMap.GetObjectByKey( Dep, AccInfo ) == false ) 
        {
            AccInfo = new AccountInfo( Dep );   
            FDepartmentMap.Add( Dep, AccInfo );
        }
        if( AcceptOrd == TRUE )///< Turn on Currency margin
        {
            AccInfo->AddCurrency( Currency );        
            UFC::BufferedLog::Printf( " Department[%s] can place Currency[%s] order.", Dep.c_str(), Currency.c_str() );
        }
        else///< Turn off Currency margin
        {
            AccInfo->DelCurrency( Currency );
            UFC::BufferedLog::Printf( " Department[%s] can NOT place Currency[%s] order.", Dep.c_str(), Currency.c_str() );                        
        }                
    }
    else
    {
        if( AcceptOrd == TRUE )
        {
            AddAE( Currency, AE ); ///< AE can place order of this currency.
            UFC::BufferedLog::Printf( " AE[%s] can place Currency[%s] order.", AE.c_str(), Currency.c_str() );            
        }
        else
        {
            DeleteAE( Currency, AE );///< AE can NOT place order of this currency.
            UFC::BufferedLog::Printf( " AE[%s] can NOT place Currency[%s] order.", AE.c_str(), Currency.c_str() );                        
        }    
    }    
}
//------------------------------------------------------------------------------
void KGIPTRiskManager::CountDown( )
{
    int MissSec;
    
    FCountDown--;
    MissSec = FTimeOutSec - FCountDown;
    if( MissSec % (FTimeOutSec/10) == 0 )
        UFC::BufferedLog::Printf( " Don't receive data from AS400 [%d]sec .", MissSec );
    if( FCountDown <= 0 )
    {
        UFC::BufferedLog::Printf( " Clear all data, No one can place order now." );
        UFC::PLockObject Lock(  FAccountLimitCS );
        
        FCountDown = FTimeOutSec;        
        ///< Clear all accounts currency margin.
        AccountInfo* ClearAcc = FAccountMap.First();
        
        while( ClearAcc != NULL )
        {
            ClearAcc->ClearCurrency();
            ClearAcc = FAccountMap.Next();
        };
        ///< Clear all Departments currency margin.
        ClearAcc = FDepartmentMap.First();
        
        while( ClearAcc != NULL )
        {
            ClearAcc->ClearCurrency();
            ClearAcc = FDepartmentMap.Next();
        };        
        
        ///< Clear all AE.
        UFC::PHashedSet<UFC::AnsiString>* DelSet = FCurrencyAcceptAE.First();
        
        while( DelSet != NULL )
        {
            delete DelSet;
            DelSet = FCurrencyAcceptAE.Next();
        };
        FCurrencyAcceptAE.Clear();
    }
}
//------------------------------------------------------------------------------
void KGIPTRiskManager::AddAE( const UFC::AnsiString& Currency, const UFC::AnsiString& AE )
{
    UFC::PHashedSet<UFC::AnsiString>* AESet;
    
    if( FCurrencyAcceptAE.GetObjectByKey( Currency, AESet ) == FALSE )
    {
        AESet = new UFC::PHashedSet<UFC::AnsiString>();
        FCurrencyAcceptAE.Add( Currency, AESet );
    }
    AESet->Add( AE );
}
//------------------------------------------------------------------------------
void KGIPTRiskManager::DeleteAE( const UFC::AnsiString& Currency, const UFC::AnsiString& AE )
{
    UFC::PHashedSet<UFC::AnsiString>* AESet;
    
    if( FCurrencyAcceptAE.GetObjectByKey( Currency, AESet ) == TRUE )        
        AESet->Delete( AE );    
}
//------------------------------------------------------------------------------
BOOL KGIPTRiskManager::AcceptAE( const UFC::AnsiString& Currency, const UFC::AnsiString& Account, const UFC::AnsiString& AE, UFC::AnsiString& Reason )
{   
    UFC::AnsiString  AEPrefix( AE.c_str(), 3 );
    UFC::AnsiString  BypassKey;
    
    BypassKey.Printf( "%s%s", AEPrefix.c_str(), Account.c_str() );    
    UFC::BufferedLog::Printf( " Check bypass delta Key[%s] ", BypassKey.c_str());
    if( FBypassPrefixSet.Exists( BypassKey ) == FALSE ) ///< Prefix not exists.    
    {   
        ///< Chack department flag
        AccountInfo* AccInfo;
        
        if( FDepartmentMap.GetObjectByKey( AEPrefix, AccInfo) == TRUE )
        {
            if( AccInfo->CanPlaceCurrency( Currency ) == FALSE )
            {
                UFC::BufferedLog::Printf( " Department[%s] can't place currency[%s] order.", AEPrefix.c_str(), Currency.c_str() );
                Reason.Printf( "Exceed Department[%s] Currency[%s] Margin.", AEPrefix.c_str(), Currency.c_str() );                        
                return FALSE;
            }                
        }
        else
        {
            UFC::BufferedLog::Printf( " AE[%s] department[%s] not exists.", AE.c_str(), AEPrefix.c_str() );
            Reason.Printf( " AE[%s] department[%s] not exists.", AE.c_str(), AEPrefix.c_str() );
            return FALSE;
        }
        ///< Chack AE flag
        UFC::PHashedSet<UFC::AnsiString>* AESet;

        if( FCurrencyAcceptAE.GetObjectByKey( Currency, AESet ) == TRUE )        
        {
            if( AESet->Exists( AE ) == false )    
            {
                UFC::BufferedLog::Printf( " AE[%s] can't place currency[%s] order.", AE.c_str(), Currency.c_str() );
                Reason.Printf( "Exceed AE[%s] Currency[%s] Margin.", AE.c_str(), Currency.c_str() );                        
                return FALSE;
            }
            else
                return TRUE;
        }
        UFC::BufferedLog::Printf( " AE[%s] No AE can place currency[%s] order.", AE.c_str(), Currency.c_str() );
        Reason.Printf( "Exceed AE[%s] Currency[%s] Margin.", AE.c_str(), Currency.c_str() );        
        return FALSE;
    }
    else
    {
        UFC::BufferedLog::Printf( " AE[%s] Account[%s] with prefix[%s] bypass check delta.", AE.c_str(), Account.c_str(), AEPrefix.c_str() );
        return TRUE; ///< AE with this prefix by pass delta chack.
    }
}
//------------------------------------------------------------------------------

