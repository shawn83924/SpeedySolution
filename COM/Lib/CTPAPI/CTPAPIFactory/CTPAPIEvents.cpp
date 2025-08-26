// CTPAPIFactory.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "CTPAPIComm.h"
//---------------------------------------------------------------------
void CTPAPIHandler::OnFrontConnected()
{
	if( FhCTP->OnFrontConnected != NULL )
		FhCTP->OnFrontConnected( FhCTP->FOwner );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnFrontDisconnected(int nReason)
{
	if( FhCTP->OnFrontDisconnected != NULL )
		FhCTP->OnFrontDisconnected( FhCTP->FOwner, nReason );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                        CThostFtdcRspInfoField *pRspInfo,
                        int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspUserLogin != NULL )
		FhCTP->OnRspUserLogin( FhCTP->FOwner, pRspUserLogin, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                            CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspUserLogout != NULL )
		FhCTP->OnRspUserLogout( FhCTP->FOwner, pUserLogout, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQrySettlementInfo != NULL )
		FhCTP->OnRspQrySettlementInfo( FhCTP->FOwner, pSettlementInfo, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspSettlementInfoConfirm != NULL )
		FhCTP->OnRspSettlementInfoConfirm( FhCTP->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                            CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspOrderInsert != NULL )
		FhCTP->OnRspOrderInsert( FhCTP->FOwner, pInputOrder, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnOrderInsert != NULL )
		FhCTP->OnErrRtnOrderInsert( FhCTP->FOwner, pInputOrder, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                            CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspOrderAction != NULL )
		FhCTP->OnRspOrderAction( FhCTP->FOwner, pInputOrderAction, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnOrderAction != NULL )
		FhCTP->OnErrRtnOrderAction( FhCTP->FOwner, pOrderAction, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryOrder(CThostFtdcOrderField *pOrder,
                        CThostFtdcRspInfoField *pRspInfo,
                        int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryOrder != NULL )
		FhCTP->OnRspQryOrder( FhCTP->FOwner, pOrder, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryExchange(CThostFtdcExchangeField *pExchange,
                            CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryExchange != NULL )
		FhCTP->OnRspQryExchange( FhCTP->FOwner, pExchange, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                            CThostFtdcRspInfoField *pRspInfo,
                            int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInstrument != NULL )
		FhCTP->OnRspQryInstrument( FhCTP->FOwner, pInstrument, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInvestorPosition != NULL )
		FhCTP->OnRspQryInvestorPosition( FhCTP->FOwner, pInvestorPosition, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if( FhCTP->OnRtnOrder != NULL )
		FhCTP->OnRtnOrder( FhCTP->FOwner, pOrder );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if( FhCTP->OnRtnTrade != NULL )
		FhCTP->OnRtnTrade( FhCTP->FOwner, pTrade );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspError != NULL )
		FhCTP->OnRspError( FhCTP->FOwner, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
///< Not implement RegXXXXXX
//---------------------------------------------------------------------
void CTPAPIHandler::OnHeartBeatWarning( int nTimeLapse)
{
	if( FhCTP->OnHeartBeatWarning != NULL )
		FhCTP->OnHeartBeatWarning( FhCTP->FOwner, nTimeLapse );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspAuthenticate( CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspAuthenticate != NULL )
		FhCTP->OnRspAuthenticate( FhCTP->FOwner, pRspAuthenticateField, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspUserPasswordUpdate( CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspUserPasswordUpdate != NULL )
		FhCTP->OnRspUserPasswordUpdate( FhCTP->FOwner, pUserPasswordUpdate, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspTradingAccountPasswordUpdate( CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspTradingAccountPasswordUpdate != NULL )
		FhCTP->OnRspTradingAccountPasswordUpdate( FhCTP->FOwner, pTradingAccountPasswordUpdate, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspParkedOrderInsert( CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspParkedOrderInsert != NULL )
		FhCTP->OnRspParkedOrderInsert( FhCTP->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspParkedOrderAction( CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspParkedOrderAction != NULL )
		FhCTP->OnRspParkedOrderAction( FhCTP->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQueryMaxOrderVolume( CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQueryMaxOrderVolume != NULL )
		FhCTP->OnRspQueryMaxOrderVolume( FhCTP->FOwner, pQueryMaxOrderVolume, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspRemoveParkedOrder( CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspRemoveParkedOrder != NULL )
		FhCTP->OnRspRemoveParkedOrder( FhCTP->FOwner, pRemoveParkedOrder, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspRemoveParkedOrderAction( CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspRemoveParkedOrderAction != NULL )
		FhCTP->OnRspRemoveParkedOrderAction( FhCTP->FOwner, pRemoveParkedOrderAction, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTrade( CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTrade != NULL )
		FhCTP->OnRspQryTrade( FhCTP->FOwner, pTrade, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTradingAccount( CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTradingAccount != NULL )
		FhCTP->OnRspQryTradingAccount( FhCTP->FOwner, pTradingAccount, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInvestor( CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInvestor != NULL )
		FhCTP->OnRspQryInvestor( FhCTP->FOwner, pInvestor, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTradingCode( CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTradingCode != NULL )
		FhCTP->OnRspQryTradingCode( FhCTP->FOwner, pTradingCode, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInstrumentMarginRate != NULL )
		FhCTP->OnRspQryInstrumentMarginRate( FhCTP->FOwner, pInstrumentMarginRate, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInstrumentCommissionRate != NULL )
		FhCTP->OnRspQryInstrumentCommissionRate( FhCTP->FOwner, pInstrumentCommissionRate, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryDepthMarketData != NULL )
		FhCTP->OnRspQryDepthMarketData( FhCTP->FOwner, pDepthMarketData, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTransferBank( CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTransferBank != NULL )
		FhCTP->OnRspQryTransferBank( FhCTP->FOwner, pTransferBank, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInvestorPositionDetail != NULL )
		FhCTP->OnRspQryInvestorPositionDetail( FhCTP->FOwner, pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryNotice( CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryNotice != NULL )
		FhCTP->OnRspQryNotice( FhCTP->FOwner, pNotice, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQrySettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQrySettlementInfoConfirm != NULL )
		FhCTP->OnRspQrySettlementInfoConfirm( FhCTP->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryInvestorPositionCombineDetail( CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryInvestorPositionCombineDetail != NULL )
		FhCTP->OnRspQryInvestorPositionCombineDetail( FhCTP->FOwner, pInvestorPositionCombineDetail, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryCFMMCTradingAccountKey( CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryCFMMCTradingAccountKey != NULL )
		FhCTP->OnRspQryCFMMCTradingAccountKey( FhCTP->FOwner, pCFMMCTradingAccountKey, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryEWarrantOffset( CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryEWarrantOffset != NULL )
		FhCTP->OnRspQryEWarrantOffset( FhCTP->FOwner, pEWarrantOffset, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTransferSerial( CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTransferSerial != NULL )
		FhCTP->OnRspQryTransferSerial( FhCTP->FOwner, pTransferSerial, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryAccountregister( CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryAccountregister != NULL )
		FhCTP->OnRspQryAccountregister( FhCTP->FOwner, pAccountregister, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	if( FhCTP->OnRtnInstrumentStatus != NULL )
		FhCTP->OnRtnInstrumentStatus( FhCTP->FOwner, pInstrumentStatus );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo)
{
	if( FhCTP->OnRtnTradingNotice != NULL )
		FhCTP->OnRtnTradingNotice( FhCTP->FOwner, pTradingNoticeInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnErrorConditionalOrder( CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder)
{
	if( FhCTP->OnRtnErrorConditionalOrder != NULL )
		FhCTP->OnRtnErrorConditionalOrder( FhCTP->FOwner, pErrorConditionalOrder );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryContractBank( CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryContractBank != NULL )
		FhCTP->OnRspQryContractBank( FhCTP->FOwner, pContractBank, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryParkedOrder( CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryParkedOrder != NULL )
		FhCTP->OnRspQryParkedOrder( FhCTP->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryParkedOrderAction( CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryParkedOrderAction != NULL )
		FhCTP->OnRspQryParkedOrderAction( FhCTP->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryTradingNotice( CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryTradingNotice != NULL )
		FhCTP->OnRspQryTradingNotice( FhCTP->FOwner, pTradingNotice, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryBrokerTradingParams( CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryBrokerTradingParams != NULL )
		FhCTP->OnRspQryBrokerTradingParams( FhCTP->FOwner, pBrokerTradingParams, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQryBrokerTradingAlgos( CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQryBrokerTradingAlgos != NULL )
		FhCTP->OnRspQryBrokerTradingAlgos( FhCTP->FOwner, pBrokerTradingAlgos, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnFromBankToFutureByBank( CThostFtdcRspTransferField *pRspTransfer)
{
	if( FhCTP->OnRtnFromBankToFutureByBank != NULL )
		FhCTP->OnRtnFromBankToFutureByBank( FhCTP->FOwner, pRspTransfer);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnFromFutureToBankByBank( CThostFtdcRspTransferField *pRspTransfer)
{
	if( FhCTP->OnRtnFromFutureToBankByBank != NULL )
		FhCTP->OnRtnFromFutureToBankByBank( FhCTP->FOwner, pRspTransfer);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromBankToFutureByBank( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromBankToFutureByBank != NULL )
		FhCTP->OnRtnRepealFromBankToFutureByBank( FhCTP->FOwner, pRspRepeal);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromFutureToBankByBank( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromFutureToBankByBank != NULL )
		FhCTP->OnRtnRepealFromFutureToBankByBank( FhCTP->FOwner, pRspRepeal);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnFromBankToFutureByFuture( CThostFtdcRspTransferField *pRspTransfer)
{
	if( FhCTP->OnRtnFromBankToFutureByFuture != NULL )
		FhCTP->OnRtnFromBankToFutureByFuture( FhCTP->FOwner, pRspTransfer);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnFromFutureToBankByFuture( CThostFtdcRspTransferField *pRspTransfer)
{
	if( FhCTP->OnRtnFromFutureToBankByFuture != NULL )
		FhCTP->OnRtnFromFutureToBankByFuture( FhCTP->FOwner, pRspTransfer);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromBankToFutureByFutureManual( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromBankToFutureByFutureManual != NULL )
		FhCTP->OnRtnRepealFromBankToFutureByFutureManual( FhCTP->FOwner, pRspRepeal);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromFutureToBankByFutureManual( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromFutureToBankByFutureManual != NULL )
		FhCTP->OnRtnRepealFromFutureToBankByFutureManual( FhCTP->FOwner, pRspRepeal);
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnQueryBankBalanceByFuture( CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount)
{
	if( FhCTP->OnRtnQueryBankBalanceByFuture != NULL )
		FhCTP->OnRtnQueryBankBalanceByFuture( FhCTP->FOwner, pNotifyQueryAccount );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnBankToFutureByFuture( CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnBankToFutureByFuture != NULL )
		FhCTP->OnErrRtnBankToFutureByFuture( FhCTP->FOwner, pReqTransfer, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnFutureToBankByFuture( CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnFutureToBankByFuture != NULL )
		FhCTP->OnErrRtnFutureToBankByFuture( FhCTP->FOwner, pReqTransfer, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnRepealBankToFutureByFutureManual( CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnRepealBankToFutureByFutureManual != NULL )
		FhCTP->OnErrRtnRepealBankToFutureByFutureManual( FhCTP->FOwner, pReqRepeal, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnRepealFutureToBankByFutureManual( CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnRepealFutureToBankByFutureManual != NULL )
		FhCTP->OnErrRtnRepealFutureToBankByFutureManual( FhCTP->FOwner, pReqRepeal, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnErrRtnQueryBankBalanceByFuture( CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo)
{
	if( FhCTP->OnErrRtnQueryBankBalanceByFuture != NULL )
		FhCTP->OnErrRtnQueryBankBalanceByFuture( FhCTP->FOwner, pReqQueryAccount, pRspInfo );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromBankToFutureByFuture( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromBankToFutureByFuture != NULL )
		FhCTP->OnRtnRepealFromBankToFutureByFuture( FhCTP->FOwner, pRspRepeal );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnRepealFromFutureToBankByFuture( CThostFtdcRspRepealField *pRspRepeal)
{
	if( FhCTP->OnRtnRepealFromFutureToBankByFuture != NULL )
		FhCTP->OnRtnRepealFromFutureToBankByFuture( FhCTP->FOwner, pRspRepeal );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspFromBankToFutureByFuture( CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspFromBankToFutureByFuture != NULL )
		FhCTP->OnRspFromBankToFutureByFuture( FhCTP->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspFromFutureToBankByFuture( CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspFromBankToFutureByFuture != NULL )
		FhCTP->OnRspFromBankToFutureByFuture( FhCTP->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRspQueryBankAccountMoneyByFuture( CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhCTP->OnRspQueryBankAccountMoneyByFuture != NULL )
		FhCTP->OnRspQueryBankAccountMoneyByFuture( FhCTP->FOwner, pReqQueryAccount, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnOpenAccountByBank( CThostFtdcOpenAccountField *pOpenAccount)
{
	if( FhCTP->OnRtnOpenAccountByBank != NULL )
		FhCTP->OnRtnOpenAccountByBank( FhCTP->FOwner, pOpenAccount );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnCancelAccountByBank( CThostFtdcCancelAccountField *pCancelAccount)
{
	if( FhCTP->OnRtnCancelAccountByBank != NULL )
		FhCTP->OnRtnCancelAccountByBank( FhCTP->FOwner, pCancelAccount );
}
//---------------------------------------------------------------------
void CTPAPIHandler::OnRtnChangeAccountByBank( CThostFtdcChangeAccountField *pChangeAccount)
{
	if( FhCTP->OnRtnChangeAccountByBank != NULL )
		FhCTP->OnRtnChangeAccountByBank( FhCTP->FOwner, pChangeAccount );
}
//---------------------------------------------------------------------

