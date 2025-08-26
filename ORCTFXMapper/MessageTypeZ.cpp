//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message QuoteCancel from counterparty
//
//  Transfer this FIX message to TAIFEX order message R090
//  and send to TAIFEX. (Options only)
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::QuoteCancel& QuoteCancel, const FIX::SessionID& sessionID )
{
	UFC::AnsiString        R090;
    OrderRecord*           OrigQuote;
	///< Required tags
	FIX::QuoteID					   QuoteID;   ///< Tag: 117
	///< Other tags
	FIX::SecurityID                    SecurityID;///< Tag: 48
	FIX::Symbol                        Symbol;    ///< Tag: 55
	FIX::NoQuoteEntries                noQuoteEntries;
	///< Fetch Required fields
    QuoteCancel.get( QuoteID );
	///< Fetch Other fields
	if( QuoteCancel.isSetField( noQuoteEntries ) )
    {
		QuoteCancel.get( noQuoteEntries );
        if( noQuoteEntries.getValue() >= 1 )
        {
			FIX44::QuoteCancel::NoQuoteEntries NoQuoteEntries;

            QuoteCancel.getGroup( 1, NoQuoteEntries );
            NoQuoteEntries.get( Symbol );
            NoQuoteEntries.get( SecurityID );
        }
    }
	if( (OrigQuote = FStore->GetOrder( QuoteID.getString().c_str() )) != NULL )
    {
        if( OrigQuote->GetState() == osCanceled ) ///< This already canceled.
            ReportQuoteStatus( FIX::QuoteID( QuoteID.getString().c_str() ), FIX::QuoteStatus( FIX::QuoteStatus_QUOTE_NOT_FOUND ),FIX::Text("Quote canceled") );
        else
        {
            ClientOrderID  NID = ClientOrderID::CreateClientOrderID( mkOptions, tkQuoteCancel );

            OrigQuote->CancelQuoteString( (Int32)NID, R090 );
            OrigQuote->SetLastClOrdID( QuoteID.getString().c_str() );
            FStore->AddRef( (Int32)NID, OrigQuote ); ///< This NID ref. to the original order record.
            FStore->UpdateOrder( OrigQuote );        ///< Sync to file.
            //FStore->AddClOrdID( (Int32)NID, new AnsiString( QuoteID.getString().c_str()));
            Application->SendOrder( (Int32)NID, R090, FIXMapper::WebID.c_str() );
        }
    }
	else ///< Quote not found! Send Quote back.
		ReportQuoteStatus( FIX::QuoteID( QuoteID.getString().c_str() ), FIX::QuoteStatus( FIX::QuoteStatus_QUOTE_NOT_FOUND ),FIX::Text("Quote not exists") );
}
//---------------------------------------------------------------------------
