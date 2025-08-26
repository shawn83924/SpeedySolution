//---------------------------------------------------------------------------

#ifndef GroupServerH
#define GroupServerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TServerGroupForm : public TForm
{
__published:	// IDE-managed Components
	TListView *GroupServerListView;
	TListView *ServerListView;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TBitBtn *AddBtn;
	TBitBtn *DelBtn;
	TLabel *Label1;
	TLabel *Label2;
	TImageList *OSImageList;
	TEdit *GroupNameEdit;
	TLabel *Label3;
	TBitBtn *UpBitBtn;
	TBitBtn *DownBitBtn;
	void __fastcall AddBtnClick(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall UpBitBtnClick(TObject *Sender);
	void __fastcall DownBitBtnClick(TObject *Sender);
	void __fastcall GroupServerListViewSelectItem(TObject *Sender, TListItem *Item,
          bool Selected);
	void __fastcall ServerListViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);

private:	// User declarations
public:		// User declarations
	__fastcall TServerGroupForm(TComponent* Owner);
	void __fastcall LoadFromListView( TListView *Servers );
	void __fastcall LoadFromListView( const String& Names, TListView *Servers );
	String __fastcall GetServersNameString( void );
	String __fastcall GetServersIPString( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TServerGroupForm *ServerGroupForm;
//---------------------------------------------------------------------------
#endif
