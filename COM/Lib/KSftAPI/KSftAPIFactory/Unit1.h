//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "KSftBCBTraderAPI.h"
//---------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
class KSftListener : public TKSftTraderSpi
{
public:
	virtual void OnKSftFrontConnected( );
	virtual void OnKSftFrontDisconnected( int nReason );
	virtual void OnKSftRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
public:
	virtual void OnKSftRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKSftRtnOrder( KS_::CThostFtdcOrderField *pOrder );
	virtual void OnKSftRtnTrade( KS_::CThostFtdcTradeField *pTrade );
	virtual void OnKSftRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
};
//---------------------------------------------------------------------------
class TTradingForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *BtnConn_Login;
	TButton *BtnDisconnect;
	TLabel *Label1;
	TEdit *TxtBrokerID;
	TEdit *TxtUserID;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *TxtPassword;
	TLabel *Label4;
	TEdit *TxtServerAddress;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall BtnConn_LoginClick(TObject *Sender);
	void __fastcall BtnDisconnectClick(TObject *Sender);
private:
	void GetGlobalVariant( void );
	void SetGlobalVariant( void );
public:
	void SetButtons( bool IsConnected );
	void ReleaseAPI( void );
public:		// User declarations
	KSftListener* 	FListener;
	TKSftTraderApi*	FKSFTAPI;
public:		// User declarations
	TStringList*	FStrings;
	__fastcall TTradingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTradingForm *TradingForm;
//---------------------------------------------------------------------------
#endif
