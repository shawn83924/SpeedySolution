//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
void OrcTFXMapper::ReportQuoteStatus( FIX::QuoteID QuoteID, FIX::QuoteStatus Status,FIX::Text Text )
{
	FIX44::QuoteStatusReport report( QuoteID );

	report.set( Status );
    report.set( Text );
	Application->SendToimFIX( FMsgSource, report );
}
//---------------------------------------------------------------------------
//  Receive FIX message Quote from counterparty
//
//  Transfer this FIX message to TAIFEX order message R090
//  and send to TAIFEX. (Options only)
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::Quote& Quote, const FIX::SessionID& sessionID )
{
	///< Required tags
	FIX::Symbol                        Symbol;    ///< Tag: 55
	FIX::QuoteID					   QuoteID;   ///< Tag: 117
    FIX::QuoteReqID                    QuoteReqID;
	///< Other tags
	FIX::BidPx						   BidPx( 0 );     ///< Tag: 132
	FIX::OfferPx					   OfferPx( 0 );   ///< Tag: 133
	FIX::BidSize					   BidSize( 0 );   ///< Tag: 134
	FIX::OfferSize					   OfferSize( 0 ); ///< Tag: 135
	FIX::SecurityID                    SecurityID;     ///< Tag: 48
	FIX::Account                       Account( "8888888" );   ///< Tag: 1

	UFC::AnsiString                    R090;
	UFC::AnsiString                    OIDStr = "00000";
	UFC::AnsiString                    AE = FIXMapper::AE;
	OrderRecord*                       NewQuote;
    ClientOrderID                      NID;
	///< Fetch Required fields
    Quote.get( Symbol );
    Quote.get( QuoteID );
	///< Fetch Other fields
	if( Quote.isSetField( BidPx ) )
		Quote.get( BidPx );
    else
        PrintFieldNotfond( "Quote", "BidPx");
    if( Quote.isSetField( OfferPx ) )
		Quote.get( OfferPx );
    else
        PrintFieldNotfond( "Quote", "OfferPx");
	if( Quote.isSetField( BidSize ) )
		Quote.get( BidSize );
    else
        PrintFieldNotfond( "Quote", "BidSize");
	if( Quote.isSetField( OfferSize ) )
		Quote.get( OfferSize );
    else
        PrintFieldNotfond( "Quote", "OfferSize");
	if( Quote.isSetField( SecurityID ) )
		Quote.get( SecurityID );
    else
        PrintFieldNotfond( "Quote", "SecurityID");
    if( Quote.isSetField( 14001 ) )
        AE = Quote.getField( 14001 ).c_str();
    if( Application->IsMapperGenOID() == TRUE )
    {
       if(  OrderIDGenerator::GenerateOrderID( OIDStr ) == FALSE )
       {
      		ReportQuoteStatus( QuoteID,FIX::QuoteStatus( FIX::QuoteStatus_REJECTED ),FIX::Text("Out of OrderID.") );
            return;
       }
    }
    NID = ClientOrderID::CreateClientOrderID( mkOptions, tkNewQuote );
	///< Create a quote object.
	NewQuote = new TSCTFXOrderRecord( NID,OIDStr,QuoteID,Account,AE,SecurityID,Symbol,BidPx,BidSize,OfferPx,OfferSize,FMsgSource);
    if( Quote.isSetField( QuoteReqID ) )
        NewQuote->SetQuoteResponding( TRUE );
	FStore->AddOrder( (Int32)NID, NewQuote );
    NewQuote->NewQuoteString( R090 );
    if( Application->SendOrder( (Int32)NID, R090, FIXMapper::WebID.c_str() ) == FALSE )
		ReportQuoteStatus( QuoteID,FIX::QuoteStatus( FIX::QuoteStatus_REJECTED ),FIX::Text("9098 MBus service not available") );
}
//---------------------------------------------------------------------------
