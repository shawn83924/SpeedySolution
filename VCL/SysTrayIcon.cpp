//---------------------------------------------------------------------------
// SysTrayIcon Version 1.0
//---------------------------------------------------------------------------
// Coded by Li-Yuan Chang 1998/12/15
// Copyright WayTech Development Inc.
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SysTrayIcon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TSysTrayIcon *)
{
     new TSysTrayIcon(NULL);
}
//---------------------------------------------------------------------------
//    Constructor of TSysTrayIcon
//---------------------------------------------------------------------------
__fastcall TSysTrayIcon::TSysTrayIcon(TComponent* Owner)
    : TComponent(Owner)
{
    if((OwnerForm = dynamic_cast< TForm* >( Owner ))==0 )
      throw Exception( "The owner of a TSysTrayIcon must be a form" );
    // Register a message for Tray Icon's callback
    MSG_NOTIFY = RegisterWindowMessageA("Yuan's Tray Icon Component");
    FTrayIcon = new TIcon();
    // Hook the default window proc
    OrgWindowProc = OwnerForm->WindowProc;
    OwnerForm->WindowProc = NewWindowProc;
    if( !ComponentState.Contains(csDesigning)&&FShowFlag ) AddTrayIcon();
}

//---------------------------------------------------------------------------
//    Destructor of TViewPort
//---------------------------------------------------------------------------
__fastcall TSysTrayIcon::~TSysTrayIcon( void )
{
    OwnerForm->WindowProc = OrgWindowProc;
    if( !ComponentState.Contains(csDesigning)&&FShowFlag ) DeleteTrayIcon();
    delete FTrayIcon;
}
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::ShowToolTips( const String& Title, const String& Tips, TTipsIconType Type, int ms )
{
    int TitleLen, TipsLen;
	NOTIFYICONDATAW IconData = {0};

    IconData.cbSize = sizeof( IconData );
    IconData.hWnd   = OwnerForm->Handle;
	IconData.uFlags = NIF_INFO;
	IconData.dwInfoFlags = Type;
	IconData.uTimeout    = ms; ///< in milliseconds
	wcsncpy_s( IconData.szInfoTitle, 64,  Title.c_str(), Title.Length() );
	wcsncpy_s( IconData.szInfo, 256, Tips.c_str(), Tips.Length() );
	Shell_NotifyIconW( NIM_MODIFY, &IconData);
}
//---------------------------------------------------------------------------
//    New Window Proc handleing the System Tray call back message
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::NewWindowProc( TMessage &Msg)
{
    if( Msg.Msg == MSG_NOTIFY ) IconNotify( Msg );
    else                        OrgWindowProc( Msg );
}
//---------------------------------------------------------------------------
//    Fill the  NOTIFYICONDATA data structure
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::FillNotifyIconData( void )
{
    int HintLen;
    tnd.cbSize           = sizeof(NOTIFYICONDATA);
    tnd.hWnd             = OwnerForm->Handle;
    tnd.uID              = 0;
    tnd.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnd.uCallbackMessage = MSG_NOTIFY;
	tnd.hIcon            = FTrayIcon->Handle;
	wcsncpy_s(tnd.szTip, 64, FIconHint.c_str(), FIconHint.Length() );
}

//---------------------------------------------------------------------------
//    Add a Icon into the Tray Area
//---------------------------------------------------------------------------
bool __fastcall TSysTrayIcon::AddTrayIcon( void )
{
    bool Rtn;

    FillNotifyIconData( );
    Rtn = Shell_NotifyIconW( NIM_ADD, &tnd);
    // For some reason, if there is no tool tip set up, then the icon
    // doesn't display.  This fixes that.
    if( FIconHint == "" )
      PostMessage( OwnerForm->Handle, MSG_NOTIFY, 0, WM_RESETICON );
    return Rtn;
}
//---------------------------------------------------------------------------
//    Modify the Tray Icon's properties
//---------------------------------------------------------------------------
bool __fastcall TSysTrayIcon::ModifyTrayIcon( void )
{
    FillNotifyIconData();
	return Shell_NotifyIconW( NIM_MODIFY, &tnd);
}
//---------------------------------------------------------------------------
//    Delete the Tray Icon
//---------------------------------------------------------------------------
bool __fastcall TSysTrayIcon::DeleteTrayIcon( void )
{
	return Shell_NotifyIconW( NIM_DELETE, &tnd);
}
//---------------------------------------------------------------------------
//    When right mouse click popup the menu
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::RightPopupMenu( void )
{
    POINT MousePos;

    if( FEnabled )
    {
        GetCursorPos( &MousePos );
        FPopupMenu->PopupComponent = OwnerForm;
        SetForegroundWindow( OwnerForm->Handle );
        FPopupMenu->Popup( MousePos.x, MousePos.y );
    }
}
//---------------------------------------------------------------------------
//    Handle the call back message
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::IconNotify(TMessage& Msg)
{
    switch(Msg.LParam)
    {
        case WM_RBUTTONUP:     if( FEnabled && FPopupMenu!=NULL)      RightPopupMenu( );
                               if( FEnabled && FOnRightClick!=NULL )  FOnRightClick(OwnerForm);
                               break;
        case WM_LBUTTONDBLCLK: if( FEnabled && FOnDoubleClick!=NULL )
                                   FOnDoubleClick(OwnerForm);
                               break;
        case WM_LBUTTONUP:     if( FEnabled && FOnClick!=NULL )
                                   FOnClick(OwnerForm);
                               break;
        case WM_RESETICON:     ModifyTrayIcon( );
                               break;
        default:               break;
    }
}
//---------------------------------------------------------------------------
//     Public Method let programmer to change the icon in the tray area
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::ChangeIcon( HICON NewIcon )
{
    FTrayIcon->ReleaseHandle();
    FTrayIcon->Handle = NewIcon;
    if( !ComponentState.Contains(csDesigning)  && FShowFlag )
         ModifyTrayIcon();
}
//---------------------------------------------------------------------------
//     Set the tray icon
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::SetTrayIcon( TIcon* Value)
{
    if( Value != NULL )
    {
      FTrayIcon->Assign( Value );
      if( !ComponentState.Contains(csDesigning)  && FShowFlag )
         ModifyTrayIcon();
    }
}
//---------------------------------------------------------------------------
//     Show or hide the tray icon
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::SetShowIcon( bool Value)
{
    FShowFlag = Value;
    if( !ComponentState.Contains(csDesigning) )
    {
       if( FShowFlag )      AddTrayIcon();
       else                 DeleteTrayIcon();
    }
}
//---------------------------------------------------------------------------
//     Set the tray icon's hint
//---------------------------------------------------------------------------
void __fastcall TSysTrayIcon::SetIconHint( String Value )
{
	FIconHint = Value;
	if( !ComponentState.Contains(csDesigning)&&FShowFlag ) ModifyTrayIcon();
}
//---------------------------------------------------------------------------
//     Register this component to C++ Builder
//---------------------------------------------------------------------------
namespace Systrayicon
{
    void __fastcall PACKAGE Register()
    {
        TComponentClass classes[1] = {__classid(TSysTrayIcon)};
        RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
//  End of file:
//---------------------------------------------------------------------------
