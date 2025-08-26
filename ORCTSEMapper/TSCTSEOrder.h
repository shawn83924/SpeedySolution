//---------------------------------------------------------------------------
#if !defined(TSCTSEOrder_H)
#define TSCTSEOrder_H
//------------------------------------------------------------------------------
#include "MapperApp.h"
#include "imFIXMapper.h"
#include "OrderStore.h"
#include "NetworkID.h"
#include "SymbolUtility.h"
#include "../UFC/TParseData.h"
#include "../UFC/TFixedFormat.h"
//------------------------------------------------------------------------------
class TSCTSEOrderRecord : public OrderRecord
{
private:
    static UFC::TFixedFormat*	FOrderFormat;
    static UFC::TRenderData*	FOrderRender;
private:
	UFC::AnsiString		FAccount;
    UFC::AnsiString		FSecurityID;
    UFC::AnsiString     FSalesID;
public:
	UFC::AnsiString&	GetAccount( void )         { return FAccount; }
	UFC::AnsiString&	GetSecurityID( void )	    { return FSecurityID; }
public:
    ///< For Order
    TSCTSEOrderRecord(  ClientOrderID&                      CNID,
                        UFC::AnsiString&                    OrderID,
                        FIX::ClOrdID&                       ClOrdID,
                        FIX::Account&                       Account,
                        FIX::Symbol&                        Symbol,
                        FIX::OrderQty&                      OrderQty,
                        FIX::Price&                         Price,
                        FIX::Side&                          Side,
                        FIX::TimeInForce&                   TimeInForce,
                        UFC::AnsiString&                    SalesID,
                        UFC::AnsiString&                    Sender);
	TSCTSEOrderRecord( );
public:
	virtual void SaveToBuffer( UFC::MemoryStream& Stream );
	virtual void LoadFromBuffer( UFC::MemoryStream& Stream );
    virtual void NewOrderString( UFC::AnsiString& );
    virtual void CancelString( Int32 NID, UFC::AnsiString& );
    virtual void ReduceString( Int32 NID, Int32 NewQty, UFC::AnsiString& );
	virtual void FillPendingNew( FIX44::ExecutionReport& report );
};
//------------------------------------------------------------------------------
class TSCTSEOrderFactory : public OrderFactory
{
	virtual OrderRecord* CreateOrder( ) { return new TSCTSEOrderRecord( ); }
};
//------------------------------------------------------------------------------
class TSCTSEExecutionParser : public ExecutionParser
{
private:
    UFC::TParseData	 FFillParser;
    UFC::TFixedFormat FFillDataFormat;
public:
    TSCTSEExecutionParser()
    {
        FFillDataFormat.SetIniFile( "../cfg/TseOrderOK.ini" );
        FFillParser.SetDataFormat( &FFillDataFormat );
    }
    virtual BOOL Parse( MarketKind Market, const UFC::AnsiString& DataString );
};
//------------------------------------------------------------------------------
class TSCTSEConfirmParser : public ConfirmParser
{
private:
    UFC::TParseData		FOrderMKParser;
    UFC::TFixedFormat	FOrderMKDataFormat;
public:
    TSCTSEConfirmParser()
    {
        FOrderMKDataFormat.SetIniFile( "../cfg/TseOrderMK.ini" );
        FOrderMKParser.SetDataFormat( &FOrderMKDataFormat );
    }
public:
    virtual bool Parse( MarketKind Market, const UFC::AnsiString& DataString );
};
//------------------------------------------------------------------------------
#endif
