#include "RMConnection.h"
#include "iniFile.h"
#include "NetUtility.h"
#include "../ExchangeUtility/Utility.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL KeepLog;
extern BOOL RecoverMode;
extern BOOL AS400Check;
extern BOOL Newfprintf;
extern UFC::AnsiString AppName;
extern UFC::BufferedLog* GLog;
#define EXECID_LEN 64
//------------------------------------------------------------------------------
AccountInfo::AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE )
:FAE( AE )
,FAccount( Account )
{
}
//------------------------------------------------------------------------------
double AccountInfo::GetCanUse()
{
    UFC::PLockObject Lock( FCS );
    return FCanUse;
}
//------------------------------------------------------------------------------
void AccountInfo::SetCanUse( double CanUse )
{
    UFC::PLockObject Lock( FCS );
    
    FCanUse = CanUse;
}
//------------------------------------------------------------------------------
BOOL AccountInfo::CanPlaceOrder( double SubAmt )
{
    UFC::PLockObject Lock( FCS );
    
    if( FCanUse - SubAmt < 0 )
    {
        if( KeepLog == TRUE )
            GLog->fprint( " Reject! Margin not enough. Need[", UFC::DoubleToStr(SubAmt,5,2),"] CanUse[", UFC::DoubleToStr(FCanUse,5,2), "]" );
        return FALSE;
    }
    else
    {
        double BeforCanUse = FCanUse;
        
        FCanUse -= SubAmt;
        if( KeepLog == TRUE )
            GLog->fprint( " Accept! Margin is enough.  Need[", UFC::DoubleToStr(SubAmt,5,2),"] CanUse[", UFC::DoubleToStr(BeforCanUse,5,2),"] Margin leaves[", UFC::DoubleToStr(FCanUse,5,2),"]" );
        return TRUE;
    }
}
//------------------------------------------------------------------------------
void AccountInfo::CancelOrder( double AddAmt )
{
    UFC::PLockObject Lock( FCS );
    
    FCanUse += AddAmt;
}
//------------------------------------------------------------------------------
void AccountInfo::FilledOrder( double AddAmt )
{
    UFC::PLockObject Lock( FCS );
    
    FCanUse += AddAmt;
}
//------------------------------------------------------------------------------
//
// Class GCRMConnection
//
//------------------------------------------------------------------------------
GCRMConnection::GCRMConnection( int RecoverBeginTime )
:FIP( "127.0.0.1" )
,FPort( 12345 )
,FExecIDTable( 300000 )
,FGatewayIP( "127.0.0.1" )
,FGatewayPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000000" )
,FBrokerID( "F002000" )
,FAS400WebID( "SPD" )
,FAS400Src( 'S' )
,FRecoverBeginTime( RecoverBeginTime )
,FAS400OrderSeq( 0 )
,FIsLogon( FALSE )
,FRejectMarketOrd( TRUE )
,FReleaseMarginOnFilled( FALSE )
,FReleaseMarginOnRODCxl( FALSE )
,FReleaseMarginOnFOKIOCCxl( TRUE )
,FToSpeedySubject( "Risk") ///.TOSPEEY" )
,FToSpeedyKey( "ToSpeedy") ///.TOSPEEY" )
,FFutOrderSubject( AppName + ".FUT" )
,FOptOrderSubject( AppName + ".OPT")
,F400CheckOrderSubject( "To400Check" )
,F400CheckOrderKey( UFC::Hostname )
,F400CheckResultSubject( "400CheckResult")
,F400CheckResultKey( UFC::Hostname )
{    
    LoadSetting();
    UFC::AnsiString ExecIDFile;
    UFC::AnsiString FileName;

    ExecIDFile.Printf( "ExecID.%s.DATA", UFC::GetDateString().c_str() );
        
    GLog->fprint( "______________________________________________" );
    GLog->fprint( "                                              " );
    GLog->fprint( "   Dynamic loading Risk control module " );
    GLog->fprint( "   Startup at:", UFC::Hostname );
    GLog->fprint( "                                          ");
    GLog->fprint( "   Build Date:", __DATE__ );
    if( KeepLog == TRUE ) 
        GLog->fprint( "   Debug Mode:On");    
    else
        GLog->fprint( "   Debug Mode:Off");
    if( FRejectMarketOrd == TRUE )
        GLog->fprint( "   Market Order:Reject");
    else
        GLog->fprint( "   Market Order:Accept");
    GLog->fprint( "   AS400 To Speedy [", FToSpeedySubject.c_str(),"][", FToSpeedyKey.c_str(),"]" );
    GLog->fprint( "   AS400 Order Checker [", F400CheckOrderSubject.c_str(),"][", F400CheckOrderKey.c_str(),"]" );    
    GLog->fprint( "   AS400 Check Result  [", F400CheckResultSubject.c_str(),"][", F400CheckResultKey.c_str(),"]" );        
    GLog->fprint( "   Recover BeginTime:[", UFC::IntToStr( FRecoverBeginTime, 6, true ), "]" );    
    GLog->fprint( "                       " );
    GLog->fprint( "   Release Margin on Filled[", (FReleaseMarginOnFilled == TRUE)? "Yes" : "No","]" );
    GLog->fprint( "   Release Margin on ROD Canceled[", (FReleaseMarginOnRODCxl == TRUE)? "Yes" : "No","]" );
    GLog->fprint( "   Release Margin on FOK,IOC Canceled[", (FReleaseMarginOnFOKIOCCxl == TRUE)? "Yes" : "No","]" );
    GLog->fprint( "                       " );    
    GLog->fprint( "   Enable AS400 Checker[", (AS400Check == TRUE)? "Yes" : "No","]" );    
    GLog->fprint( "__________________________________________" );               
    
    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    FConfig = new UFC::UiniFile( FileName );
    
    GLog->fprint( " Load ", FileName.c_str(), " OK." );
    
    FileName.Printf( "../cfg/AS400ErrorMessage.cfg" );
    FErrMsg = new UFC::UiniFile( FileName ); 
    
    GLog->fprint( " Load ", FileName.c_str(), " OK." );
    
    if( RecoverMode == FALSE )
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "w" );
    }
    else
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "r+" );
        RecoverExecID();
    }    
    GLog->fprint( " RecoverExecID OK.");    
    ///< Create the connection object.
    FConnection = new TTaifexConnection( AppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                                    
    GLog->fprint( " Create TTaifexConnection OK.");    
    
}
//------------------------------------------------------------------------------
void GCRMConnection::InitMBus( MessageObject* MBusObj )
{
    FMessageObject = MBusObj;
    FMessageObject->AddListener( FToSpeedySubject, FToSpeedyKey, this );
    if( AS400Check == TRUE )
        FMessageObject->AddListener( F400CheckResultSubject, F400CheckResultKey, this );    
}
//------------------------------------------------------------------------------
void GCRMConnection::RecoverExecID( void )
{
    FExecIDFile->Seek( 0, UFC::soFromBeginning );
    UFC::AnsiString ExecID;
    while( FExecIDFile->GetPosition() < FExecIDFile->GetSize() )
    {
        UFC::MemoryStream Buffer( EXECID_LEN, EXECID_LEN );
        memset( (void*)Buffer.GetBuffer(), '\0', EXECID_LEN );
        FExecIDFile->Read( (void*)Buffer.GetBuffer(), EXECID_LEN );
        ExecID.LoadFromStream( &Buffer );
        ExecID.TrimRight();
        FExecIDTable.Add( ExecID );
    }
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


        Config.GetValue( "Setting", "MBusIP", FIP );
        Config.GetValue( "Setting", "GatewayIP", FGatewayIP );
        if( Config.GetValue( "Setting", "GatewayPort", Value ) )
            FGatewayPort = Value.ToInt();
        if( Config.GetValue( "Setting", "RejectMarketOrder", Value ) )
            FRejectMarketOrd = Value.ToInt();
        if( Config.GetValue( "Setting", "AS400Check", Value ) )
            AS400Check = Value.ToInt();                
        if( Config.GetValue( "Setting", "Source", Value ) )
            FAS400Src = Value[ 0 ];                
        Config.GetValue( "Setting", "WebID", FAS400WebID );
        Config.GetValue( "Setting", "ID", FID );
        Config.GetValue( "Setting", "Password", FPassword );
        Config.GetValue( "Setting", "Account", FAccount );
        Config.GetValue( "ToSpeedy", "Subject",  FToSpeedySubject );
        Config.GetValue( "ToSpeedy", "Key",  FToSpeedyKey );
        if( Config.GetValue( "Setting", "ReleaseMarginOnFilled", Value ) )
            FReleaseMarginOnFilled  = Value.ToInt();
        if( Config.GetValue( "Setting", "ReleaseMarginOnRODCanceled", Value ) )
            FReleaseMarginOnRODCxl = Value.ToInt();
        if( Config.GetValue( "Setting", "ReleaseMarginOnIOCFOKCanceled", Value ) )
            FReleaseMarginOnFOKIOCCxl = Value.ToInt();

        if( Config.GetValue( "Futures", "Default", Value ) )
            FDefFutDeposit = Value.ToInt();
        else
            FDefFutDeposit = 100000;
        if( Config.GetValue( "OptionsCM", "Default", Value ) )
            FDefOptCM = Value.ToInt();
        else
            FDefOptCM = 100;
        if( Config.GetValue( "OptionsA", "Default", Value ) )
            FDefOptAValue = Value.ToInt();
        else
            FDefOptAValue = 100;
        
        GLog->fprint( " Default Futures deposit[", UFC::DoubleToStr( FDefFutDeposit, 5, 2), "]" );
        GLog->fprint( " Default Options A value[", UFC::DoubleToStr( FDefOptAValue, 5, 2), "]" );
        GLog->fprint( " Default Options Contract Multiplier[", UFC::DoubleToStr( FDefOptCM, 5, 2), "]"  );
    
        FFutDeposit.Clear();
        if( (IniSection = Config.GetSection( "Futures")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue(i,Name,Value) && Name != "Default" )
                {
                    double Deposit = Value.ToDouble();
                    FFutDeposit.Add(Name,Deposit);
                    GLog->fprint( " Futures deposit[", Name.c_str(),"][",UFC::DoubleToStr( Deposit, 5, 2 ),"]" );
                }
            }
        }
        FOptAValue.Clear();
        if( (IniSection = Config.GetSection( "OptionsA")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue(i,Name,Value) && Name != "Default" )
                {
                    double AVal = Value.ToDouble();
                    
                    FOptAValue.Add(Name,AVal);
                    GLog->fprint( " Options A Value[", Name.c_str(),"][", UFC::DoubleToStr(AVal,5,2),"]" );                    
                }
            }
        }
        FOptCM.Clear();
        if( (IniSection = Config.GetSection( "OptionsCM")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue(i,Name,Value) && Name != "Default" )
                {
                    double CM = Value.ToDouble();
                    
                    FOptCM.Add(Name,CM);
                    GLog->fprint( " Options Contract Multiplier[", Name.c_str(), UFC::DoubleToStr( CM,5,2), "]" );                    
                }
            }
        }
    }
    catch( UFC::Exception& e )
    {
        GLog->fprint( " Exception[", e.what(), "]" );        
        exit(1);
    }
}
//------------------------------------------------------------------------------
GCRMConnection::~GCRMConnection( void )
{
    ///< Delete TTaifexConnection object if exists.    
    if( FConnection != NULL )
    {
        FConnection->Logoff();
        delete FConnection;
    }
}
//------------------------------------------------------------------------------
double GCRMConnection::GetFuturesDeposit( const UFC::AnsiString& SymbolPrefix )
{
    double           Value;
    UFC::PLockObject Lock( FCommodCS );

    if( FFutDeposit.GetObjectByKey( SymbolPrefix ,Value) == true )
        return Value;
    return FDefFutDeposit;
}
//------------------------------------------------------------------------------
double GCRMConnection::GetOptionsCM( const UFC::AnsiString& SymbolPrefix )
{
    double           Value;
    UFC::PLockObject Lock( FCommodCS );

    if( FOptCM.GetObjectByKey( SymbolPrefix ,Value) == true )
        return Value;
    return FDefOptCM;
}
//------------------------------------------------------------------------------
double GCRMConnection::GetOptionsA( const UFC::AnsiString& SymbolPrefix )
{
    double           Value;
    UFC::PLockObject Lock( FCommodCS );

    if( FOptAValue.GetObjectByKey( SymbolPrefix ,Value) == true )
        return Value;
    return FDefOptAValue;
}
//------------------------------------------------------------------------------
double GCRMConnection::GetCommodDeposit( const UFC::AnsiString& Symbol )
{
    UFC::AnsiString  SymbolPrefix( Symbol.c_str(), 3 );    
    return GetFuturesDeposit( SymbolPrefix );
}
//------------------------------------------------------------------------------
double GCRMConnection::GetOptionsDeposit( const UFC::AnsiString& Symbol, double Px, bool IsBuy )
{
    UFC::AnsiString  Sym = Symbol;
    UFC::AnsiString  SymbolPrefix( Symbol.c_str(), 3 );    

    if( Sym.Length() > 10 )///< Multileg order(Same MXF)
    {
        return GetFuturesDeposit( "MXF" );
    }
    else if( Sym.Length() == 10 )///< Single Order
    {
        double CM,AVal;
        CM = GetOptionsCM( SymbolPrefix );
        if( IsBuy == FALSE )
        {
            AVal = GetOptionsA( SymbolPrefix );
            return Px*CM + AVal;
        }
        else
            return Px*CM;
    }
    else
        return FDefFutDeposit;
}
//------------------------------------------------------------------------------
bool GCRMConnection::IsReplacePx( const UFC::AnsiString& Order )
{
    if( Order.SubString( 2, 2 ).ToInt() == 6 )
        return true;
    return false;
}
//---------------------------------------------------------------------------
void GCRMConnection::GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = Key.AnsiPos(',');

    if( SplitPos !=  -1 )
        AE = Key.SubString( 0, SplitPos );
    else
        AE = Key;
}
//------------------------------------------------------------------------------
void GCRMConnection::UseMargin( const UFC::AnsiString& Account, UFC::AnsiString& AE, bool NeedLog )
{
    UFC::AnsiString Value;
    int  MarginLevel = 3; ///< Default: AE Margin

    if( FConfig->GetValue( Account, AE, Value ) == true )
        MarginLevel = Value.ToInt();
    if( MarginLevel == 1 )
    {
        AE = "      ";
        if( KeepLog == TRUE && NeedLog == TRUE )
            GLog->fprint( " Use Company margin. Account[", Account.c_str(),"] AE[", AE.c_str(), "] " );        
    }
    else if( MarginLevel == 2 )
    {
        AE.SetLength( 3 );
        AE.PadThis( 6,' ');
        if( KeepLog == TRUE && NeedLog == TRUE  )
            GLog->fprint( " Use Group margin. Account[", Account.c_str(),"] AE[",AE.c_str(), "] " );
    }
    else
    {
        if( KeepLog == TRUE && NeedLog == TRUE  )
            GLog->fprint( " Use AE margin. Account[", Account.c_str(),"] AE[",AE.c_str(),"] " );        
    }
}
//------------------------------------------------------------------------------
TCheckResult GCRMConnection::CheckOrder( CheckData& Obj )
{    
    UFC::AnsiString AE;
    UFC::AnsiString BrokerID( Obj.Order.c_str() + 14, 7 );
    UFC::AnsiString Account( Obj.Order.c_str() + 26, 7 );
    UFC::AnsiString Symbol( Obj.Order.c_str() + 34, 20 );    
    int             Qty        = UFC::AnsiString::StrToInt32( Obj.Order.c_str() + 65, 4 );            
    char            OpenOffset = Obj.Order[70];
    char            Side       = Obj.Order[54];
    char            OrderType  = Obj.Order[55];

    
    GetAE( Obj.Key, AE );
    Symbol.TrimRight();
    if( KeepLog == TRUE )
    {
        GLog->fprint( " Recv from Speedy Gateway:[", Obj.Order.c_str(), "] " );
        GLog->fprint( " BrokerID[", BrokerID.c_str(), "] Account[", Account.c_str(),"] AE[",AE.c_str(),"] Symbol[",Symbol.c_str(),"] OpenOffset[", UFC::CharToStr( OpenOffset ),"] " );
    }
    UseMargin( Account, AE );
    AccountInfo* Info = GetAccountInfo( Account, AE );
    
    if( IsReplacePx( Obj.Order ) ) ///< Skip Replace PX
        return crPass;
    if( Info == NULL  ) ///< Account information not exosts.
    {
        GLog->fprint( " Can't fetch the record: [", Account.c_str(),",", AE.c_str(), "] " );
        GLog->fprint( " Speedy RM Reject Order:[", Obj.Order.c_str(), "] " );            
        Obj.Msg = "Account not exists.";            
        return crReject;
    }
    if( FRejectMarketOrd == TRUE && OrderType == 'M') ///< Reject Market Order.
    {        
        GLog->fprint( " Speedy RM Reject Order:[", Obj.Order.c_str(),"] " );                        
        Obj.Msg = "Can't place market order.";                
        return crReject;
    }            
    ///< Check Margin.
    if( Obj.Market == mFutures ) ///< Futures
    {
        double CommodMargins = GetCommodDeposit( Symbol );

        if( Info->CanPlaceOrder( CommodMargins * Qty ) == false )        
        {     
            if( AS400Check == FALSE )
            {
                Obj.Msg = "Exceed Margin.";
                return crReject;
            }
            else
            {
                SendToAS400Checker( FALSE, Obj.NID, Obj.Order, Obj.Key );
                return crToOderRM;
            }
        }
        else
            return crPass;
    }
    else ///< Options
    {
        int    Price         = UFC::AnsiString::StrToInt32(  Obj.Order.c_str() + 56, 9 );
        int    Precision     = FConnection->GetPricePrecision( nsOrderMessageDefine::mTWOptions, nsOrderMessageDefine::tsNormal, Symbol );
        double OrderPx       = UFC::IntToDouble( Price, Precision );
        double CommodMargins = GetOptionsDeposit( Symbol, OrderPx, (Side=='B') );

        if( Info->CanPlaceOrder( CommodMargins * Qty ) == false )
        {
            if( AS400Check == FALSE )
            {
                Obj.Msg = "Exceed Margin.";
                return crReject;
            }
            else
            {
                SendToAS400Checker( TRUE, Obj.NID, Obj.Order, Obj.Key );
                return crToOderRM;
            }
        }
        else
            return crPass;
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    if( Subject == F400CheckResultSubject ) ///< Recv check result
        CheckResultFromAS400( Data );
    else if( Subject == FToSpeedySubject ) ///< Recv Margin from Backend.
        ReceiveMarginInfo( Data );    
}
//-------------------------------------------------------------------------------
// Account     [ 7  ] 
// Sub-Account [ 6  ]
// Sign        [ 1  ]    
// Integer     [ 11 ]
// Fraction    [ 2  ]
//-------------------------------------------------------------------------------
void GCRMConnection::ReceiveMarginInfo( MTree* Data )
{
    UFC::AnsiString RiskInfo;
   
    if( Data->get( "Data", RiskInfo ) && RiskInfo.Length() >= 27 )
    {
        UFC::AnsiString Account( RiskInfo.c_str() , 7 );
        UFC::AnsiString AE( RiskInfo.c_str() + 7, 6 );
        char            Sign = RiskInfo[13];
        int             IntVal = UFC::AnsiString( RiskInfo.c_str() + 14, 11 ).ToInt();
        int             FractionVal = UFC::AnsiString( RiskInfo.c_str() + 25, 2 ).ToInt();
        double          CanUse;

        GLog->fprint( " Account[", Account.c_str(), "] AE[", AE.c_str(), "] Sign[", UFC::CharToStr(Sign), "] CanUse[", UFC::IntToStr(IntVal), ".", UFC::IntToStr(FractionVal,2,true), "]" );
        if( Sign == '-' )
            CanUse = -1 *( (double)IntVal + ((double)FractionVal)/100.0 );
        else
            CanUse = (double)IntVal + ((double)FractionVal)/100.0 ;
        SetAccountInfo( Account,  AE,  CanUse );
    }    
}
//-------------------------------------------------------------------------------
void GCRMConnection::CheckResultFromAS400( MTree* Data )
{
    UFC::AnsiString Result;
    int Market = mFutures;
   
    if( Data->get( "Data", Result ) && Result.Length() >= 512 )
    {    
        
        UFC::AnsiString Order  = Result.SubString( 0, 75 );
        UFC::AnsiString Key    = Result.SubString( 145, 259 );
        UFC::AnsiString Status = Result.SubString( 404, 2 );
        UFC::AnsiString ECode  = Result.SubString( 407, 7 );
        int NID = Result.SubString( 134, 11 ).ToInt();
        
        if( Order[0] == '9' && Order[1] == '3' )
        {
            Market = mOption;  
            Order[0] = '3';
            Order[1] = '0';
        }
        Key.TrimRight();
        ECode.TrimRight();

        GLog->fprint( " AS400 Result[", Result.c_str(), "] " );
        GLog->fprint( " Order from AS400[", Order.c_str(), "] ");        
        GLog->fprint( " Status[",Status.c_str(),"] Key[", Key.c_str(), "] "  );                                    

        CheckData RObj( Market, NID, Order, Key, "" );        
        
        if( Status == "00")        
        {
            RObj.Msg = " Pass AS400 Check.";
            if( FResultListener != NULL )
                FResultListener->OnResult( RObj, 1 );
        }    
        else
        {   
            if( FErrMsg->GetValue( "AS400ErrorCode", ECode, RObj.Msg ) == FALSE )
                RObj.Msg.Printf( "Reject by AS400 Code[%s]", ECode.c_str() );
            if( FResultListener != NULL )
                FResultListener->OnResult( RObj, 0 );            
            GLog->fprint( " AS400 Reject[",  RObj.Msg.c_str(), "] " );                            
        }
    }
    else
        GLog->fprint( " Check result from AS400 miss value [Data].");                                    

}
//-------------------------------------------------------------------------------
void GCRMConnection::SendToAS400Checker( BOOL IsOpt, int NID, const UFC::AnsiString& Order, const UFC::AnsiString& Key )
{
    MTree            Data;
    UFC::AnsiString  AE,AS400Order,Sys,Space,NIDKey;    
    
    GetAE( Key, AE );
    AE.PadThis( 6,' ');
    Space.PadThis( 41,' ');//WEBIP(25) + CA(1) +LogonID(10) +GroupID(5)
    NIDKey.Printf("%011d%s", NID, Key.c_str() );
    if( IsOpt == FALSE )
        Sys = "30";
    else
        Sys = "93";
    AS400Order.Printf( "%s%s%s%s%08d%c%s%s", 
                        Sys.c_str(),
                        Order.c_str() + 2, 
                        AE.c_str(), 
                        FAS400WebID.c_str(),
                        FAS400OrderSeq,FAS400Src,
                        Space.c_str(),
                        NIDKey.c_str());
    AS400Order.PadThis( 512, ' ' );
    FAS400OrderSeq++;
    GLog->fprint( " Send Order to AS400[", AS400Order.c_str(), "] " );    
    Data.append( "Data", AS400Order );                
    FMessageObject->Send( F400CheckOrderSubject, F400CheckOrderKey, Data );    
}
//-------------------------------------------------------------------------------
AccountInfo* GCRMConnection::GetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE )
{
    UFC::AnsiString Key;
    AccountInfo*    Acc;

    Key.Printf( "%s%s", Account.c_str(), AE.c_str() );
    if( AccountInfoTable.GetObjectByKey( Key, Acc ) == TRUE )
        return  Acc;
    return NULL;
}
//-------------------------------------------------------------------------------
void  GCRMConnection::SetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, double CanUse )
{
    AccountInfo*    AccInfo;
    UFC::AnsiString Key;

    Key.Printf( "%s%s", Account.c_str(), AE.c_str() );
    if( AccountInfoTable.GetObjectByKey( Key, AccInfo ) == FALSE )
    {
         AccInfo = new AccountInfo( Account, AE );
         AccountInfoTable.Add( Key, AccInfo );
    }
    AccInfo->SetCanUse( CanUse );
}
//-------------------------------------------------------------------------------    
void GCRMConnection::OnConnected( void )
{
    GLog->fprint( " Login( ID=[", FID.c_str(),"], Password=[",FPassword.c_str(),"] )" );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void GCRMConnection::OnDisconnected( void )
{
    GLog->fprint( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void GCRMConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {        
        UFC::AnsiString BeginTime;
        
        FIsLogon = TRUE;       
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        GLog->fprint( " Message[", ReplyString, "]( CID=", UFC::IntToStr( CID ), " )....logon ok!");         
        GLog->fprint( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );        
        FConnection->RecoverFromLocalFile( BeginTime.c_str(), rdBoth, rmAll );        
    }
    else
    {
        GLog->fprint( " Login failed[", ReplyString, "]" );        
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::Recover( const UFC::AnsiString& BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FRecovering = TRUE;
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );            
        GLog->fprint( " Recover from:", BeginTime.c_str() );        
    }
}    
//------------------------------------------------------------------------------
void GCRMConnection::OnRecoverFinished( int Count )
{
    FIsLogon = TRUE;
    FRecovering = FALSE;
    GLog->fprint( " OnRecoverFinished( Count = ", UFC::IntToStr( Count )," )"  );        
    UFC::SleepMS( 1000 );    
}    
//------------------------------------------------------------------------------
void GCRMConnection::CheckConnection( void )
{
    if( FIsLogon == FALSE && FRecovering == FALSE )
    {
        GLog->fprint( " Connect to Gateway.");        
        FConnection->Connect( FGatewayIP.c_str() , FGatewayPort, 3 );    
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup)
{
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::etRejected:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
        case nsOrderMessageDefine::osCanceled:            
        case nsOrderMessageDefine::osReplaced:
             if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced || 
                 Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                return;
             OnCancelReplace( Msg );
             break;
           
        default: break;
    }      
}
//------------------------------------------------------------------------------
void GCRMConnection::UpdateExecID( const UFC::AnsiString& ExecID )
{
    FExecIDTable.Add( ExecID );
    UFC::MemoryStream Buffer( EXECID_LEN, EXECID_LEN );

    memset( (void*)Buffer.GetBuffer(), '\0', EXECID_LEN );
    ExecID.SaveToStream( &Buffer );
    FExecIDFile->Write( Buffer.GetBuffer(), EXECID_LEN );
    FExecIDFile->Flush();
}
//------------------------------------------------------------------------------
void GCRMConnection::LogExec( const char* Type ,TExecutionReportMessage* Msg, int Qty, double BeforeMargin, double ReleaseMargin, double CanUse )
{    
    if( KeepLog == FALSE )
        return;    
    GLog->fprint( " --- Order ",Type, " OrderID[", Msg->GetOrderID(), "] ---" );
    GLog->fprint( " Before[", UFC::IntToStr(Msg->GetBeforeQty()),"] After[", UFC::IntToStr(Msg->GetAfterQty()),")] ", Type, " Qty[", UFC::IntToStr( Qty), "] " );
    GLog->fprint( " Account[",Msg->GetAccount(),"] AE[", Msg->GetAE(),"] Symbol[",Msg->GetSymbol(),"] " );
    GLog->fprint( " Before margin[",UFC::DoubleToStr(BeforeMargin,5,2),"] Release margin[",UFC::DoubleToStr(ReleaseMargin,5,2),"]" );                
    GLog->fprint( " After margin[",UFC::DoubleToStr(CanUse,5,2), "] " );                
}
//------------------------------------------------------------------------------
void GCRMConnection::OnCancelReplace( TExecutionReportMessage* Msg )
{    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    double          CommodMargins = GetCommodDeposit( Symbol );
    AccountInfo*    Info;

    UseMargin( Account, AE, FALSE );
    if( (Info = GetAccountInfo( Account, AE )) != NULL )
    {
        nsOrderMessageDefine::ExecTypeEnum ExecType = Msg->GetExecType();

        if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
            CommodMargins = GetOptionsDeposit( Symbol, Msg->GetPrice(), (Msg->GetSide() == nsOrderMessageDefine::sBuy) );
        else
            CommodMargins = GetCommodDeposit( Symbol );        
        if( (ExecType == nsOrderMessageDefine::etPartiallyFilled ||  ExecType == nsOrderMessageDefine::etFilled) && FReleaseMarginOnFilled == TRUE )
        {
            if( !FExecIDTable.Exists( ExecID ) )
            {
                UpdateExecID( ExecID );
                Int32  Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
                double BeforeMargin = Info->GetCanUse();

                Info->FilledOrder( CommodMargins * Qty );
                LogExec( "Filled", Msg , Qty, BeforeMargin, CommodMargins * Qty, Info->GetCanUse() );
            }
        }
        else if( ExecType == nsOrderMessageDefine::etCanceled )
        {
            Int32  Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
            double BeforeMargin = Info->GetCanUse();

            if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifROD ) ///<  ROD
            {
                if( FReleaseMarginOnRODCxl == FALSE )
                    return;
            }
            else ///FOK,IOC
            {
                if( FReleaseMarginOnFOKIOCCxl == FALSE  )
                    return;
            }
            Info->CancelOrder( CommodMargins * Qty );
            LogExec( "Canceled", Msg , Qty, BeforeMargin, CommodMargins * Qty, Info->GetCanUse() );            
        }
        else if( ExecType == nsOrderMessageDefine::etReplaced && Msg->GetTMPExecType() == nsOrderMessageDefine::tetReplaced )
        {
            if( !FExecIDTable.Exists( ExecID ) )
            {
                UpdateExecID( ExecID );
                Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
                double BeforeMargin = Info->GetCanUse();

                Info->CancelOrder( CommodMargins * Qty );
                LogExec( "Replaced", Msg , Qty, BeforeMargin, CommodMargins * Qty, Info->GetCanUse() );            
            }
        }
        else if( ExecType == nsOrderMessageDefine::etRejected &&  Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew  )
        {
            Int32 Qty = Msg->GetOrderQty();
            Int32 ErrCode = UFC::AnsiString( Msg->GetStatusCode() ).ToInt();

            if( ErrCode != 97 ) ///< Not reject by RiskManager, need release margin.
            {               
                double BeforeMargin = Info->GetCanUse();
                
                Info->CancelOrder( CommodMargins * Qty );
                LogExec( "Rejected", Msg , Qty, BeforeMargin, CommodMargins * Qty, Info->GetCanUse() );            
            }
        }
    }
    else
        GLog->fprint( " Execution[", Msg->GetSrc(), "] Account[", Account, "] AE[", AE, "] Not exists!" );
        
}
//------------------------------------------------------------------------------

