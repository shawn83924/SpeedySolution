//---------------------------------------------------------------------------

#ifndef WebBrowserFormH
#define WebBrowserFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <SHDocVw.hpp>
#include <Vcl.OleCtrls.hpp>
//---------------------------------------------------------------------------
class TBrowserForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TWebBrowser *WebBrowser;
	void __fastcall WebBrowserNavigateComplete2(TObject *ASender, IDispatch * const pDisp,
          const OleVariant &URL);
	void __fastcall WebBrowserBeforeNavigate2(TObject *ASender, IDispatch * const pDisp,
          const OleVariant &URL, const OleVariant &Flags, const OleVariant &TargetFrameName,
          const OleVariant &PostData, const OleVariant &Headers,
		  WordBool &Cancel);
private:	// User declarations
	bool FLoading;
public:		// User declarations
	__fastcall TBrowserForm(TComponent* Owner);
	bool __fastcall WaitLoading( void );
	String LoginID;
};
//---------------------------------------------------------------------------
extern PACKAGE TBrowserForm *BrowserForm;
//---------------------------------------------------------------------------
#endif
