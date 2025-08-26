//---------------------------------------------------------------------------
#if !defined(TSCTFXOrder_H)
#define TSCTFXOrder_H
//------------------------------------------------------------------------------
#include "MapperApp.h"
#include "imFIXMapper.h"
#include "OrderStore.h"
#include "NetworkID.h"
#include "SymbolUtility.h"
#include "../UFC/iniFile.h"
//------------------------------------------------------------------------------
class AccountAttribute
{
private:
    UFC::AnsiString   Account;
    UFC::AnsiString   FBrokerID;
    UFC::AnsiString   FWebID;
public:
    AccountAttribute( UFC::AnsiString, UFC::AnsiString, UFC::AnsiString );
public:
    UFC::AnsiString   GetAccount( void ) { return Account; }
    UFC::AnsiString   GetBrokerID( void ) { return FBrokerID; }
    UFC::AnsiString   GetAccountWebID( void ) { return FWebID; }
};
//-----------------------------------------------------------------------------


class TSCTFXOrderRecord : public OrderRecord
{
 private:
	UFC::AnsiString  FAccount;
    UFC::AnsiString  FSecurityID;
    UFC::AnsiString  FSubAccount;

 public:
	UFC::AnsiString& GetAccount( void )         { return FAccount; }
	UFC::AnsiString& GetSecurityID( void )	    { return FSecurityID; }
 public:
    ///< For Order
    TSCTFXOrderRecord(  ClientOrderID&                      CNID,
                        UFC::AnsiString&                    OrderID,
                        FIX::ClOrdID&                       ClOrdID,
						FIX::Account&                       Account,
                        UFC::AnsiString&                    SubAccount,
						FIX::SecurityID&                    SecurityID,
                        FIX::Symbol&                        Symbol,
                        FIX::OrderQty&                      OrderQty,
                        FIX::Price&                         Price,
                        FIX::Side&                          Side,
                        FIX::TimeInForce&                   TimeInForce,
                        UFC::AnsiString&                    Sender);
	///< For Quote
	TSCTFXOrderRecord( ClientOrderID&                      CNID,
					   UFC::AnsiString&                    OrderID,
					   FIX::QuoteID&                       QuoteID,
					   FIX::Account&                       Account,
                       UFC::AnsiString&                    SubAccount,
					   FIX::SecurityID&                    SecurityID,
					   FIX::Symbol&                        Symbol,
					   FIX::BidPx&                         BidPx,
					   FIX::BidSize&                       BidSize,
					   FIX::OfferPx&                       OfferPx,
					   FIX::OfferSize&                     OfferSize,
					   UFC::AnsiString&                    Sender);
	TSCTFXOrderRecord():OrderRecord(){}
private:
	UFC::AnsiString FTradeDate;
	char GetOrderCondition( void );
public:
	///< For Object persistence.
	virtual void SaveToBuffer( UFC::MemoryStream& Stream );
	virtual void LoadFromBuffer( UFC::MemoryStream& Stream );
	///< For Order
    virtual void NewOrderString( UFC::AnsiString& ){}
    virtual void CancelString( Int32 NID, UFC::AnsiString& ){}
    virtual void ReduceString( Int32 NID, Int32 NewQty, UFC::AnsiString& ){}
    void NewOrderString( UFC::AnsiString&, AccountAttribute* );
    void CancelString( Int32 NID, UFC::AnsiString&, AccountAttribute* );
    void ReduceString( Int32 NID, Int32 NewQty, UFC::AnsiString&, AccountAttribute* );

    ///< For Quote
	virtual void NewQuoteString(  UFC::AnsiString&  );
	virtual void CancelQuoteString(  Int32 NID, UFC::AnsiString&  );
    ///< For FIX
    virtual void FillPendingNew( FIX44::ExecutionReport& report );
};
//------------------------------------------------------------------------------
class TSCTFXOrderFactory : public OrderFactory
{
	virtual OrderRecord* CreateOrder( ) { return new TSCTFXOrderRecord( ); }
};
//------------------------------------------------------------------------------
class TSCTFXExecutionParser : public ExecutionParser
{
public:
    virtual BOOL Parse( MarketKind Market, const UFC::AnsiString& DataString );
};
//------------------------------------------------------------------------------
class TSCTFXConfirmParser : public ConfirmParser
{
public:
    virtual void Parse( MarketKind Market, const UFC::AnsiString& DataString );
};
//------------------------------------------------------------------------------
#endif
