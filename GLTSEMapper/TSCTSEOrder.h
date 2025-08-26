//---------------------------------------------------------------------------
#if !defined(TSCTSEOrder_H)
#define TSCTSEOrder_H
//------------------------------------------------------------------------------
#include "../imFIXMapper/MapperApp.h"
#include "imFIXMapper.h"
#include "../imFIXMapper/OrderStore.h"
#include "../imFIXMapper/NetworkID.h"
#include "../imFIXMapper/SymbolUtility.h"
#include "../UFC/TParseData.h"
#include "../UFC/TFixedFormat.h"
#include "../UFC/TRenderData.h"
#include "T32Parser.h"
//------------------------------------------------------------------------------
class TSCTSEOrderRecord : public OrderRecord
{
private:
    static UFC::TFixedFormat*	FOrderFormat;
    static UFC::TRenderData*	FOrderRender;
public:
    static UFC::PHashedList<UFC::AnsiString, TradeUnit*> FTradeUnitTable;
private:
    UFC::AnsiString		FAccount;
    UFC::AnsiString		FSalesID;
public:
    UFC::AnsiString&	GetAccount( void )         { return FAccount; }
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
                        UFC::AnsiString&                    Sender,
                        BOOL                                IsMarket );
    TSCTSEOrderRecord( );
    static void InitRender();
    static void LoadTradeUnit( void );
public:
	virtual void SaveToBuffer( UFC::MemoryStream& Stream );
	virtual void LoadFromBuffer( UFC::MemoryStream& Stream );
    virtual void NewOrderString( UFC::AnsiString& );
    virtual void CancelString( Int32 NID, UFC::AnsiString& );
    virtual void ReduceString( Int32 NID, Int32 NewQty, UFC::AnsiString& );
	virtual void FillPendingNew( FIX44::ExecutionReport& report );
	void SetOrderRender(UFC::TRenderData* OrderRender) { FOrderRender = OrderRender; }
};
//------------------------------------------------------------------------------
class TSCTSEOrderFactory : public OrderFactory
{
	virtual OrderRecord* CreateOrder() { return new TSCTSEOrderRecord(); }
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
    virtual BOOL Parse( MarketKind Market, const UFC::AnsiString& DataString );
};
//------------------------------------------------------------------------------
#endif
