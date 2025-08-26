//---------------------------------------------------------------------------
// SysTrayIcon Version 1.0
//---------------------------------------------------------------------------
// Coded by Li-Yuan Chang 1998/12/15
// Copyright WayTech Development Inc.
//---------------------------------------------------------------------------
#ifndef SysTrayIconH
#define SysTrayIconH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ShellAPI.h>
#define  WM_RESETICON 0x268

typedef enum
{
    itError   = NIIF_ERROR,
    itWarning = NIIF_WARNING,
    itInfo    = NIIF_INFO
} TTipsIconType;
//---------------------------------------------------------------------------
class PACKAGE TSysTrayIcon : public TComponent
{
private:
    bool            FShowFlag;
    bool            FEnabled;    
    UINT            MSG_NOTIFY;
    TForm          *OwnerForm;
    TIcon          *FTrayIcon;
    TPopupMenu     *FPopupMenu;
	String          FIconHint;
    TNotifyEvent    FOnClick;
    TNotifyEvent    FOnRightClick;
    TNotifyEvent    FOnDoubleClick;
    TWndMethod      OrgWindowProc;
    NOTIFYICONDATAW tnd;
    void __fastcall SetTrayIcon( TIcon* Value);
    void __fastcall SetShowIcon( bool Value );
	void __fastcall SetIconHint( String Value );
    void __fastcall NewWindowProc( TMessage &Msg );
    void __fastcall FillNotifyIconData( void );
    void __fastcall IconNotify( TMessage& Msg );
    bool __fastcall AddTrayIcon( void );
    bool __fastcall DeleteTrayIcon( void );
    bool __fastcall ModifyTrayIcon( void );
    void __fastcall RightPopupMenu( void );
protected:
__published:
    __property bool         Enabled
                            = {read = FEnabled,write=FEnabled,default=true};
    __property bool         ShowIcon
                            = {read=FShowFlag,write=SetShowIcon,default=false};
    __property TIcon       *TrayIcon
                            = {read=FTrayIcon,write=SetTrayIcon};
    __property TPopupMenu  *PopupMenu
                            = {read=FPopupMenu,write=FPopupMenu,default=NULL};
	__property String       IconHint
							= {read=FIconHint,write=SetIconHint};
    __property TNotifyEvent OnClick
                            = {read=FOnClick,write=FOnClick, default=NULL};
    __property TNotifyEvent OnRightClick
                            = {read=FOnRightClick,write=FOnRightClick,default=NULL};
    __property TNotifyEvent OnDoubleClick
                            = {read=FOnDoubleClick,write=FOnDoubleClick,default=NULL};
public:
    __fastcall TSysTrayIcon(TComponent* Owner);
    __fastcall ~TSysTrayIcon( void );
    void __fastcall ChangeIcon( HICON );
    void __fastcall ShowToolTips( const String& Caption, const String& Tips, TTipsIconType Type = itInfo, int ms = 10000 );
};
//---------------------------------------------------------------------------
#endif
