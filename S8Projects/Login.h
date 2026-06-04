//---------------------------------------------------------------------------

#ifndef LoginH
#define LoginH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "IdExplicitTLSClientServerBase.hpp"

#include <IdHTTP.hpp>
#include "GraphButton.h"
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TLoginForm : public TForm
{
__published:	// IDE-managed Components
	TImage *BKImage;
	TActivityIndicator *ActivityIndicator;
	TEdit *IDEdit;
	TEdit *PasswordEdit;
	TCheckBox *AccountCheckBox;
	TCheckBox *SavePasswordCheckBox;
	TLabel *StatusLabel;
	TTimer *WaitTimer;
	TGraphButton *LoginButton;
	TGraphButton *ExitButton;
	TLabel *JoinMemberLabel;
	TLabel *ResetPasswordLabel;
	TLabel *SaveAccountLabel;
	TLabel *SavePasswordLabel;
	TLabel *EnvLabel;
	TLabel *VersionLabel;
	TComboBox *NetworkComboBox;
	TSpeedButton *CalendarButton;
	TCheckBox *AtAOECheckBox;
	TLabel *AtAOELabel;
	TLabel *ExLabel;
	TEdit *Edit1;
	void __fastcall LoginButtonClick(TObject *Sender);
	void __fastcall WaitTimerTimer(TObject *Sender);
	void __fastcall ExitButtonClick(TObject *Sender);
	void __fastcall LabelMouseEnter(TObject *Sender);
	void __fastcall LabelMouseLeave(TObject *Sender);
	void __fastcall SaveAccountLabelClick(TObject *Sender);
	void __fastcall SavePasswordLabelClick(TObject *Sender);
	void __fastcall JoinMemberLabelClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall IDEditEnter(TObject *Sender);
	void __fastcall PasswordEditEnter(TObject *Sender);
	void __fastcall ResetPasswordLabelClick(TObject *Sender);
	void __fastcall CalendarButtonClick(TObject *Sender);
	void __fastcall AtAOELabelClick(TObject *Sender);
private:	// User declarations
	int FWaitCount;
	String FVersion;
	bool __fastcall CheckFreeTry( void );
	void __fastcall OnNCHitTest( TMessage &Message );
	void __fastcall WndProc( TMessage &Msg );
	bool __fastcall RequestLogon(
		const String& ID,
        const String& Account,
		const String& Password);
	void __fastcall GenData( const String& ID, const String& Password, String& Out );
	bool __fastcall GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON );
	void __fastcall LoadIDPassword( void );
	void __fastcall SaveIDPassword( void );
public:		// User declarations
	__fastcall TLoginForm(TComponent* Owner);
	bool __fastcall Logon( void );
	__property String Version = {read = FVersion }; ///< OK
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginForm *LoginForm;
//---------------------------------------------------------------------------
#endif
