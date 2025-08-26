//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#ifndef KStrdConnListenerH
#define KStrdConnListenerH
//------------------------------------------------------------------------------------------------------------------------
#include "KStrdBCBTraderAPI.h"
//------------------------------------------------------------------------------------------------------------------------
class TTaifexConnection;
//------------------------------------------------------------------------------------------------------------------------
//
// class KStrdConnListener
// Implement TKStrdTraderSpi callback functions
//
// OnKStrdAPIConnected, OnKStrdAPIDisconnected Trigger by KStrd API thread.
// Not the same thread as the main UI thread!
// Don't modify any GDI object in this function
//
//------------------------------------------------------------------------------------------------------------------------
class KStrdConnListener : public TKStrdTraderSpi
{
private:
	TTaifexConnection* FSpeedyConnPtr;
public:
	void OnKStrdAPIConnected();
	void OnKStrdAPIDisconnected( int nReason );
	void OnKStrdAPIRecoverFinished( int Count );
	// ---- CThostFtdcTraderSpi ----
	void OnKStrdFrontConnected( );
	void OnKStrdFrontDisconnected( int nReason );
	void OnKStrdRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus );
	void OnKStrdRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer );
	void OnKStrdRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer );
	void OnKStrdRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount );
	void OnKStrdRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	// [KStrd] KingStar Reject execution
	void OnKStrdRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	void OnKStrdRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	// [KStrd] Exchange Order confirm
	void OnKStrdRtnOrder( KS_::CThostFtdcOrderField *pOrder );
	void OnKStrdErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	void OnKStrdErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	// [KStrd] Order fill execution
	void OnKStrdRtnTrade( KS_::CThostFtdcTradeField *pTrade );
	// ---- CKSOptionSpi ----
	// all no implement
	// ---- CKSVocSpi ----
	// [KStrd] KingStar special Investor Position for Open
	void OnKStrdRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );

	// Lock
	///鎖定應答
	void OnKStrdRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///請求查詢鎖定回應
	void OnKStrdRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///請求查詢鎖定證券倉位回應
	void OnKStrdRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///鎖定通知
	void OnKStrdRtnLock( KS_::CThostFtdcLockField *pLock );
	///鎖定錯誤通知
	void OnKStrdErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo );

public:
	KStrdConnListener( TTaifexConnection* SpeedyConnPtr );
	~KStrdConnListener();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

