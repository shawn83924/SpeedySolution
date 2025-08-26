//---------------------------------------------------------------------------

#ifndef MDIMainH
#define MDIMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include "SysTrayIcon.h"
#include <Tabs.hpp>
#include <Menus.hpp>
#include <XPMan.hpp>
#include <AppEvnts.hpp>
#include <ExtCtrls.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TMDIForm : public TForm
{
__published:	// IDE-managed Components
	TImageList *ToolImageList;
	TImageList *GrayToolImageList;
	TToolBar *ToolBar;
	TToolButton *LogonButton;
	TToolButton *LogoutButton;
	TToolButton *SettingButton;
	TToolButton *ChangePasswdButton;
	TToolButton *SeparatorButton1;
	TToolButton *EventLogToolButton;
	TToolButton *AboutButton;
	TToolButton *SeparatorButton3;
	TToolButton *MinimizeToolButton;
	TToolButton *NewWinButton;
	TSysTrayIcon *SysTrayIcon;
	TTabSet *ServerTabSet;
	TImageList *ImageList;
	TToolButton *CascadeButton;
	TPopupMenu *TrayPopupMenu;
	TMenuItem *RestoreMenuItem;
	TMenuItem *EventLogMenuItem;
	TMenuItem *N12;
	TMenuItem *AboutMenuItem;
	TXPManifest *XPManifest;
	TApplicationEvents *ApplicationEvents;
	TToolButton *TileToolButton;
	TToolButton *SeparatorButton;
	TPopupMenu *TabPopupMenu;
	TMenuItem *CloseMenuItem;
	TTimer *FormShowTimer;
	TToolButton *ClockButton;
	TMenuItem *CascadeMenuItem;
	TToolButton *CheckSystemToolButton;
	TMenuItem *CloseAllMenuItem;
	void __fastcall LogonButtonClick(TObject *Sender);
	void __fastcall NewFormToolButtonClick(TObject *Sender);
	void __fastcall SysTrayIconDoubleClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall AboutToolButtonClick(TObject *Sender);
	void __fastcall SettingButtonClick(TObject *Sender);
	void __fastcall LogoutButtonClick(TObject *Sender);
	void __fastcall ServerTabSetChange(TObject *Sender, int NewTab,
          bool &AllowChange);
	void __fastcall ChangePasswdButtonClick(TObject *Sender);
	void __fastcall ServerTabSetGetImageIndex(TObject *Sender, int TabIndex,
          int &ImageIndex);
	void __fastcall CascadeButtonClick(TObject *Sender);
	void __fastcall ApplicationEventsMinimize(TObject *Sender);
	void __fastcall EventLogToolButtonClick(TObject *Sender);
	void __fastcall TileToolButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ServerTabSetMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall CloseMenuItemClick(TObject *Sender);
	void __fastcall FormShowTimerTimer(TObject *Sender);
	void __fastcall CheckSystemToolButtonClick(TObject *Sender);
	void __fastcall CloseAllMenuItemClick(TObject *Sender);

private:	// User declarations
	LCID FSystemDefaultLCID;
	bool FCloseAll;
	void __fastcall LoadLanguage( void );
	void __fastcall SaveLanguage( void );
	void __fastcall LoadPosition( void );
	void __fastcall SavePosition( void );
	void __fastcall UpdateCaption( void );
	void __fastcall LoadAutoLogon( void );
private:
	void __fastcall MDILogon(TObject *Sender);
	void __fastcall MDILogonFailed(TObject *Sender);
	void __fastcall MDIAdminLogon(TObject *Sender);
	void __fastcall MDILogoff(TObject *Sender);
	void __fastcall MDIMaxinum(TObject *Sender);
	void __fastcall MDIClose(TObject *Sender);
	void __fastcall MDIActive(TObject *Sender);
	void __fastcall MDITimer(TObject *Sender);
public:		// User declarations
	__fastcall TMDIForm(TComponent* Owner);
	void __fastcall StopCloseAll( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TMDIForm *MDIForm;
//---------------------------------------------------------------------------
#endif
