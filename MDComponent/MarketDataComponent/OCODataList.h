//---------------------------------------------------------------------------

#ifndef OCODataListH
#define OCODataListH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include "OCODataStore.h"
//---------------------------------------------------------------------------
class PACKAGE TOCODataList : public TCustomListView
{
private:
	void __fastcall InitString( void );
	void __fastcall InitialCol( void );
	TListColumn* __fastcall AddColField( int Field, int width = 0 );
	void __fastcall SetColWidth( TListColumn* NewCol, const String& ColName );
	void __fastcall SetColWidth( TListColumn* NewCol, const int width );
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
private:
	static String HeaderString[];
	Graphics::TBitmap* FBufferBmp;
	Graphics::TBitmap* FDeleteBmp;
	Graphics::TBitmap* FColumn0BMP;

protected:
	virtual void __fastcall Loaded(void);
	DYNAMIC void __fastcall Click(void);

public:
	__fastcall TOCODataList(TComponent* Owner);
	__fastcall ~TOCODataList(void);
	void __fastcall AddData(OCOPair* pair);
	void __fastcall RefreshSubItem(TListItem* ItemPtr, OCOPair* pair);

	void __fastcall TestFunctionForAddData(void);
__published:
    __property Font;
	__property Align;
	__property ViewStyle;
	__property Graphics::TBitmap* DeleteBMP = { read = FDeleteBmp, write = SetDeleteBmp };
};
//---------------------------------------------------------------------------
#endif
