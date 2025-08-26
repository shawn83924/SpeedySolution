//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.hpp>
#include "PlayList.h"
#pragma hdrstop

#include "PurchaseList.h"
#pragma package(smart_init)
const ICON_INDEX ICON_DIR = 0;
const ICON_INDEX ICON_NOFILE = 1;
const ICON_INDEX ICON_EXPAND = 2;
const ICON_INDEX ICON_FILE = 3;
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPurchaseList *)
{
    new TPurchaseList(NULL);
}
//---------------------------------------------------------------------------
namespace Purchaselist
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TPurchaseList)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
int __fastcall CompText(void* Item1, void* Item2)
{
    return CompareText( ((CNode*)Item1)->FName, ((CNode*)Item2)->FName );
}
//---------------------------------------------------------------------------
int __fastcall CompNodeType(void* Item1, void* Item2)
{
    return CompareValue( ((CNode*)Item1)->FIcon, ((CNode*)Item2)->FIcon );
}
//---------------------------------------------------------------------------
//
//  CNode
//
//---------------------------------------------------------------------------
__fastcall CNode::CNode(const TSearchRec& SearchRec, const AnsiString& ParentPath, int Index)
:FSubNodes(NULL)
,FIcon(ICON_DIR)
,FExpand(false)
,FIndex(Index)
,FTopNode(true)
,FParent(NULL)
,FSize(0)
{
    FName = SearchRec.Name;
    //FDateTime = FileDateToDateTime(SearchRec.Time);
    FFilePath = ParentPath + FName;
    GetMediaLength();
}
//---------------------------------------------------------------------------
__fastcall CNode::~CNode(void)
{
    if( FSubNodes != NULL )
    {
        ClearSubNodes();
        delete FSubNodes;
    }
}
//---------------------------------------------------------------------------
void __fastcall CNode::GetMediaLength(void)
{
    TPlayList* plFile = new TPlayList(NULL);
    plFile->Open( FFilePath );
    if( plFile->FileCount() == 1 )
    {
        CPLFile* fFile = plFile->GetFile(0);
        FMediaLength = fFile->FMediaLength;
        FSize = fFile->FSize;
    }
    delete plFile;
}
//---------------------------------------------------------------------------
void __fastcall CNode::BuildSubNodeList(void)
{
    if( FSubNodes != NULL )
        ClearSubNodes();
    else
        FSubNodes = new TList;

    TSearchRec SearchRec;
    int SearchAttr = faAnyFile;
    SearchAttr &= ~faDirectory;
    AnsiString Path = FFilePath + "\\";

    if( FindFirst(Path + "*", SearchAttr, SearchRec) == 0 )
    {
        CNode* Node = NULL;
        int Index = 0;
        AnsiString Ex;
        do
        {
            if( (SearchRec.Attr & SearchAttr) == SearchRec.Attr )
            {
                Ex = SearchRec.Name.SubString( SearchRec.Name.Length()-3, 4 );
                if( stricmp(Ex.c_str(), ".mpx") != 0 && stricmp(Ex.c_str(), ".mp3") != 0 )
                {
                    delete Node;
                    Node = NULL;
                    continue;
                }
                if( (Node = new CNode(SearchRec, Path, Index)) != NULL )
                {
                    Node->FIcon = ICON_FILE;
                    Node->FTopNode = false;
                    Node->FParent = this;
                    Node->GetFileTimeString();
                    FSubNodes->Add( (void*)Node );
                    ++Index;
                }
            }
        } while( FindNext(SearchRec) == 0 );
        FindClose( SearchRec );
    }
    if( FSubNodes->Count == 0 )
        FIcon = ICON_NOFILE;
    else if( FSubNodes->Count > 1 )
        FSubNodes->Sort( CompText );
}
//---------------------------------------------------------------------------
void __fastcall CNode::ClearSubNodes(void)
{
    CNode* SubNode = NULL;
    if( FSubNodes->Count > 0 )
        FIcon = ICON_DIR;
    for( int i = FSubNodes->Count - 1; i >= 0; --i )
    {
        if( (SubNode = (CNode*)FSubNodes->Items[i]) != NULL )
        {
            delete SubNode;
            SubNode = NULL;
            FSubNodes->Delete(i);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CNode::GetFileTimeString(void)
{
    FILETIME ft,localft;
    SYSTEMTIME st;
    TFileStream* fStream = new TFileStream( FFilePath, fmOpenRead );
    GetFileTime( (HANDLE)(fStream->Handle), &ft, NULL, NULL );
    delete fStream;
    FileTimeToLocalFileTime( &ft, &localft );
    FileTimeToSystemTime( &localft, &st );
    FDateTime.printf( "%04d/%02d/%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
    //FDateTime = FileDateToDateTime(SearchRec.Time);
}
//---------------------------------------------------------------------------
//
//  TPurchaseList
//
//---------------------------------------------------------------------------
__fastcall TPurchaseList::TPurchaseList(TComponent* Owner)
:TListView(Owner)
,FLastItem(NULL)
,FRoot("c:\\")
{
    InitControl();
}
//---------------------------------------------------------------------------
__fastcall TPurchaseList::~TPurchaseList(void)
{
    if( FNodes != NULL )
    {
        FNodes->Clear();
        delete FNodes;         
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::InitControl(void)
{
    TListColumn* pColumn = NULL;
    pColumn              = Columns->Add();
    pColumn->Caption     = "名稱";
    pColumn              = Columns->Add();
    pColumn->Caption     = "播放時間";
    pColumn              = Columns->Add();
    pColumn->Caption     = "購買日期";

    ViewStyle            = vsReport;
    ReadOnly             = true;
    OnCustomDrawItem     = DoCustomDrawItem;
    OnDblClick           = DoDblClick;
    OnMouseDown          = DoMouseDown;
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::Loaded(void)
{
    int ColumnWidth = (Width-4) / 3;
    Column[0]->Width = ColumnWidth;
    Column[1]->Width = ColumnWidth;
    Column[2]->Width = ColumnWidth;

    SetRoot( FRoot );
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::SetRoot(AnsiString RootPath)
{
    if( RootPath.Length() > 0 )
    {
        FRoot = RootPath;

        if( FRoot[ FRoot.Length() ] != '\\' )
            FRoot += "\\";

        ResetTopNodes();
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::ResetTopNodes(void)
{
    ClearListView();
    BuildTopNodeList();
    AddTopNodeIntoListView();
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::ClearListView(void)
{
    if( FNodes != NULL )
    {
        FNodes->Clear();
        Items->Clear();
    }
    else
    {
        FNodes = new TList;
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::BuildTopNodeList(void)
{
    TSearchRec SearchRec;
    int SearchAttr = faAnyFile;
    SearchAttr &= ~faHidden;
    SearchAttr &= ~faSysFile;

    if( FindFirst(FRoot + "*", SearchAttr, SearchRec) == 0 )
    {
        CNode* Node = NULL;
        int Index = 0;
        AnsiString Ex;
        do
        {
            if( (SearchRec.Attr & SearchAttr) == SearchRec.Attr )
            {
                if( (Node = new CNode(SearchRec, FRoot, Index)) != NULL )
                {
                    if( Node->FName != "." && Node->FName != ".." )
                    {
                        if( (SearchRec.Attr & faDirectory) == 0 )
                        {
                            Ex = ExtractFileExt(SearchRec.Name);//SearchRec.Name.SubString( SearchRec.Name.Length()-3, 4 );
                            if( stricmp(Ex.c_str(),".mpx") != 0 )
                            {
                                delete Node;
                                Node = NULL;
                                continue;
                            }
                            Node->FIcon = ICON_FILE;
                            Node->GetFileTimeString();
                        }
                        FNodes->Add( (void*)Node );
                        ++Index;
                    }
                }
            }

        } while( FindNext(SearchRec) == 0 );
        FindClose( SearchRec );
    }
    if( FNodes->Count > 1 )
    {
        FNodes->Sort( CompNodeType );
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::AddTopNodeIntoListView(void)
{
    TListItem* NewItem = NULL;
    CNode* Node = NULL;
    int NodeCount = FNodes->Count;

    for( int i = 0; i < NodeCount; ++i )
    {
        if( (Node = (CNode*)FNodes->Items[i]) != NULL )
        {
            NewItem = Items->Add();
            NewItem->Data = (void*)Node;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::DoCustomDrawItem(
      TCustomListView *Sender, TListItem *Item, TCustomDrawState State,
      bool &DefaultDraw)
{
    Graphics::TBitmap * Buffer = new Graphics::TBitmap();
    TRect ItemRect = Item->DisplayRect( drBounds );

    Buffer->Width  = ItemRect.Width();
    Buffer->Height = ItemRect.Height();
    Buffer->Canvas->Font->Assign( Font );

    if( Item->Selected == true )
    {
        if( State.Contains( cdsFocused ) )
        {
            Buffer->Canvas->Pen->Color   = clNavy;
            Buffer->Canvas->Brush->Color = clNavy;
        }
        else
        {
            Buffer->Canvas->Pen->Color   = clNavy;
            Buffer->Canvas->Brush->Color = clNavy;
        }
        Buffer->Canvas->Font->Color  = clWhite;
    }
    else
    {
        Buffer->Canvas->Pen->Color   = clWhite;
        Buffer->Canvas->Brush->Color = clWhite;
        Buffer->Canvas->Font->Color  = clBlack;
    }
    //Buffer->Canvas->FillRect( Rect(0,0,Buffer->Width,Buffer->Height) );
    /*
    if( Font->Size > 2 )
        Buffer->Canvas->Font->Size = Font->Size - 2;
        */
    
    DrawCaption( Buffer->Canvas, Item );
    Buffer->Canvas->Pen->Color   = clWhite;
    Buffer->Canvas->Brush->Color = clWhite;
    Buffer->Canvas->Font->Color  = clBlack;
    DoCustomDrawSubItem( Sender, Buffer->Canvas, Item );
    Sender->Canvas->Draw( 0, ItemRect.Top, Buffer );

    delete Buffer;
    DefaultDraw = false;
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::DrawCaption(TCanvas * canvas, TListItem* Item)
{
    TRect Rect = Item->DisplayRect( drLabel );
    Rect.Bottom = Rect.Height();
    Rect.Top = 0;
    AnsiString str1;
    AnsiString str2;
    CNode* Node = (CNode*)Item->Data;
    ICON_INDEX Icon = ICON_DIR;
    if( Node != NULL )
    {
        Icon = Node->FIcon;
        if( Node->FTopNode == true )
            Node->FIndex = Item->Index;
        else
            Rect.Left += 20;
    }
    if( SmallImages != NULL )
        SmallImages->Draw( canvas, Rect.Left - SmallImages->Width - 1, 0, Icon, true );

    str1 = ((CNode*)Item->Data)->FName;
    str2 = ExtractFileExt( str1 );

    Item->Caption = str1.SubString( 1, str1.Length() - str2.Length() );

    DrawTextEx( canvas->Handle, Item->Caption.c_str(), Item->Caption.Length(), &Rect, DT_LEFT|DT_END_ELLIPSIS, NULL );
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::DoCustomDrawSubItem(TCustomListView* Sender, TCanvas* canvas, TListItem* Item)
{
    AnsiString str = ((CNode*)Item->Data)->FMediaLength;
    DrawSubItem( canvas, 1, str, DT_LEFT|DT_END_ELLIPSIS );
    str = ((CNode*)Item->Data)->FDateTime;
    DrawSubItem( canvas, 2, str, DT_LEFT|DT_END_ELLIPSIS );
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::DrawSubItem( TCanvas* canvas, int Index, const AnsiString& DrawString, UINT TextFormat )
{
    TRect Rect;
    ListView_GetSubItemRect( Handle, 0, Index, LVIR_BOUNDS, &Rect );
    Rect.Bottom = Rect.Height();
    Rect.Top = 0;
    Rect.Left += 5;
    Rect.Right -= 5;
    DrawTextEx( canvas->Handle, DrawString.c_str(), DrawString.Length(), &Rect, TextFormat, NULL );
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::DoDblClick(TObject *Sender)
{
    if( FLastItem != NULL )
    {
        CNode* Node = (CNode*)Items->Item[FLastItem->Index]->Data;
        if( Node != NULL && Node->FIcon != ICON_FILE )
        {
            if( Node->FExpand == false )
            {
                Node->BuildSubNodeList();
                if( Node->FSubNodes->Count > 0 )
                    LoadFiles( Node );
            }
            else
                RemoveFiles( Node );
        }
        FLastItem = NULL;
    }
    this->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::LoadFiles(CNode* Node)
{
    int AddIndex = Node->FIndex + 1;
    int NodeCount = Node->FSubNodes->Count;
    TListItem* NewItem = NULL;
    CNode* SubNode = NULL;

    for( int i = 0; i < NodeCount; ++i )
    {
        if( (SubNode = (CNode*)(Node->FSubNodes->Items[i])) != NULL )
        {
            NewItem = Items->Insert( AddIndex + i );
            NewItem->Data = (void*)SubNode;
            SubNode = NULL;
        }
    }

    if( Node->FSubNodes->Count > 0 )
    {
        Node->FExpand = true;
        Node->FIcon = ICON_EXPAND;
    }
}
//---------------------------------------------------------------------------
void __fastcall TPurchaseList::RemoveFiles(CNode* Node)
{
    int DelIndex = Node->FIndex + 1;

    for( int i = Node->FSubNodes->Count - 1; i >= 0; --i )
    {
        Items->Delete( DelIndex + i );
    }
    Node->ClearSubNodes();
    Node->FExpand = false;
}
//---------------------------------------------------------------------------
ICON_INDEX __fastcall TPurchaseList::GetSelectedType(void)
{
    TListItem* SelItem = this->Selected;
    if( SelItem != NULL )
    {
        CNode* SelNode = (CNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            return SelNode->FIcon;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPurchaseList::GetSelectedPath(void)
{
    TListItem* SelItem = this->Selected;
    AnsiString FilePath = "";
    if( SelItem != NULL )
    {
        CNode* SelNode = (CNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            FilePath = SelNode->FFilePath;
        }
    }
    return FilePath;
}
//---------------------------------------------------------------------------
ICON_INDEX __fastcall TPurchaseList::GetItemType(int Index)
{
    TListItem* SelItem = this->Items->Item[Index];
    if( SelItem != NULL )
    {
        CNode* SelNode = (CNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            return SelNode->FIcon;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPurchaseList::GetItemPath(int Index)
{
    TListItem* SelItem = this->Items->Item[Index];
    AnsiString FilePath = "";
    if( SelItem != NULL )
    {
        CNode* SelNode = (CNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            FilePath = SelNode->FFilePath;
        }
    }
    return FilePath;
}
//---------------------------------------------------------------------------

void __fastcall TPurchaseList::DoMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    FLastItem = GetItemAt( X, Y );
}
//---------------------------------------------------------------------------
int __fastcall TPurchaseList::GetSelectedSize(void)
{
    int Result = 0;
    TListItem* pItem = NULL;
    CNode* SelNode = NULL;
	for( int i = Items->Count - 1; i >= 0; --i )
	{
	    if( (pItem = Items->Item[i]) != NULL )
        {
		    if( pItem->Selected )
		    {
                if( (SelNode = (CNode*)pItem->Data) != NULL )
                    Result += SelNode->FSize;
            }
        }
    }
    return Result;
}
//---------------------------------------------------------------------------

