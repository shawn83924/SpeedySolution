//---------------------------------------------------------------------------
#ifndef ORCTFX_MAPPER_H
#define ORCTFX_MAPPER_H
//---------------------------------------------------------------------------
#include "TSCTFXOrder.h"
#include "imFIXMapper.h"
#include "SymbolUtility.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/NewOrderMultileg.h"
#include "quickfix/fix44/Quote.h"
#include "quickfix/fix44/QuoteCancel.h"
#include "quickfix/fix44/QuoteRequest.h"
#include "quickfix/fix44/QuoteStatusReport.h"
#include "quickfix/fix44/SecurityDefinition.h"
#include "quickfix/fix44/SecurityDefinitionRequest.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/OrderCancelReplaceRequest.h"
#include "quickfix/fix44/OrderCancelReject.h"
#include "../UFC/iniFile.h"
//---------------------------------------------------------------------------
extern FIXMapperApp* Application;

//------------------------------------------------------------------------------

class OrcTFXMapper : public FIXMapper
{
private:
  UFC::PHashedList<UFC::AnsiString, AccountAttribute*>  FAccountList;
  UFC::UiniFile   ErrorMessage;
private:
  ///< For Futures
  void FutFilled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void FutCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void FutReplaced( OrderRecord* OrgOrd, ExecutionParser* Execution );
  ///< For Option
  void OptFilled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void OptCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void OptReplaced( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void QuoteCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution );
private:
  void SendSecurityDefinitionOPT( const UFC::AnsiString & IniFilePath, const FIX44::SecurityDefinitionRequest& Request );
  void SendSecurityDefinitionFUT( const UFC::AnsiString & IniFilePath, const FIX44::SecurityDefinitionRequest& Request );
  void ReportQuoteStatus( FIX::QuoteID QuoteID, FIX::QuoteStatus QuoteStatus, FIX::Text Text  );
  ///< Functions send OrderCancelReject to reject OrderCancelRequest or OrderCancelReplaceRequest.
  void RejectCancelNotFound( FIX::ClOrdID ClOrdID, FIX::OrigClOrdID OrigClOrdID,FIX::CxlRejResponseTo CxlRejResponseTo );
  void RejectCancelCanceled( FIX::ClOrdID,FIX::OrigClOrdID,FIX::OrderID,FIX::CxlRejResponseTo );
  void RejectCancelFilled( FIX::ClOrdID,FIX::OrigClOrdID,FIX::OrderID,FIX::CxlRejResponseTo );

  void Reject( const char* OIDStr,const char* ErrorText,FIX::Side Side,FIX::ClOrdID& ClOrdID,FIX::Symbol& Symbol,FIX::OrdType OrderType,FIX::OrderQty OrderQty );
  void AddLegs( OrderRecord* NewOrder, const FIX44::NewOrderMultileg& Order, int Count, int Qty );
  ///< Functions to handle confirm message
  void FillOrderStatus( ClientOrderID& NID, FIX::ExecType& ExecType, FIX::OrdStatus& OrdStatus );
  void ReplyConfirmExecution( OrderRecord* OrgOrd, FIX::ExecType ExecType, FIX::OrdStatus OrdStatus );
  ///< Functions to handle confirm error message
  void OrderCancelReject( Int32 NID,OrderRecord* OrgOrd, UFC::AnsiString& ErrorStr, BOOL IsCancel );
  void RejectExecution( OrderRecord* OrgOrd, UFC::AnsiString& ErrorStr );

  void PrintFieldNotfond( const char* Message, const char* FieldName );
public:
  OrcTFXMapper( const UFC::AnsiString& MapperName, OrderFactory* Factory,NIDRule* Rule, BOOL Recover );
  ///< Handle messages form TSE.
  virtual void onStockConfirm( Int32 NID, ConfirmParser* Confirm );
  virtual void onStockError( Int32 NID, ConfirmParser* );
  virtual void onStockExecution( ExecutionParser* Execution ) {}
  ///< Handle messages form TAIFEX Options.(Speedy)
  virtual void onOptionsConfirm( Int32 NID, ConfirmParser* Confirm );
  virtual void onOptionsError( Int32 NID, ConfirmParser* );
  virtual void onOptionsExecution( ExecutionParser* Execution  );
  virtual void onCancelOrder( Int32 NID );
  ///< Handle messages form TAIFEX Futures.(Speedy)
  virtual void onFuturesConfirm( Int32 NID, ConfirmParser* Confirm );
  virtual void onFuturesError( Int32 NID, ConfirmParser* );
  virtual void onFuturesExecution( ExecutionParser* Execution  );
  ///< Handle FIX incoming messages.
  virtual void onMessage( const FIX44::NewOrderSingle& Order, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::OrderCancelRequest& Cancel, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::OrderCancelReplaceRequest& Cancel, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::Quote& Quote, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::QuoteCancel& QuoteCancel, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::SecurityDefinitionRequest & Request, const FIX::SessionID & sessionID );
  virtual void onMessage( const FIX44::QuoteRequest& QuoteRequest, const FIX::SessionID& sessionID );
public:
  void ReadAccountList( void );
};
//---------------------------------------------------------------------------

#endif
