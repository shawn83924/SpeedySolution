//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
#include "KStrdBCBTraderAPI.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
#include <map>
#include <vector>
//---------------------------------------------------------------------------
using namespace std; // #include <map>
//---------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
class KStrdListener : public TKStrdTraderSpi
{
public:
	virtual void OnKStrdFrontConnected( );
	virtual void OnKStrdFrontDisconnected( int nReason );
	virtual void OnKStrdRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
public:
	virtual void OnKStrdRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnKStrdRtnOrder( KS_::CThostFtdcOrderField *pOrder );
	virtual void OnKStrdRtnTrade( KS_::CThostFtdcTradeField *pTrade );
	virtual void OnKStrdRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
public:
	virtual void OnKStrdRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnKStrdErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo);
	// Lock
	///鎖定應答
	virtual void OnKStrdRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///請求查詢鎖定回應
	virtual void OnKStrdRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///請求查詢鎖定證券倉位回應
	virtual void OnKStrdRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	///鎖定通知
	virtual void OnKStrdRtnLock( KS_::CThostFtdcLockField *pLock );
	///鎖定錯誤通知
	virtual void OnKStrdErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo );
};
//---------------------------------------------------------------------------
class TMemoData //Memo資料
{
public:
	String sKind;
	String sText;
TMemoData():sKind(""),sText(""){}
};
//---------------------------------------------------------------------------
class TStockLock //StockLock資料
{
public:
	String sInsertDate; 	// 委託(報單)日期
	String sInsertTime; 	// 委託(插入)時間
	String sRequestID; 		// 請求編號 8 碼
	String sInstrumentID;   // 商品(合約)代碼
	String sLockType;		// 鎖定類型(申請類別)
	int iVolume;			// 數量
	String sStatusMsg;		// 狀態資訊(備註)
	String sClearingPartID;	// 結算會員編號(席位號)
public:
	void DataClear();

	TStockLock(); // constructor
};
//---------------------------------------------------------------------------
class TTradingForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel_Main;
	TMemo *Memo1;
	TPanel *Panel_Top;
	TEdit *TxtServerAddress;
	TLabel *Label4;
	TEdit *TxtPassword;
	TLabel *Label3;
	TEdit *TxtUserID;
	TLabel *Label2;
	TLabel *Label1;
	TEdit *TxtBrokerID;
	TPanel *Panel_Left;
	TButton *BtnConn_Login;
	TButton *BtnDisconnect;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *Button6;
	TSplitter *Splitter1;
	TEdit *TxtFrontID;
	TEdit *TxtSessionID;
	TLabel *Label5;
	TLabel *Label6;
	TPanel *Panel_Bottom;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TRadioGroup *RadioGroup_BS;
	TRadioGroup *RadioGroup_CombHedge;
	TEdit *TxtExchangeID;
	TEdit *TxtInstrumentID;
	TEdit *TxtQty;
	TEdit *TxtPx;
	TButton *Button1;
	TLabel *Label7;
	TEdit *TxtOrderSysID;
	TEdit *TxtOrderLocalID;
	TLabel *Label9;
	TButton *Button7;
	TTabSheet *TabSheet3;
	TButton *Button8;
	TButton *Button9;
	TButton *Button10;
	TRadioGroup *RadioGroup_Lock;
	TEdit *TxtMaxOrderRef;
	TLabel *Label10;
	TEdit *TxtInstrumentStockID;
	TLabel *Label12;
	TEdit *TxtLockSysID;
	TPanel *Panel_Ref;
	TLabel *Label8;
	TEdit *TxtOrderRef;
	TPanel *Panel_LockRtn;
	TPanel *Panel_LockRtn_Menu;
	TPanel *Panel_LockRtn_Grid;
	TStringGrid *StringGrid_StockQry;
	TStringGrid *StringGrid_StockLock;
	TPanel *Panel_LockRtn_Grid_Tit;
	TPanel *Panel_LockRtn_StockQry_Tit;
	TPanel *Panel_LockRtn_StockLock_Tit;
	TPanel *Panel_LockRtn_Menu_Main;
	TLabel *Label_LockRtn_Menu;
	TLabel *Label_StockID;
	TComboBox *ComboBox_StockID;
	TLabel *Label_MaxLock;
	TEdit *Edit_MaxLock;
	TLabel *Label_MaxUnLock;
	TEdit *Edit_MaxUnLock;
	TLabel *Label_Qty;
	TEdit *Edit_Qty;
	TBitBtn *BitBtn_MaxLock_Qry;
	TBitBtn *BitBtn_MaxUnLock_Qry;
	TBitBtn *BitBtn_Lock;
	TBitBtn *BitBtn_UnLock;
	TLabel *Label_LockRtn_StockQry_Tit;
	TLabel *Label_LockRtn_StockLock_Tit;
	TEdit *TxtLockVolume;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall BtnConn_LoginClick(TObject *Sender);
	void __fastcall BtnDisconnectClick(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall StringGrid_StockQryDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall StringGrid_StockLockDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);

private:
	//
	TColor m_StockQry_BgColor; // 持倉查詢-背景顏色
	TColor m_StockLock_BgColor; // 鎖定委託-背景顏色
	//
	void GetGlobalVariant( void );
	void SetGlobalVariant( void );

public:
	TTimer *TimerMsg;
	TTimer *TimerStockLock;
	//map<String, TMemoData> mapMemoData;
	//map<String, TMemoData> :: iterator pMD;

	// RequestID 型態 8碼
	// 11:37:59+01
	// "11375901" ==> Key一共8碼
	map<String, TStockLock> mapStockLock; // StockLock 的資料 for "StringGrid_StockLock"
	map<String, TStockLock>::iterator p_StockLock;
	TStockLock StockLock;

	//

	void __fastcall OnTimerMsg(TObject *Sender);
	void __fastcall OnTimerStockLock(TObject *Sender);
	void SetButtons( bool IsConnected );
	void ReleaseAPI( void );
	void AddMemo(String Kind, String Text);
	void DoStockLock(TStockLock aStockLock);
	void StringGrid_DataClear(TStringGrid *Grid, int iCount);

public:		// User declarations
	KStrdListener* 		FListener;
	TKStrdTraderApi*	FKSTRDAPI;

public:		// User declarations
	TStringList*	FStrings;
	__fastcall TTradingForm(TComponent* Owner);
	__fastcall ~TTradingForm( );
};
//---------------------------------------------------------------------------
extern PACKAGE TTradingForm *TradingForm;
//---------------------------------------------------------------------------
#endif
