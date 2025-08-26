//---------------------------------------------------------------------------
#ifndef ORCTFX_MAPPER_H
#define ORCTFX_MAPPER_H
//---------------------------------------------------------------------------
#include "TSCTSEOrder.h"
#include "imFIXMapper.h"
#include "SymbolUtility.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/NewOrderMultileg.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/OrderCancelReplaceRequest.h"
#include "quickfix/fix44/OrderCancelReject.h"
#include "quickfix/fix44/SecurityDefinition.h"
#include "quickfix/fix44/SecurityDefinitionRequest.h"
#include "../UFC/TRenderData.h"
#include "../UFC/TFixedFormat.h"
#include "SecurityDefinitionParser.h"
#include "CheckLeftQty.h"
#include <math.h>
//---------------------------------------------------------------------------
extern FIXMapperApp* Application;
//------------------------------------------------------------------------------
class OrcTSEMapper : public FIXMapper
{
private:
    UFC::UiniFile         ErrMsgFile;
    LeftQtyStore*         LStore;
private:
  ///< For Stock
  void StockFilled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void StockCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution );
  void StockReplaced( OrderRecord* OrgOrd, ExecutionParser* Execution );
private:
  void Reject( const char* OIDStr,
               const char* ErrorText,
               FIX::Side Side,
               FIX::ClOrdID&                      ClOrdID,
               FIX::Symbol&                       Symbol,
               FIX::OrdType                       OrderType,
               FIX::OrderQty                      OrderQty );
  ///< Functions send OrderCancelReject to reject OrderCancelRequest or OrderCancelReplaceRequest.
  void RejectCancelNotFound( FIX::ClOrdID ClOrdID, FIX::OrigClOrdID OrigClOrdID,FIX::CxlRejResponseTo CxlRejResponseTo );
  void RejectCancelCanceled( FIX::ClOrdID,FIX::OrigClOrdID,FIX::OrderID,FIX::CxlRejResponseTo );
  void RejectCancelFilled( FIX::ClOrdID,FIX::OrigClOrdID,FIX::OrderID,FIX::CxlRejResponseTo );
  ///< Functions to handle confirm error message
  void OrderCancelReject( Int32 NID, OrderRecord* OrgOrd,UFC::AnsiString& ErrorStr, BOOL IsCancel );
  void RejectExecution( OrderRecord* OrgOrd, UFC::AnsiString& ErrorStr );
  ///< Functions to handle confirm message
  void FillOrderStatus( ClientOrderID& NID, FIX::ExecType& ExecType, FIX::OrdStatus& OrdStatus );
  void ReplyConfirmExecution( OrderRecord* OrgOrd, FIX::ExecType ExecType, FIX::OrdStatus OrdStatus );
public:
  OrcTSEMapper( const UFC::AnsiString& MapperName, OrderFactory* Factory,NIDRule* Rule, BOOL Recover, LeftQtyStore* LeftStore )
  :FIXMapper( MapperName, Factory, Rule, Recover )
  ,LStore( LeftStore )
  ,ErrMsgFile( "../cfg/ErrorMessage.ini" ){}
  virtual void onCommand( Int32 CMD, UFC::AnsiString Data );
  ///< Handle messages from TSE.
  virtual void onStockConfirm( Int32 NID, ConfirmParser* Confirm );
  virtual void onStockError( Int32 NID, ConfirmParser* );
  virtual void onStockExecution( ExecutionParser* Execution );
  ///< Handle messages from TAIFEX Options.(Speedy)
  virtual void onOptionsConfirm( Int32 NID, ConfirmParser* Confirm ){UFC::BufferedLog::Printf( " It's a Stock Mapper,Options confirm NID:%08d not found!\n", NID);}
  virtual void onOptionsError( Int32 NID, ConfirmParser* )          {UFC::BufferedLog::Printf( " It's a Stock Mapper,Options error NID:%08d not found!\n", NID);}
  virtual void onOptionsExecution( ExecutionParser* Execution  )    {UFC::BufferedLog::Printf( " It's a Stock Mapper,but received a Options execution");}
  virtual void onCancelOrder( Int32 NID )                           {UFC::BufferedLog::Printf( " It's a Stock Mapper,but received a Options auto delete order event");}
  ///< Handle messages from TAIFEX Futures.(Speedy)
  virtual void onFuturesConfirm( Int32 NID, ConfirmParser* Confirm ){UFC::BufferedLog::Printf( " It's a Stock Mapper,Futrres confirm NID:%08d not found!\n", NID);}
  virtual void onFuturesError( Int32 NID, ConfirmParser* )          {UFC::BufferedLog::Printf( " It's a Stock Mapper,Futrres error NID:%08d not found!\n", NID);}
  virtual void onFuturesExecution( ExecutionParser* Execution  )    {UFC::BufferedLog::Printf( " It's a Stock Mapper,but received a Futrres execution");}
  ///< Handle FIX incoming messages.
  virtual void onMessage( const FIX44::NewOrderSingle& Order, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::OrderCancelRequest& Cancel, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::OrderCancelReplaceRequest& Cancel, const FIX::SessionID& sessionID );
  virtual void onMessage( const FIX44::SecurityDefinitionRequest & Request, const FIX::SessionID & sessionID );
  ///<Handle error message from TSE or AS/400
  UFC::AnsiString GetErrorMessage( UFC::AnsiString ErrorCode);

private:///> for SecurityDefinitionDownload
  void ParseT30V(const FIX44::SecurityDefinitionRequest& Request);
  void ParseT30OV(const FIX44::SecurityDefinitionRequest& Request);
  void SendSecurityDefinition(const char* PrefixString, const char* RequestID, SecurityDefinitionParser &TSecurityDefinitionParser);
};
//---------------------------------------------------------------------------
#endif
