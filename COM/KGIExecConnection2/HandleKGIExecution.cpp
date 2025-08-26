#include "GCExecConnection.h"
extern BOOL            DumpOnly;
extern MessageObject*  MessageObj;
//------------------------------------------------------------------------------
void GCExecConnection::KGIConfirmToBackend( TExecutionReportMessage* Msg, BOOL IsQuote )
{
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFX,Backend, SSNID;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;

    char QueueData[ 513 ];     
    ///< TAIFEX part.
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "%s30%s", "        ", Msg->GetSrc()+2 );
    else
        TFX.Printf( "%s93%s", "        ", Msg->GetSrc()+2 );
    ///< SSNID
    SSNID.Printf( "%05d", atoi( Msg->GetPVC() ) ) ;
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Backend part.
    if( GIGO.AnsiPos( "ClOrdID=") == 0 )///< From Orc
        GIGO = AE;        
    else ///< From API
    {
        if( GIGO.Length() >= 6 )
            AE = GIGO.SubString( 0,6 );
        else
            AE = GIGO;
    }  
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
    Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(),AE.c_str());
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    memcpy( QueueData + 424, SSNID.c_str(), 5 );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());        
    if( IsQuote == TRUE )
    {
        UFC::AnsiString QKey;
        
        if( Market == nsOrderMessageDefine::mTWFutures )
            QKey.Printf( "FUT_%s", Msg->GetOrderID() );
        else
            QKey.Printf( "OPT_%s", Msg->GetOrderID() );
        if( Msg->GetSide() == nsOrderMessageDefine::sBuy )
        {                
            UFC::AnsiString Px( QueueData + 63, 9 );
            UFC::AnsiString Qty( QueueData + 80, 4 );
            
            FQuoteTable.Add( QKey, new QuoteInfo( Px,Qty ) );
            UFC::BufferedLog::Printf( " Skip Quote[%s] Buy Side Px[%s] Qty[%s]", QKey.c_str(), Px.c_str(), Qty.c_str() );
            return;
        }
        else
        {
            QuoteInfo* BuyInfo = FQuoteTable.GetObjectByKey( QKey );
            if( BuyInfo != NULL )
            {
                memcpy( QueueData + 63, BuyInfo->Price.c_str(), 9);
                memcpy( QueueData + 80, BuyInfo->Qty.c_str(), 4);            
                FQuoteTable.DeleteByKey( QKey );
                delete BuyInfo;
            }
            else
            {
                UFC::BufferedLog::Printf( " Quote[%s] Buy Side Price, Qty not found!", QKey.c_str() );
                return;
            }
        }
    }
    Data.append("Data", QueueData );
    UFC::BufferedLog::Printf( " Px=%5.3f", Msg->GetPrice() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT CONFIRM> <%s>", QueueData );
            if( DumpOnly == FALSE )
                MessageObj->Send( FConfirmSubject, FConfirmKey, Data );            
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <%s>", QueueData );                                                            
            if( DumpOnly == FALSE )
                MessageObj->Send( FConfirmSubject, FConfirmKey, Data );
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void GCExecConnection::KGIFillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFX, Backend, TimeNow, MatchTime, SSNID;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char QueueData[ 513 ];     

    if( Src[0] == '1' )
        MatchTime = Src.SubString( 61, 8 );
    else if( Src[0] == '2' )
        MatchTime = Src.SubString( 109 , 8 );
    else
        MatchTime = Src.SubString( 57, 8 );

    UFC::BufferedLog::DebugPrintf( " MatchTime:[%s]", MatchTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", Msg->GetBrokerID() );
    ///< TAIFEX part.
    //UFC::GetHHMMSS( TimeNow );
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "        500002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    else
        TFX.Printf( "        950002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    ///< Backend part.1    
    if( GIGO.AnsiPos( "ClOrdID=") == 0 )///< From Orc
        GIGO = AE;        
    else ///< From API
    {
        if( GIGO.Length() >= 6 )
            AE = GIGO.SubString( 0,6 );
        else
            AE = GIGO;
    }      
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
    Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(), AE.c_str());
    char TIF;
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        TIF = 'I';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        TIF = 'F';
    else
        TIF = 'R';

    SSNID.Printf( "%05d%c", atoi( Msg->GetPVC()), TIF );
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    memcpy( QueueData + 424, SSNID.c_str(), 6 );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());        
    Data.append("Data", QueueData );
    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", QueueData );                                                
            if( DumpOnly == FALSE )
                MessageObj->Send( FFillSubject, FFillKey, Data );
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", QueueData );
            if( DumpOnly == FALSE )
                MessageObj->Send( FFillSubject, FFillKey, Data );
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
