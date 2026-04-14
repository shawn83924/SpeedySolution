//---------------------------------------------------------------------------

#ifndef OCODataListH
#define OCODataListH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include "OrderStore_OCO.h"
//---------------------------------------------------------------------------
class PACKAGE TOCODataList : public TCustomListView, public IOCOOrderStoreListener
{
private:
	TOrderStore_OCO* FOCOStore;
	std::map<AnsiString, TOCOPair*> FPairingOCO;
private:
	void __fastcall InitString( void );
	void __fastcall InitialCol( void );
	void __fastcall ResetItem( void );
	TListColumn* __fastcall AddColField( int Field );
	void __fastcall SetColWidth( TListColumn* NewCol, const int ColWidth );
	void __fastcall SetDeleteBmp( Graphics::TBitmap* deleteBmp );
	void __fastcall AdvancedCustomDraw(
		TCustomListView *Sender,
		const TRect &ARect,
		TCustomDrawStage Stage,
		bool &DefaultDraw);
	void __fastcall AdvancedCustomDrawItem(
		TCustomListView* Sender,
		TListItem* Item,
		TCustomDrawState State,
		TCustomDrawStage Stage,
		bool &DefaultDraw);
	void __fastcall SetDelBMPCanvasColor(TCanvas* canvas, bool isSelected);
	void __fastcall DrawBar( TCanvas* canvas, TRect& dRect, TColor BarColor );
	void __fastcall DrawIcon( TRect& ItemRect );
	bool __fastcall IsMouseInDeleteIcon( void );
	void __fastcall OnLeftMouseDown(int X, int Y);
	void __fastcall OnLeftMouseUp(int X, int Y);
	//need to remove
	void __fastcall DeleteItem( TListItem* ItemPtr);
	virtual AnsiString GetEx( void );
	virtual AnsiString GetSymbol( void );
	virtual void OnOCOOrderUpdate(TOCOPair* Pair, OCOUpdateType Type);
private:
	static String HeaderString[];
	static int HeaderWidth[];
	Graphics::TBitmap* FBufferBmp;
	Graphics::TBitmap* FDeleteBmp;
	Graphics::TBitmap* FColumn0BMP;
	TListItem* FMouseDownItem;
	bool FIsDownInDelImg;

protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall WndProc(TMessage& Message);
    DYNAMIC void __fastcall Edit(const TLVItem &Item);
public:
	__fastcall TOCODataList(TComponent* Owner);
	__fastcall ~TOCODataList(void);
	void __fastcall SubscribeStore(TOrderStore_OCO* OCOStore);
    void __fastcall UnSubscribeStore(void);
	void __fastcall AddData(TOCOPair* pair);
	void __fastcall EditData(TOCOPair* pair);
	void __fastcall DeleteData(TOCOPair* pair);
	void __fastcall RefreshSubItem(TListItem* ItemPtr, TOCOPair* pair);
__published:
    __property Font;
	__property Align;
	__property ViewStyle;
	__property Graphics::TBitmap* DeleteBMP = { read = FDeleteBmp, write = SetDeleteBmp };
	__property TOrderStore_OCO* OCOStore = { read = FOCOStore, write = FOCOStore };
};
//---------------------------------------------------------------------------
#endif
