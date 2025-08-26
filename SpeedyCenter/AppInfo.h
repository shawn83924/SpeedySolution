//---------------------------------------------------------------------------

#ifndef AppInfoH
#define AppInfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include "IniEditBox.h"
#include "LinkFrame.h"
#include "..\Migo\MTree.h"
//---------------------------------------------------------------------------
class TSimTFXForm;
//---------------------------------------------------------------------------
class TAppInfoForm : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl;
    TTabSheet *SettingSheet;
    TTabSheet *SpiderNetSheet;
    TImage *LogoImage;
    TIniEditBox *IniEditBox;
    TTabSheet *SpeedySheet;
	TImage *SpeedyImage;
    TLabel *SpeedyLabel;
    TCheckBox *HACheckBox;
    TCheckBox *MTCheckBox;
    TRadioGroup *FIXNameRadioGroup;
    TEdit *NameEdit;
    TBitBtn *OKBitBtn;
    TBitBtn *CancelBitBtn;
	TLabeledEdit *AccountEdit;
	TBitBtn *CancelAllBtn;
	TLabeledEdit *PasswordEdit;
	TGroupBox *CancelAllGroupBox;
	TGroupBox *KSGroupBox;
	TBitBtn *OnBtn;
	TLabeledEdit *KSPasswordEdit;
	TRadioButton *ByFCMRadioButton;
	TRadioButton *BySIDRadioButton;
	TComboBox *SIDComboBox;
	TComboBox *FCMComboBox;
	TComboBox *FCM2ComboBox;
	TComboBox *SFCMComboBox;
	TBitBtn *OffBtn;
	TBevel *Bevel;
	TListView *KSListView;
	TComboBox *KSTSComboBox;
	TComboBox *TSComboBox;
    void __fastcall FIXNameRadioGroupClick(TObject *Sender);
    void __fastcall OKBitBtnClick(TObject *Sender);
	void __fastcall CancelAllBtnClick(TObject *Sender);
	void __fastcall OnBtnClick(TObject *Sender);
	void __fastcall OffBtnClick(TObject *Sender);
	void __fastcall KSTSComboBoxChange(TObject *Sender);
private:	// User declarations
	int FItemIndex;
	TMarket FMarket;
	TSimTFXForm* FOwnerForm;
	void __fastcall UpdateFCMList( void );
public:		// User declarations
	__fastcall TAppInfoForm(TComponent* Owner);
	TModalResult __fastcall Show( TSimTFXForm* OF, AnsiString File );
	TModalResult __fastcall ShowSpeedy( TSimTFXForm* OF,TMarket Market );
	TModalResult __fastcall ShowSpiderNet( TSimTFXForm* OF,int Index, const AnsiString& Host );
	void __fastcall UpdateKillSwitch( TSimTFXForm* OF,MTree* Tree );
	void __fastcall SetOwner( TSimTFXForm* OwnerF ) {FOwnerForm = OwnerF;}
};
//---------------------------------------------------------------------------
extern PACKAGE TAppInfoForm *AppInfoForm;
//---------------------------------------------------------------------------
#endif
