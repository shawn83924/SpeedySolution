//---------------------------------------------------------------------------

#ifndef LineEditFormH
#define LineEditFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include "LinkFrame.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
extern bool IsValidIP( const AnsiString& IP );
//---------------------------------------------------------------------------
class TPVCSettingForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *BrokerLabel;
	TLabel *CMorIPLabel;
	TLabel *PortLabel;
	TLabel *PVCLabel;
	TLabel *APCodeLabel;
	TLabel *PasswordLabel;
	TComboBox *APCodeComboBox;
	TBitBtn *OKBitBtn;
	TBitBtn *CancelBitBtn;
	TEdit *LinkEdit;
	TEdit *PVCEdit;
	TLabel *MarketLabel;
	TComboBox *SysComboBox;
	TEdit *LinePWDEdit;
	TEdit *CMEdit;
	TEdit *FCMEdit;
	TBevel *Bevel;
	TLabel *LocalPortURLLabel;
	TEdit *LocalPortURLEdit;
	TCheckBox *OffHourCheckBox;
	TLabel *PVCIDLabel;
	TEdit *PVCIDEdit;
	TCheckBox *EnabledCheckBox;
	TComboBox *ProtocolComboBox;
	TLabel *ProtocolLabel;
	TLabel *GroupLabel;
	TEdit *GroupEdit;
	TLabel *TWSETPLabel;
	TComboBox *TWSETPComboBox;
	void __fastcall APCodeComboBoxChange(TObject *Sender);
	void __fastcall SetMarketComboBox( MarketSet& Market );
	void __fastcall OKBitBtnClick(TObject *Sender);
	void __fastcall SysComboBoxChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	int  FLink;
	int  FPVC;
	int  FLocalPort;
	int  FGroup;
	bool FIsEnabled;
	bool FIsOffHour;
	AnsiString FFCM;
	AnsiString FCM;
	AnsiString FPWD;
	AnsiString FPVCID;
	int  FOrgLink;
	int  FOrgPVC;
	int  FOrgLocalPort;
	int  FOrgGroup;
	int  FOrgTPIndex;
	bool FOrgIsEnabled;
	bool FOrgIsOffHour;
	AnsiString FOrgFCM;
	AnsiString FOrgCM;
	AnsiString FOrgPWD;
	AnsiString FOrgPVCID;
	bool FIsCopy;
	void __fastcall SetExchange( bool IsTSEC );
	bool __fastcall CheckTAIFEX( );
	bool __fastcall CheckTSEC( );
public:		// User declarations
	__fastcall TPVCSettingForm( TComponent* Owner, MarketSet& Market );
	__fastcall TPVCSettingForm( TComponent* Owner, TLineInfo* info, MarketSet& Market, bool IsCopy = false );
    TMarket __fastcall GetMarket();  ///< Return the market type of this connection.
	void __fastcall SetAPCode( int APCode, TMarket );
	void __fastcall SetAPCodeCombobox( bool IsTSEC );
    int __fastcall GetAPCode( );
    int __fastcall GetLocalPort( );
    int __fastcall GetLink()         {return FLink;}
	int __fastcall GetPVC()          {return FPVC;}
	int __fastcall GetGroup()        {return FGroup;}
	int __fastcall GetThreougput();
	bool __fastcall GetIsEnabled()   {return FIsEnabled;}
	bool __fastcall GetIsOffHour()   {return FIsOffHour;}
	AnsiString __fastcall GetPWD()   {return FPWD;}
	AnsiString __fastcall GetFCM()   {return FFCM;}
    AnsiString __fastcall GetCM()    {return FCM;}
    AnsiString __fastcall GetPVCID();
    bool  __fastcall IsModified();
    bool  __fastcall IsConnectionModified();
};
//---------------------------------------------------------------------------
extern PACKAGE TPVCSettingForm *PVCSettingForm;
//---------------------------------------------------------------------------
#endif
