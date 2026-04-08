//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "OCODataList.h"
#include "MDComponentStrings.hpp"
#pragma package(smart_init)
const int NumberOfListViewColumn = 10;
String TOCODataList::HeaderString[NumberOfListViewColumn];/* = {"刪","狀態","商品","委量一","價一","買賣一","委量二","價二","買賣二","備註"}; */
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
__fastcall TOCODataList::TOCODataList(TComponent* Owner)
	: TCustomListView(Owner)
{
	FBufferBmp = new Graphics::TBitmap();
	FDeleteBmp = new Graphics::TBitmap();
	FColumn0BMP= new Graphics::TBitmap();
	FMouseDownItem = NULL;

	ViewStyle = vsReport;
	OwnerDraw = true;

	InitString();
	InitialCol();
	OnAdvancedCustomDraw = AdvancedCustomDraw;
	OnAdvancedCustomDrawItem = AdvancedCustomDrawItem;
}
//---------------------------------------------------------------------------
__fastcall TOCODataList::~TOCODataList()
{
	delete FBufferBmp;
	delete FDeleteBmp;
    delete FColumn0BMP;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::InitString( void )
{
	HeaderString[0] = Mdcomponentstrings_MD_OCODLIST_DELETE; // 刪
	HeaderString[1] = Mdcomponentstrings_MD_OCODLIST_STATUS; // 狀態
	HeaderString[2] = Mdcomponentstrings_MD_OCODLIST_SYMBOL; // 商品
	HeaderString[3] = Mdcomponentstrings_MD_OCODLIST_QTY1;   // 委量一
	HeaderString[4] = Mdcomponentstrings_MD_OCODLIST_PRICE1; // 價一
	HeaderString[5] = Mdcomponentstrings_MD_OCODLIST_SIDE1;  // 買賣一
	HeaderString[6] = Mdcomponentstrings_MD_OCODLIST_QTY2;   // 委量二
	HeaderString[7] = Mdcomponentstrings_MD_OCODLIST_PRICE2; // 價二
	HeaderString[8] = Mdcomponentstrings_MD_OCODLIST_SIDE2;  // 買賣二
    HeaderString[9] = Mdcomponentstrings_MD_OCODLIST_NOTES;  // 備註
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::Loaded(void)
{
	TCustomListView::Loaded();
	FBufferBmp->Canvas->Font->Assign( Font );
	DeleteBMP->TransparentMode = tmAuto;
	DeleteBMP->Transparent = true;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::InitialCol( void )
{
	TListColumn* NewCol;

	Columns->BeginUpdate();
	Columns->Clear();
	for( register int i = 0; i < NumberOfListViewColumn; i++ )
		AddColField(i);

	Columns->EndUpdate();
}
//---------------------------------------------------------------------------
TListColumn* __fastcall TOCODataList::AddColField( int Field )
{
	TListColumn* NewCol;

	FBufferBmp->Canvas->Font->Assign( Font );
	NewCol = (TListColumn*)Columns->Add();
	NewCol->Caption = HeaderString[ Field ];
	NewCol->Tag = Field;
	SetColWidth( NewCol, HeaderString[Field] );
	return NewCol;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::SetColWidth( TListColumn* Col, const String& ColName )
{
	if(Col->Tag == 0)
	{
		Col->Width	  = 55;
		Col->MinWidth = 55;
		Col->MaxWidth = 55;
		return;
	}
	int curWidth = FBufferBmp->Canvas->TextWidth( ColName ) + 24;
	Col->Width = curWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::SetColWidth( TListColumn* Col, const int width )
{
	Col->Width = width;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::SetDeleteBmp(Graphics::TBitmap* deleteBmp)
{
	FDeleteBmp->Assign(deleteBmp);
	FDeleteBmp->Dormant();
	FDeleteBmp->FreeImage();
	FDeleteBmp->TransparentMode = tmAuto;
    FDeleteBmp->Transparent = true;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::AdvancedCustomDraw(
		TCustomListView *Sender,
		const TRect &ARect,
		TCustomDrawStage Stage,
		bool &DefaultDraw)
{
    if( Stage == cdPreErase )
	{
		DefaultDraw = false;
		return;
	}
	if( Stage != cdPrePaint )
		return;

	TRect PaintRect = ARect;
	if( Items->Count > 0 )
	{
		TListItem* LastItemPtr = (*(this->Items))[this->Items->Count - 1];
		TRect      ItemRect    = LastItemPtr->DisplayRect(drBounds);
		TRect      RightRect   = ARect;

		if (RightRect.Right > ItemRect.Right)
		{
			RightRect.Left = ItemRect.Right;
			Canvas->Brush->Color = Color;//clWindow;
			Canvas->FillRect( RightRect );
		}
		if (ItemRect.Bottom < PaintRect.Bottom)
		{
			PaintRect.Top = ItemRect.Bottom;
			Canvas->Brush->Color = Color;//clWindow;
			Canvas->FillRect( PaintRect );
		}
	}
	else
	{
		Canvas->Brush->Color = Color;//clWindow;
		Canvas->FillRect( PaintRect );
	}
	DefaultDraw = true;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::AdvancedCustomDrawItem(
		TCustomListView* Sender,
		TListItem* Item,
		TCustomDrawState State,
		TCustomDrawStage Stage,
		bool &DefaultDraw)
{
	DefaultDraw = false;
	TOCOPair* pair = (TOCOPair*)Item->Data;
	if(pair == NULL)
	{
		DefaultDraw = true;
		return;
	}

	TRect ItemRect = Item->DisplayRect(drBounds);
	TRect DrawRect;
	FColumn0BMP->Canvas->Font->Assign( this->Font );
	if ((FColumn0BMP->Width != ItemRect.Width()) || (FColumn0BMP->Height != ItemRect.Height()))
		FColumn0BMP->SetSize( ItemRect.Width(), ItemRect.Height() );

	SetDelBMPCanvasColor(FColumn0BMP->Canvas, Item->Selected);
	DrawRect = TRect(0, 0, FColumn0BMP->Width, FColumn0BMP->Height);
	// Fill background
	DrawBar(FColumn0BMP->Canvas, DrawRect, FColumn0BMP->Canvas->Brush->Color);
	// Draw delete icon
	DrawIcon(ItemRect);
    //Draw SubItems
	DrawRect.Left   = 0;
	DrawRect.Right  = ItemRect.Width() - 1;
	DrawRect.Top    = 0;
	DrawRect.Bottom = ItemRect.Height() - 1;
	FColumn0BMP->Canvas->Brush->Style = bsClear;

	TListColumn* PreColumn;
	TListColumn* CurColumn;
	String       SubItemText;
	for( register int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
	{
		TTextFormat Formats;

		PreColumn = Columns->Items[ColumnIndex - 1];
		CurColumn = Columns->Items[ColumnIndex];
		SubItemText    = Item->SubItems->Strings[ ColumnIndex - 1 ];
		DrawRect.left  = DrawRect.left + PreColumn->Width;
		DrawRect.Right = DrawRect.Left + CurColumn->Width;
		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FColumn0BMP->Canvas->TextRect( DrawRect, SubItemText, Formats );
	}
	///< Past Bitmap
	Canvas->Draw(ItemRect.Left, ItemRect.Top, FColumn0BMP);
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::SetDelBMPCanvasColor(TCanvas* canvas, bool isSelected)
{
	canvas->Font->Color  = clBlack;

	if(isSelected)
	{
		canvas->Brush->Color = clGray;
        return;
	}
	canvas->Brush->Color = clWhite;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::DrawBar( TCanvas* canvas, TRect& dRect, TColor BarColor )
{
	int R = GetRValue( (DWORD)BarColor );
	int G = GetGValue( (DWORD)BarColor );
	int B = GetBValue( (DWORD)BarColor );
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = R<<8;
	vert [1] .Green  = G<<8;
	vert [1] .Blue   = B<<8;
	vert [1] .Alpha  = 0x0000;

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = R<<8;
	vert [0] .Green  = G<<8;
	vert [0] .Blue   = B<<8;
	vert [0] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::DrawIcon( TRect& ItemRect )
{
	TRect DrawRect;
	DrawRect.Left   = 0;
	DrawRect.Top    = 0;
	DrawRect.Right  = ItemRect.Height();
	DrawRect.Bottom = ItemRect.Height();
	FColumn0BMP->Canvas->StretchDraw( DrawRect, FDeleteBmp);
}
//---------------------------------------------------------------------------
bool __fastcall TOCODataList::IsMouseInDeleteIcon( void )
{
	TPoint CurPosition = CalcCursorPos();
	TListItem *CurItemPtr = this->GetItemAt(CurPosition.x, CurPosition.y);
	if(CurItemPtr == NULL)
		return false;

	TRect itemRect = CurItemPtr->DisplayRect(drBounds);
	if ((CurPosition.x < itemRect.left) || (CurPosition.x >= itemRect.Height()))
		return false;

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::AddData(TOCOPair* pair)
{
	Items->BeginUpdate();
	TListItem* item = Items->Add();
	item->Data = pair;

	item->Caption = HeaderString[0];
	RefreshSubItem( item, pair);
	Items->EndUpdate();
	item->MakeVisible( false );
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::RefreshSubItem(TListItem* ItemPtr, TOCOPair* pair)
{
    if( ItemPtr->SubItems->Count > 0 )
		ItemPtr->SubItems->Clear();
	for( register int i = 1; i < NumberOfListViewColumn; i++)
		ItemPtr->SubItems->Add( pair->GetStringField(i) );
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::WndProc(TMessage& Message)
{
	switch (Message.Msg)
	{
		case WM_LBUTTONDOWN:
        {
			int X = LOWORD(Message.LParam);
			int Y = HIWORD(Message.LParam);
			OnLeftMouseDown(X,Y);

			if (FIsDownInDelImg)
				return;
			break;
		}
		case WM_LBUTTONUP:
		{
			int X = LOWORD(Message.LParam);
			int Y = HIWORD(Message.LParam);
			OnLeftMouseUp(X,Y);
			break;
		}
	}
	TCustomListView::WndProc(Message);
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::Edit(const TLVItem &Item)
{
	// Do nothing — suppress inline editing entirely
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::OnLeftMouseDown(int X, int Y)// set left down Item
{
	TListItem* CurItemPtr = this->GetItemAt(X, Y);
	if(CurItemPtr == NULL)
	{
		FMouseDownItem = NULL;
		FIsDownInDelImg = false;
		return;
	}

	FMouseDownItem = CurItemPtr;
	FIsDownInDelImg = IsMouseInDeleteIcon();
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::OnLeftMouseUp(int X, int Y)
{
	TListItem* CurItemPtr = this->GetItemAt(X, Y);
	if(CurItemPtr == NULL)
		return;

	if(FIsDownInDelImg && (CurItemPtr == FMouseDownItem) && IsMouseInDeleteIcon())
    {
		DeleteItem(CurItemPtr);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::DeleteItem(TListItem* ItemPtr)
{
    if(ItemPtr == NULL)
        return;

	TOCOPair* pair = (TOCOPair*)ItemPtr->Data;
    if(pair != NULL)
        delete pair;

	Items->BeginUpdate();
    ItemPtr->Data = NULL;
    ItemPtr->Delete();
	Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TOCODataList::TestFunctionForAddData(void)
{
	for( int i = 0; i < 10 ; i++ )
	{
		TOCOPair* pair = new TOCOPair();
		pair->OrderQty1 = i;
		pair->OrderQty2 = i+1;
		pair->ConditionPrice1 = i + 200;
		pair->ConditionPrice2 = i + 400;
		pair->OrderSide1 = nsOrderMessageDefine::sBuy;
		pair->OrderSide1 = nsOrderMessageDefine::sSell;
		pair->Symbol = L"2330";

        AddData(pair);
	}
}
//---------------------------------------------------------------------------
namespace Ocodatalist
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOCODataList)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TOCODataList *)
{
	new TOCODataList(NULL);
}
