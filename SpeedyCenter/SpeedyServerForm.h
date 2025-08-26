//---------------------------------------------------------------------------

#ifndef SpeedyServerFormH
#define SpeedyServerFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TServerForm : public TForm
{
__published:	// IDE-managed Components
	TListView *ServerListView;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TLabeledEdit *IPLabeledEdit;
	TPopupMenu *PopupMenu;
	TMenuItem *ItemDelete;
	TImageList *OSImageList;
	TMenuItem *ItemNewGroup;
	TMenuItem *ItemModifyGroup;
	TBitBtn *NewGroupBtn;
	TTimer *AutoConnectTimer;
	void __fastcall ServerListViewClick(TObject *Sender);
	void __fastcall ServerListViewDblClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ItemDeleteClick(TObject *Sender);
	void __fastcall ServerListViewColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall ServerListViewCompare(TObject *Sender, TListItem *Item1, TListItem *Item2,
          int Data, int &Compare);
	void __fastcall ItemNewGroupClick(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);
	void __fastcall ItemModifyGroupClick(TObject *Sender);
	void __fastcall AutoConnectTimerTimer(TObject *Sender);
private:	// User declarations
	int FSortIndex;
	int FSortDir[3];
	int FServerCount;
	TStringList* FServerNames;
	TStringList* FServerIPs;
	String FHeaderStrings[3];
	bool __fastcall IsValidIP( const String& IP );
	void __fastcall OpenGroupItem( TListItem* Item );
	void __fastcall OpenItem( TListItem* Item );
	void __fastcall OpenItem( const String& IP );
	TListItem* __fastcall FindGroupItemByName( const String& Name );
	void __fastcall LoadServers( void );
public:		// User declarations
	__fastcall TServerForm(TComponent* Owner);
	void __fastcall AddServer( int OS, const AnsiString& Name, const AnsiString& IP );
	void __fastcall SaveServers( void );
	int  __fastcall GetServerCount( void );
	bool __fastcall PopServerNameIP( String& Name, String& IP );
};
//---------------------------------------------------------------------------
extern PACKAGE TServerForm *ServerForm;
//---------------------------------------------------------------------------
#endif
