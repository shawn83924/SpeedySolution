
#include "stdafx.h"
#include "FemasAPIComm.h"

//---------------------------------------------------------------------
void USTPAPIHandler::OnFrontConnected()
{
	if( FhUSTP->OnFrontConnected != NULL )
		FhUSTP->OnFrontConnected( FhUSTP->FOwner );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnFrontDisconnected(int nReason)
{
	if( FhUSTP->OnFrontDisconnected != NULL )
		FhUSTP->OnFrontDisconnected( FhUSTP->FOwner, nReason );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnHeartBeatWarning(int nTimeLapse)
{
	if( FhUSTP->OnHeartBeatWarning != NULL )
		FhUSTP->OnHeartBeatWarning( FhUSTP->FOwner,nTimeLapse);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnPackageStart(int nTopicID, int nSequenceNo)
{
	if( FhUSTP->OnPackageStart != NULL )
		FhUSTP->OnPackageStart( FhUSTP->FOwner, nTopicID, nSequenceNo);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnPackageEnd(int nTopicID, int nSequenceNo)
{
	if( FhUSTP->OnPackageEnd != NULL )
		FhUSTP->OnPackageEnd( FhUSTP->FOwner, nTopicID, nSequenceNo);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspError != NULL )
		FhUSTP->OnRspError( FhUSTP->FOwner, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspUserLogin != NULL )
		FhUSTP->OnRspUserLogin( FhUSTP->FOwner, pRspUserLogin, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspUserLogout != NULL )
		FhUSTP->OnRspUserLogout( FhUSTP->FOwner, pRspUserLogout, pRspInfo,  nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspUserPasswordUpdate != NULL )
		FhUSTP->OnRspUserPasswordUpdate( FhUSTP->FOwner, pUserPasswordUpdate, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspOrderInsert != NULL )
	    FhUSTP->OnRspOrderInsert( FhUSTP->FOwner, pInputOrder, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspOrderAction != NULL )
		FhUSTP->OnRspOrderAction( FhUSTP->FOwner, pOrderAction, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *pFlowMessageCancel)
{
	if( FhUSTP->OnRtnFlowMessageCancel != NULL )
		FhUSTP->OnRtnFlowMessageCancel( FhUSTP->FOwner, pFlowMessageCancel ); 
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRtnTrade(CUstpFtdcTradeField *pTrade)
{
	if( FhUSTP->OnRtnTrade != NULL )
		FhUSTP->OnRtnTrade( FhUSTP->FOwner, pTrade ); 
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRtnOrder(CUstpFtdcOrderField *pOrder)
{
	if( FhUSTP->OnRtnOrder != NULL )
		FhUSTP->OnRtnOrder( FhUSTP->FOwner, pOrder );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
	if( FhUSTP->OnErrRtnOrderInsert != NULL )
		FhUSTP->OnErrRtnOrderInsert( FhUSTP->FOwner, pInputOrder, pRspInfo );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
	if( FhUSTP->OnErrRtnOrderAction != NULL )
		FhUSTP->OnErrRtnOrderAction( FhUSTP->FOwner, pOrderAction, pRspInfo);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus)
{
	if( FhUSTP->OnRtnInstrumentStatus != NULL )
		FhUSTP->OnRtnInstrumentStatus( FhUSTP->FOwner, pInstrumentStatus );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryOrder != NULL )
		FhUSTP->OnRspQryOrder( FhUSTP->FOwner, pOrder, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryTrade != NULL )
		FhUSTP->OnRspQryTrade( FhUSTP->FOwner, pTrade, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryUserInvestor != NULL )
	    FhUSTP->OnRspQryUserInvestor( FhUSTP->FOwner, pRspUserInvestor, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryTradingCode != NULL )
		FhUSTP->OnRspQryTradingCode( FhUSTP->FOwner, pRspTradingCode, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryInvestorAccount != NULL )
	    FhUSTP->OnRspQryInvestorAccount( FhUSTP->FOwner, pRspInvestorAccount, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryInstrument != NULL )
		FhUSTP->OnRspQryInstrument( FhUSTP->FOwner, pRspInstrument, pRspInfo, nRequestID, bIsLast );
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryExchange(CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryExchange != NULL )
  	    FhUSTP->OnRspQryExchange( FhUSTP->FOwner, pRspExchange, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryInvestorPosition != NULL )
		FhUSTP->OnRspQryInvestorPosition( FhUSTP->FOwner, pRspInvestorPosition, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspSubscribeTopic != NULL )
		FhUSTP->OnRspSubscribeTopic( FhUSTP->FOwner, pDissemination, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryComplianceParam != NULL )
		FhUSTP->OnRspQryComplianceParam( FhUSTP->FOwner, pRspComplianceParam, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryTopic != NULL )
		FhUSTP->OnRspQryTopic( FhUSTP->FOwner, pDissemination, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryInvestorFee != NULL )
		FhUSTP->OnRspQryInvestorFee( FhUSTP->FOwner, pInvestorFee, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------
void USTPAPIHandler::OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( FhUSTP->OnRspQryInvestorMargin != NULL )
		FhUSTP->OnRspQryInvestorMargin( FhUSTP->FOwner, pInvestorMargin, pRspInfo, nRequestID, bIsLast);
}
//---------------------------------------------------------------------