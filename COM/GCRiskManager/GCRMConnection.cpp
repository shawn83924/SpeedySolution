#include "GCRMConnection.h"
#include "iniFile.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern BOOL            RecoverMode;
extern Int32           AS400Check;
extern UFC::AnsiString AppName;
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
        UFC::BufferedLog::Printf( " Reject! Margin not enough. Need[%0.2f] CanUse[%0.2f]", SubAmt, FCanUse);
        return FALSE;
    }
    else
    {
        UFC::BufferedLog::Printf( " Accept! Margin is enough.  Need[%0.2f] CanUse[%0.2f]", SubAmt, FCanUse);
        FCanUse -= SubAmt;
        UFC::BufferedLog::Printf( " leaves Margin[%0.2f]", FCanUse );
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
    UFC::AnsiString LogName, ExecIDFile;

    LogName.Printf( "../log/%s.%s.log", AppName.c_str(), UFC::GetDateString().c_str() );
    ExecIDFile.Printf( "ExecID.%s.DATA", UFC::GetDateString().c_str() );
    UFC::BufferedLog::Printf( " LogName:[%s]", LogName.c_str() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );
    
        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Dimerco Data System Risk control module " );    
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    if( FRejectMarketOrd == TRUE )
        UFC::BufferedLog::Printf( "   Market Order:Reject");
    else
        UFC::BufferedLog::Printf( "   Market Order:Accept");
    UFC::BufferedLog::Printf( "   AS400 To Speedy [%s][%s]", FToSpeedySubject.c_str(), FToSpeedyKey.c_str() );
    UFC::BufferedLog::Printf( "   AS400 Order Checker [%s][%s]", F400CheckOrderSubject.c_str(), F400CheckOrderKey.c_str() );    
    UFC::BufferedLog::Printf( "   AS400 Check Result  [%s][%s]", F400CheckResultSubject.c_str(), F400CheckResultKey.c_str() );        
    UFC::BufferedLog::Printf( "   Recover BeginTime:[%d]", FRecoverBeginTime );    
    UFC::BufferedLog::Printf( "                       " );
    UFC::BufferedLog::Printf( "   Release Margin on Filled[%s]", (FReleaseMarginOnFilled == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "   Release Margin on ROD Canceled[%s]", (FReleaseMarginOnRODCxl == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "   Release Margin on FOK,IOC Canceled[%s]", (FReleaseMarginOnFOKIOCCxl == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "                       " );    
    switch( AS400Check )
    {
        case 0:  UFC::BufferedLog::Printf( "   Enable AS400 Checker[No]" );break;    
        case 1:  UFC::BufferedLog::Printf( "   Enable AS400 Checker[Yes]" );break;    
        case 2:  UFC::BufferedLog::Printf( "   Always use AS400 Checker" );break;          
        default: UFC::BufferedLog::Printf( "   Unknown AS400Check[%d]", AS400Check );
                 exit( 0 );
                 break;          
    }
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    
    FMessageObject = new MessageObject( AppName, "1.0.0", "Dimerco RM connection." );
    FMessageObject->AddListener( FFutOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FOptOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FToSpeedySubject, FToSpeedyKey, this );
    if( AS400Check > 0 )
        FMessageObject->AddListener( F400CheckResultSubject, F400CheckResultKey, this );
    
    UFC::AnsiString FileName;

    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    FConfig = new UFC::UiniFile( FileName );
    FileName.Printf( "../cfg/AS400ErrorMessage.cfg" );
    FErrMsg = new UFC::UiniFile( FileName ); 
    if( RecoverMode == FALSE )
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "w" );
    }
    else
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "a+" );
        RecoverExecID();
    }
    ///< Create the connection object.
    FConnection = new TTaifexConnection( AppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    ///< Start running thread.
    FMessageObject->Start();
    FMessageObject->WaitForConnected();
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
        UFC::BufferedLog::DebugPrintf( " ExecID:[%s]", ExecID.c_str() );
        FExecIDTable.Add( ExecID );
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::LoadSetting( void )
{
    UFC::AnsiString FileName,Name,Value,DayName;
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
        UFC::BufferedLog::Printf( " Default Futures deposit[%0.2f]", FDefFutDeposit );
        UFC::BufferedLog::Printf( " Default Options A value[%0.2f]", FDefOptAValue );
        UFC::BufferedLog::Printf( " Default Options Contract Multiplier[%0.2f]", FDefOptCM );

        FFutDeposit.Clear();
        if( (IniSection = Config.GetSection( "Futures")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue(i,Name,Value) && Name != "Default" )
                {
                    double Deposit = Value.ToDouble();
                    FFutDeposit.Add(Name,Deposit);
                    UFC::BufferedLog::Printf( " Futures deposit[%s][%0.2f]", Name.c_str(), Deposit );
                }
            }
        }
        if( (IniSection = Config.GetSection( "FuturesDaytrade")) != NULL )
        {
            for(int i=0;i<IniSection->ItemCount();i++)
            {
                if( IniSection->GetNameValue( i , Name, Value) && Name != "Default" )
                {
                    double Deposit = Value.ToDouble();
                    
                    DayName.Printf( "%sD", Name.c_str() );
                    FFutDeposit.Add( DayName, Deposit);
                    UFC::BufferedLog::Printf( " Futures Day-trade deposit[%s][%0.2f]", Name.c_str(), Deposit );
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
                    UFC::BufferedLog::Printf( " Options A Value[%s][%0.2f]", Name.c_str(), AVal );
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
                    UFC::BufferedLog::Printf( " Options Contract Multiplier[%s][%0.2f]", Name.c_str(), CM );
                }
            }
        }
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf(" Exception[%s].", e.what() );
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
double GCRMConnection::GetFuturesDeposit( const UFC::AnsiString& SymbolPrefix, bool IsDayTrade)
{
    double           Value;
    UFC::PLockObject Lock( FCommodCS );

    if( IsDayTrade == true )
    {
        UFC::AnsiString DayTradePrefix;
        
        DayTradePrefix.Printf( "%sD", SymbolPrefix.c_str() );
        if( FFutDeposit.GetObjectByKey( DayTradePrefix ,Value) == true )
        {
            UFC::BufferedLog::Printf( " [%s] Day-trade margin[%0.2f]", SymbolPrefix.c_str(), Value );
            return Value;
        }
        else if( FFutDeposit.GetObjectByKey( SymbolPrefix ,Value) == true )
            return Value;
    }
    else
    {
        if( FFutDeposit.GetObjectByKey( SymbolPrefix ,Value) == true )
            return Value;
    }
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
double GCRMConnection::GetCommodDeposit( const UFC::AnsiString& Symbol, bool IsDayTrade )
{
    UFC::AnsiString  SymbolPrefix( Symbol.c_str(), 3 );    
    return GetFuturesDeposit( SymbolPrefix, IsDayTrade );
}
//------------------------------------------------------------------------------
double GCRMConnection::GetOptionsDeposit( const UFC::AnsiString& Symbol, double Px, bool IsBuy )
{
    UFC::AnsiString  Sym = Symbol;
    UFC::AnsiString  SymbolPrefix( Symbol.c_str(), 3 );    

    if( Sym.Length() > 10 )///< Multileg order(Same MXF)
    {
        return GetFuturesDeposit( "MXF", false );
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
bool GCRMConnection::GetAEFromData( const UFC::AnsiString& Data, UFC::AnsiString& AE )
{
    UFC::NameValueMessage NV("^");
        
    NV.FromString( Data  );        
    return NV.Get( "Sub", AE );
}
//---------------------------------------------------------------------------
void GCRMConnection::GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = Key.AnsiPos(',');
    
    if( SplitPos !=  -1 ) ///< AE,Data
        AE = Key.SubString( 0, SplitPos );
    else ///< Only AE no user data
        AE = Key;
}
//---------------------------------------------------------------------------
void GCRMConnection::GetSubAccount( const UFC::AnsiString& Key, UFC::AnsiString& Sub )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = Key.AnsiPos(',');
    
    if( SplitPos !=  -1 ) ///< AE,Data
    {
        UFC::AnsiString GIGO( Key.c_str() + SplitPos + 1 );
        
        if( GetAEFromData( GIGO, Sub ) == TRUE )
            Sub.PadThis( 6, ' ' );
        else
            Sub = "      ";
    }
    else ///< Only AE no user data
        Sub = "      ";
}
//------------------------------------------------------------------------------
void GCRMConnection::UseMargin( const UFC::AnsiString& Account, UFC::AnsiString& AE )
{
    UFC::AnsiString Value;
    int  MarginLevel = 3; ///< Default: AE Margin

    if( FConfig->GetValue( Account, AE, Value ) == true )
        MarginLevel = Value.ToInt();
    if( MarginLevel == 1 )
    {
        AE = "      ";
        UFC::BufferedLog::Printf( " Use Company margin. Account[%s] AE[%s] ", Account.c_str(),AE.c_str() );
    }
    else if( MarginLevel == 2 )
    {
        AE.SetLength( 3 );
        AE.PadThis( 6,' ');
        UFC::BufferedLog::Printf( " Use Group margin. Account[%s] AE[%s] ", Account.c_str(),AE.c_str() );
    }
    else
        UFC::BufferedLog::Printf( " Use AE margin. Account[%s] AE[%s] ", Account.c_str(),AE.c_str() );
}
//------------------------------------------------------------------------------
void GCRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    if( Subject == FFutOrderSubject|| Subject == FOptOrderSubject )
    {
        UFC::AnsiString Key, Order, AE;
        Int32 NID;        
        Int32 IsOffHour = FALSE;        
        
        if( Data->get( "ORDER", Order ) == TRUE &&  Data->get( "NID", NID ) == TRUE && Data->get( "KEY", Key ) == TRUE )
        {
            Data->get( "OFFHOUR", IsOffHour );
            UFC::BufferedLog::Printf( " Recv from Speedy Gateway:[%s]", Order.c_str() );

            UFC::AnsiString BrokerID( Order.c_str() + 14, 7 );
            UFC::AnsiString Account( Order.c_str() + 26, 7 );
            UFC::AnsiString Symbol( Order.c_str() + 34, 20 );
            UFC::AnsiString Price( Order.c_str() + 56, 9 );
            Int32           Qty        = Order.SubString( 65, 4 ).ToInt();
            char            OpenOffset = Order[70];
            char            Side       = Order[54];
            char            OrderType  = Order[55];
            bool            IsDayTrade = false;
            
            if( OpenOffset == '2' )
                IsDayTrade = true;
            GetAE( Key, AE );
            Symbol.TrimRight();
            UFC::BufferedLog::Printf( " OrderQty[%d] Price[%s] OffHour[%d]", Qty, Price.c_str(),IsOffHour );
            UFC::BufferedLog::Printf( " BrokerID[%s] Account[%s] AE[%s] Symbol[%s] OpenOffset[%c]", BrokerID.c_str(),  Account.c_str(),AE.c_str(), Symbol.c_str(), OpenOffset );
            UseMargin( Account, AE );
            AccountInfo* Info = GetAccountInfo( Account, AE );
            if( IsReplacePx( Order ) ) ///< Skip Replace PX
            {
                Data->append( "CODE", 1 );
                if( Subject == FFutOrderSubject )
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                else
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                return;
            }
            if( AS400Check == 2 )///< Send to AS400 direct
            {
                if( Subject == FFutOrderSubject )
                {
                    SendToAS400Checker( FALSE, NID, Order, Key, IsOffHour );
                    return;
                }
                else
                {
                    SendToAS400Checker( TRUE, NID, Order, Key, IsOffHour );
                    return;
                }
            }    
            if( Info == NULL  ) ///< Account information not exosts.
            {
                UFC::BufferedLog::Printf( " Can't fetch the record: [%s,%s]", Account.c_str(), AE.c_str() );
                UFC::BufferedLog::DebugPrintf( " Speedy RM Reject Order:[%s]", Order.c_str() );
                Data->append( "CODE", 0 );
                Data->append( "MSG", "Account not exists." );
                if( Subject == FFutOrderSubject )
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                else
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                return;
            }
            if( FRejectMarketOrd == TRUE && OrderType == 'M') ///< Reject Market Order.
            {
                UFC::BufferedLog::DebugPrintf( " Speedy RM Reject Order:[%s]", Order.c_str() );
                Data->append( "CODE", 0 );
                Data->append( "MSG", "Can't place market order." );
                if( Subject == FFutOrderSubject )
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                else
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                return;
            }            
            ///< Check Margin.
            if( Subject == FFutOrderSubject )
            {
                double CommodMargins = GetCommodDeposit( Symbol, IsDayTrade );

                if( Info->CanPlaceOrder( CommodMargins * Qty ) )
                {
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                }
                else
                {
                    if( AS400Check == 0 )
                    {
                        Data->append( "CODE", 0 );
                        Data->append( "MSG", "Exceed Margin." );
                        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                    }
                    else
                        SendToAS400Checker( FALSE, NID, Order, Key, IsOffHour );
                }
            }
            else if( Subject == FOptOrderSubject )
            {
                int    Precision     = FConnection->GetPricePrecision( nsOrderMessageDefine::mTWOptions, nsOrderMessageDefine::tsNormal, Symbol );
                double OrderPx       =  UFC::IntToDouble( Price.ToInt(), Precision );
                double CommodMargins = GetOptionsDeposit( Symbol, OrderPx, (Side=='B') );
                
                if( Info->CanPlaceOrder( CommodMargins * Qty ) )
                {
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                }
                else
                {
                    if( AS400Check == 0 )
                    {
                        Data->append( "CODE", 0 );
                        Data->append( "MSG", "Exceed Margin." );
                        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                    }
                    else
                        SendToAS400Checker( TRUE, NID, Order, Key, IsOffHour );
                }
            }
        }
        else
            UFC::BufferedLog::Printf( " Missing ORDER KEY or NID field." );

    }
    else if( Subject == F400CheckResultSubject ) ///< Recv check result
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

        UFC::BufferedLog::Printf( " Account[%s] AE[%s] Sign[%c] CanUse[%d.%02d]", Account.c_str(), AE.c_str(),Sign,IntVal, FractionVal);
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
    BOOL IsOpt = FALSE;
   
    if( Data->get( "Data", Result ) && Result.Length() >= 512 )
    {    
        UFC::AnsiString Order      = Result.SubString( 0, 75 );
        UFC::AnsiString Key        = Result.SubString( 145, 259 );
        char            TS         = Result[ 404 ];
        UFC::AnsiString Status     = Result.SubString( 405, 2 );        
        char            OCCode     = Result[ 407 ];
        UFC::AnsiString ECode      = Result.SubString( 408, 7 );
        char            OrigOCCode = Order[ 70 ];        
        int             NID        = Result.SubString( 134, 11 ).ToInt();
        MTree           Tree;        
        
        Order[ 70 ] = OCCode; 
        if( Order[0] == '9' && Order[1] == '3' )
        {
            IsOpt = TRUE;  
            Order[0] = '3';
            Order[1] = '0';
        }
        Key.TrimRight();
        ECode.TrimRight();
        UFC::BufferedLog::Printf( " AS400 Result[%s]", Result.c_str());
        UFC::BufferedLog::Printf( " Position Effect Original[%c] Now[%c]", OrigOCCode, OCCode );
        UFC::BufferedLog::Printf( " Order from AS400[%s]", Order.c_str());        
        UFC::BufferedLog::Printf( " Status[%s] Key[%s] TradingSession[%c]", Status.c_str(), Key.c_str(), TS );                        
        if( TS == 'P' )
            Tree.append( "OFFHOUR", (Int32) 1 );
        else
            Tree.append( "OFFHOUR", (Int32) 0 );
        Tree.append( "ORDER", Order );
        Tree.append( "NID", NID );
        Tree.append( "KEY", Key );                
        if( Status == "00")        
        {
            Tree.append( "CODE", 1 );            
            UFC::BufferedLog::Printf( " Pass AS400 Check." );                            
        }    
        else
        {
            UFC::AnsiString Msg;
            
            if( FErrMsg->GetValue( "AS400ErrorCode", ECode, Msg ) == FALSE )
                Msg.Printf( "Reject by AS400 Code[%s]", ECode.c_str() );
            Tree.append( "CODE", 0 );
            Tree.append( "MSG", Msg );
            UFC::BufferedLog::Printf( " AS400 Reject[%s]",  Msg.c_str() );                            
        }
        if( IsOpt == TRUE )
            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, Tree );  
        else
            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, Tree );                              
    }
    else
        UFC::BufferedLog::Printf( " Check result from AS400 miss value [Data].");                            
        
}
//-------------------------------------------------------------------------------
void GCRMConnection::SendToAS400Checker( BOOL IsOpt, int NID, const UFC::AnsiString& Order, const UFC::AnsiString& Key, BOOL IsOffHour )
{
    MTree Data;
    char  TS;
    UFC::AnsiString  AE,AS400Order,Sys,Space,NIDKey;    
    
    GetAE( Key, AE ); //< AE(6)       
    AE.PadThis( 6, ' ');
    Space.PadThis( 41,' ');   //< WEBIP(25) + CA(1) +LogonID(10) +GroupID(5)
    NIDKey.Printf( "%011d%s", NID, Key.c_str() );
    NIDKey.PadThis( 270,' '); //< Key(270) 
    if( IsOpt == FALSE )
        Sys = "30";
    else
        Sys = "93";
    if( IsOffHour == TRUE )
        TS = 'P';
    else
        TS = 'R';
    AS400Order.Printf( "%s%s%s%s%08d%c%s%s%c", 
                        Sys.c_str(),
                        Order.c_str() + 2, 
                        AE.c_str(), 
                        FAS400WebID.c_str(),
                        FAS400OrderSeq,
                        FAS400Src,
                        Space.c_str(),
                        NIDKey.c_str(),
                        TS );    
    AS400Order.PadThis( 512, ' ' );
    FAS400OrderSeq++;
    if( IsOffHour == TRUE )
        UFC::BufferedLog::Printf( " OffHour Order to AS400[%s]", AS400Order.c_str() );
    else
        UFC::BufferedLog::Printf( " Normal  Order to AS400[%s]", AS400Order.c_str() );
    Data.append( "Data", AS400Order );                
    FMessageObject->Send( F400CheckOrderSubject, F400CheckOrderKey, Data );    
}
//-------------------------------------------------------------------------------
AccountInfo* GCRMConnection::GetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE )
{
    UFC::AnsiString Key;

    Key.Printf( "%s%s", Account.c_str(), AE.c_str() );
    return AccountInfoTable.GetObjectByKey( Key );
}
//-------------------------------------------------------------------------------
void  GCRMConnection::SetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, double CanUse )
{
    AccountInfo*    AccInfo;
    UFC::AnsiString Key;

    Key.Printf( "%s%s", Account.c_str(), AE.c_str() );
    if( (AccInfo = AccountInfoTable.GetObjectByKey( Key )) == NULL )
    {
         AccInfo = new AccountInfo( Account, AE );
         AccountInfoTable.Add( Key, AccInfo );
    }
    AccInfo->SetCanUse( CanUse );
}
//-------------------------------------------------------------------------------    
void GCRMConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " login( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void GCRMConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void GCRMConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {        
        FIsLogon = TRUE;
        UFC::BufferedLog::Printf( " %s( CID=%d )...login ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s...login failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::Recover( const UFC::AnsiString& BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void GCRMConnection::OnRecoverFinished( int Count )
{
    FIsLogon = TRUE;
    UFC::BufferedLog::DebugPrintf( " SinoPacExecConnection::OnRecoverFinished( Count = %d )", Count );    
    UFC::SleepMS( 1000 );   
    SetAccountInfo( "9903758",  "yuan", 10000000 );
}    
//------------------------------------------------------------------------------
void GCRMConnection::CheckConnection( void )
{
    if( FIsLogon == FALSE )
        FConnection->Connect( FGatewayIP.c_str() , FGatewayPort, 3 );    
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
             {
                UFC::BufferedLog::DebugPrintf( " Replace Px. Drop this execution report." );
                return;
             }
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
void GCRMConnection::OnCancelReplace( TExecutionReportMessage* Msg )
{    
    UFC::AnsiString ExecID( Msg->GetExecID() );    
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    UFC::AnsiString AE;
    bool            IsDayTrade = (Msg->GetPositionEffect() == nsOrderMessageDefine::peRolled);    
    double          CommodMargins = GetCommodDeposit( Symbol, IsDayTrade );
    AccountInfo*    Info;
    
    if( GetAEFromData( Msg->GetData(), AE ) == FALSE )
        AE = Msg->GetAE();
    UseMargin( Account, AE );
    if( (Info = GetAccountInfo( Account, AE )) != NULL )
    {
        nsOrderMessageDefine::ExecTypeEnum ExecType = Msg->GetExecType();

        if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
            CommodMargins = GetOptionsDeposit( Symbol, Msg->GetPrice(), (Msg->GetSide() == nsOrderMessageDefine::sBuy) );
        else
            CommodMargins = GetCommodDeposit( Symbol, IsDayTrade );
        if( (ExecType == nsOrderMessageDefine::etPartiallyFilled ||  ExecType == nsOrderMessageDefine::etFilled) && FReleaseMarginOnFilled == TRUE )
        {
            if( !FExecIDTable.Exists( ExecID ) )
            {
                UpdateExecID( ExecID );
                Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();

                Info->CancelOrder( CommodMargins * Qty );
                UFC::BufferedLog::Printf( " --- Order Filled OrderID[%s] ---", Msg->GetOrderID() );
                UFC::BufferedLog::Printf( " Before[%d] After[%d] Filled Qty[%d]", Msg->GetBeforeQty(), Msg->GetAfterQty(), Qty );
                UFC::BufferedLog::Printf( " Account:[%s] AE[%s] Symbol:[%s] ", Account.c_str(), AE.c_str(), Symbol.c_str() );
                UFC::BufferedLog::Printf( " Before margin[%lf] Release margin[%lf]", Info->GetCanUse(), CommodMargins * Qty );
                Info->CancelOrder( CommodMargins * Qty );
                UFC::BufferedLog::Printf( " After margin[%lf] ", Info->GetCanUse() );
            }
        }
        else if( ExecType == nsOrderMessageDefine::etCanceled )
        {
            Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();

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
            UFC::BufferedLog::Printf( " --- Order Canceled OrderID[%s] ---", Msg->GetOrderID() );
            UFC::BufferedLog::Printf( " Before[%d] After[%d] Cancel Qty[%d]", Msg->GetBeforeQty(), Msg->GetAfterQty(), Qty );
            UFC::BufferedLog::Printf( " Account:[%s] AE[%s] Symbol:[%s] ", Account.c_str(), AE.c_str(), Symbol.c_str() );
            UFC::BufferedLog::Printf( " Before margin[%lf] Release margin[%lf]", Info->GetCanUse(), CommodMargins * Qty );
            Info->CancelOrder( CommodMargins * Qty );
            UFC::BufferedLog::Printf( " After margin[%lf] ", Info->GetCanUse() );
        }
        else if( ExecType == nsOrderMessageDefine::etReplaced && Msg->GetTMPExecType() == nsOrderMessageDefine::tetReplaced )
        {
            if( !FExecIDTable.Exists( ExecID ) )
            {
                UpdateExecID( ExecID );
                Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();

                UFC::BufferedLog::Printf( " --- Order Replaced OrderID[%s] ---", Msg->GetOrderID() );
                UFC::BufferedLog::Printf( " Before[%d] After[%d] Reduce Qty[%d]", Msg->GetBeforeQty(), Msg->GetAfterQty(), Qty );
                UFC::BufferedLog::Printf( " Account:[%s] AE[%s] Symbol:[%s] ", Account.c_str(), AE.c_str(), Symbol.c_str() );
                UFC::BufferedLog::Printf( " Before margin[%lf] Release margin[%lf]", Info->GetCanUse(), CommodMargins * Qty );
                Info->CancelOrder( CommodMargins * Qty );
                UFC::BufferedLog::Printf( " After margin[%lf] ", Info->GetCanUse() );
            }
        }
        else if( ExecType == nsOrderMessageDefine::etRejected && 
                 Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew  )
        {
            Int32 Qty = Msg->GetOrderQty();
            Int32 ErrCode = UFC::AnsiString( Msg->GetStatusCode() ).ToInt();

            if( ErrCode != 97 ) ///< Not reject by RiskManager, need release margin.
            {
                UFC::BufferedLog::Printf( " --- Order Rejected OrderID[%s] Qty[%d] ---", Msg->GetOrderID(), Qty );
                UFC::BufferedLog::Printf( " Account:[%s] AE[%s] Symbol:[%s] ", Account.c_str(), AE.c_str(), Symbol.c_str() );
                UFC::BufferedLog::Printf( " Before margin[%lf] Release margin[%lf]", Info->GetCanUse(), CommodMargins * Qty );
                Info->CancelOrder( CommodMargins * Qty );
                UFC::BufferedLog::Printf( " After margin[%lf] ", Info->GetCanUse() );
            }
        }
    }
}
//------------------------------------------------------------------------------

