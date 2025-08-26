//---------------------------------------------------------------------------

#ifndef SearchSpeedyFormH
#define SearchSpeedyFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GraphMarquee.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "SimTFXMain.h"
//---------------------------------------------------------------------------
class TSearchForm : public TForm
{
__published:	// IDE-managed Components
    TListView *ServerListView;
    TGraphMarquee *Marquee;
    TLabel *DetectLabel;
    TBitBtn *CancelBtn;
    TBitBtn *OkBtn;
    TTimer *PublishTimer;
    TTimer *TimeoutTimer;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall PublishTimerTimer(TObject *Sender);
    void __fastcall TimeoutTimerTimer(TObject *Sender);
    void __fastcall ServerListViewDblClick(TObject *Sender);
	void __fastcall CancelBtnClick(TObject *Sender);
private:	// User declarations
	AnsiString   FSelHost;
	AnsiString   FWaitHost;
	TSimTFXForm* FSimTFXForm;
	bool         FVisibleMode;
	void __fastcall WndProc( TMessage& Msg );
public:		// User declarations
	__fastcall TSearchForm(TComponent* Owner);
	void __fastcall AddHost( AnsiString& SpeedyHost, int Type );
	void __fastcall Show( TSimTFXForm* ParentForm );
	void __fastcall WaitForServer( TSimTFXForm* ParentForm, const AnsiString& SpeedyHost );
	AnsiString& GetHost(){return FSelHost;}


};
//---------------------------------------------------------------------------
extern PACKAGE TSearchForm *SearchForm;
//---------------------------------------------------------------------------
#endif
