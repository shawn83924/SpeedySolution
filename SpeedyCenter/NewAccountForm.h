//---------------------------------------------------------------------------

#ifndef NewAccountFormH
#define NewAccountFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include "..\UFC\iniFile.h"
#include "SimTFXMain.h"
#include <ComCtrls.hpp>
#include "cspin.h"
//---------------------------------------------------------------------------
class TAccountForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *PwdPanel;
    TLabel *Label2;
    TLabel *Label3;
    TMaskEdit *PwdConfirmEdit;
    TMaskEdit *PwdEdit;
    TPanel *IDPanel;
    TLabel *Label1;
    TEdit *IDEdit;
    TPanel *BtnPanel;
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
    TPanel *AttrPanel;
	TPanel *AccountPanel;
	TListView *AccountListView;
	TButton *AddAccountButton;
	TButton *DeleteAccountButton;
	TCheckBox *FUTBrokerIDCheckBox;
	TEdit *FUTBrokerIDEdit;
	TEdit *TSEBrokerIDEdit;
	TCheckBox *TSEBrokerIDCheckBox;
	TLabel *TAIFEXAccLabel;
	TLabel *Label6;
	TComboBox *CheckerComboBox;
	TLabel *Label9;
	TComboBox *DefaultCheckerComboBox;
	TRadioGroup *OIDGenGroup;
	TEdit *BeginOIDEdit;
	TEdit *EndOIDEdit;
	TEdit *TerminalsEdit;
	TBitBtn *EditBtn;
	TStaticText *StaticText1;
	TPageControl *PageControl;
	TTabSheet *TFXTabSheet;
	TTabSheet *TSETabSheet;
	TRadioGroup *TSEOIDGenGroup;
	TBitBtn *TSEEditBtn;
	TEdit *TSETerminalsEdit;
	TEdit *TSEEndOIDEdit;
	TStaticText *StaticText2;
	TEdit *TSEBeginOIDEdit;
	TComboBox *TSECheckerComboBox;
	TLabel *Label7;
	TLabel *Label10;
	TComboBox *TSEDefaultCheckerComboBox;
	TLabel *Label11;
	TComboBox *ComboBoxGroup;
	TPanel *SrcPanel;
	TEdit *IPEdit;
	TCheckBox *IPCheckBox;
	TComboBox *GroupComboBox;
	TLabel *Label4;
	TComboBox *SourceComboBox;
	TLabel *Label5;
	TTabSheet *VersionSheet;
	TLabel *Label12;
	TListBox *VerListBox;
	TButton *AddVerButton;
	TButton *DelVerButton;
	TBitBtn *BitBtn3;
	TLabel *Label13;
	TTabSheet *WarningSheet;
	TLabel *Label14;
	TCSpinEdit *Level1Spin;
	TLabel *Label15;
	TLabel *Label16;
	TCSpinEdit *Level2Spin;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *VersionLabel;
	TLabel *BuildLabel;
	TComboBox *TermsComboBox;
	TTabSheet *PATSTabSheet;
	TLabel *Label20;
	TComboBox *PATSCheckerComboBox;
	TLabel *Label21;
	TComboBox *PATSDefaultCheckerComboBox;
	TRadioGroup *PATSOIDGroup;
	TEdit *PATSBeginOIDEdit;
	TStaticText *StaticText3;
	TEdit *PATSEndOIDEdit;
	TEdit *PATSOIDPrefixEdit;
	TLabel *Label22;
	TLabel *Label23;
	TListBox *AccListBox;
	TButton *AddAccButton;
	TButton *DelAccButton;
	TLabel *Label24;
	TComboBox *OrdSrcComboBox;
	TLabel *Label25;
	TEdit *InfoSrcEdit;
	TPageControl *AccPageControl;
	TTabSheet *TAIFEXTabSheet;
	TTabSheet *TWSETabSheet;
	TListView *TWSEAccountListView;
	TLabel *TWSEAccLabel;
	TButton *DelTWSEAccountButton;
	TButton *AddTWSEAccountButton;
	TTabSheet *TFXOffHourTabSheet;
	TComboBox *OHTermsComboBox;
	TBitBtn *BitBtn1;
	TBitBtn *OHTermEditBtn;
	TEdit *OHTerminalsEdit;
	TStaticText *StaticText4;
	TEdit *OHBeginOIDEdit;
	TEdit *OHEndOIDEdit;
	TRadioGroup *OHOIDGenGroup;
	TCheckBox *OffHourCheckBox;
	TComboBox *TSETermsComboBox;
	TLabel *Label8;
	TCSpinEdit *ThroughputSpinEdit;
	TLabel *Label26;
	TCheckBox *ModifyPasswordCheckBox;
	TEdit *UsersEdit;
	TBitBtn *BitBtn2;
	TLabel *UserLabel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall IPCheckBoxClick(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall AddAccountButtonClick(TObject *Sender);
    void __fastcall DeleteAccountButtonClick(TObject *Sender);
	void __fastcall GroupComboBoxChange(TObject *Sender);
	void __fastcall FUTBrokerIDCheckBoxClick(TObject *Sender);
	void __fastcall TSEBrokerIDCheckBoxClick(TObject *Sender);
	void __fastcall OIDGenGroupClick(TObject *Sender);
	void __fastcall TFXEditBtnClick(TObject *Sender);
	void __fastcall TSEOIDGenGroupClick(TObject *Sender);
	void __fastcall TSEEditBtnClick(TObject *Sender);
	void __fastcall BitBtn3Click(TObject *Sender);
	void __fastcall AddVerButtonClick(TObject *Sender);
	void __fastcall DelVerButtonClick(TObject *Sender);
	void __fastcall Level1SpinChange(TObject *Sender);
	void __fastcall TermsComboBoxChange(TObject *Sender);
	void __fastcall PATSOIDGroupClick(TObject *Sender);
	void __fastcall AddAccButtonClick(TObject *Sender);
	void __fastcall DelAccButtonClick(TObject *Sender);
	void __fastcall NewTWSEAccountButtonClick(TObject *Sender);
	void __fastcall DelTWSEAccountButtonClick(TObject *Sender);
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall OHTermsComboBoxChange(TObject *Sender);
	void __fastcall OHTermEditBtnClick(TObject *Sender);
	void __fastcall OHOIDGenGroupClick(TObject *Sender);
	void __fastcall OffHourCheckBoxClick(TObject *Sender);
	void __fastcall TSETermsComboBoxChange(TObject *Sender);


private:	// User declarations
	TSimTFXForm* FParentForm;
    bool         FUse62CarrySeq;
	int __fastcall TypeToIndex( const UFC::AnsiString& Type );
	void __fastcall GetType( UFC::AnsiString& Type );
	void __fastcall SetOIDGenType( int Type, bool IsTFX );
	void __fastcall SetPATSOIDGenType( int Type );
	void __fastcall InitOrderIDProperties( UFC::Section* iniSection );
	void __fastcall InitOHOrderIDProperties( UFC::Section* iniSection );
	void __fastcall InitBrokerIDProperties( UFC::Section* iniSection );
	void __fastcall InitCheckerProperties( UFC::Section* iniSection );
	void __fastcall InitOtherProperties( UFC::Section* iniSection );
	bool __fastcall CheckOIDOverlap( const UFC::AnsiString& CheckAE,int TFXBeginInt, int TFXEndInt, int TermDigi );
public:		// User declarations
	__fastcall TAccountForm(TComponent* Owner);
	///< For normal user
	__fastcall TAccountForm(TComponent* Owner,  UFC::Section* iniSection,TStrings* Checker,TStrings* Source, bool ModifyAttr );
	///< For shared user
	__fastcall TAccountForm(TComponent* Owner, UFC::Section* iniSection, UFC::Section* OHiniSection, TStrings* Checkers,TStrings* Sources );
    void __fastcall UpdateNewAccount( UFC::UiniFile* ini );
    void __fastcall UpdateModifyAccount( UFC::UiniFile* ini );    
    void __fastcall UpdateChangePasswd( UFC::UiniFile* ini );
};
//---------------------------------------------------------------------------
extern PACKAGE TAccountForm *AccountForm;
//---------------------------------------------------------------------------
#endif
