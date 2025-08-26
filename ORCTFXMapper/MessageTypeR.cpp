//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message QuoteCancel from counterparty
//
//  Transfer this FIX message to TAIFEX order message R090
//  and send to TAIFEX. (Options only)
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::QuoteRequest& QuoteRequest, const FIX::SessionID& sessionID )
{
    FIX44::QuoteRequest::NoRelatedSym NoRelatedSym;
    FIX::SecurityID                   SecurityID;///< Tag: 48
    FIX::Symbol                       Symbol;    ///< Tag: 55
    FIX::QuoteReqID                   QuoteReqID;
    FIX::CFICode                      CFICode;
    UFC::AnsiString                   R070,TradeDate,TFXSecurityID;
    ClientOrderID                     NID = ClientOrderID::CreateClientOrderID( mkOptions, tkQuoteRequest );

    GetYYYYMMDD( TradeDate );
    QuoteRequest.get( QuoteReqID );
    QuoteRequest.getGroup( 1, NoRelatedSym );
      NoRelatedSym.get( Symbol );
      NoRelatedSym.get( SecurityID );
      NoRelatedSym.get( CFICode );
    TFXSecurityID = SecurityID.getString().c_str();
    TFXSecurityID.PadThis( 20, ' ');
    FStore->AddQuoteRequest( (Int32)NID, new QuoteRequestRecord(QuoteReqID, Symbol, SecurityID, CFICode) );
    R070.Printf("31%s%08d%s%sTT2F%s", FIXMapper::AE.c_str(),(Int32)NID,FIXMapper::BrokerID.c_str(),TFXSecurityID.c_str(),TradeDate.c_str());
    Application->SendOrder( (Int32)NID, R070, FIXMapper::WebID.c_str() );
}
//---------------------------------------------------------------------------
