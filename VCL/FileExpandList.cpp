//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "FileExpandList.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TFileExpandList *)
{
    new TFileExpandList(NULL);
}
//---------------------------------------------------------------------------
namespace Fileexpandlist
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TFileExpandList)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
int __fastcall CompText(void* Item1, void* Item2)
{
    return CompareText( ((CFileNode*)Item1)->FName, ((CFileNode*)Item2)->FName );
}
//---------------------------------------------------------------------------
int __fastcall CompType(void* Item1, void* Item2)
{
    return CompareValue( ((CFileNode*)Item1)->FIcon, ((CFileNode*)Item2)->FIcon );
}
//---------------------------------------------------------------------------
//
//  CFileNode
//
//---------------------------------------------------------------------------
__fastcall CFileNode::CFileNode(void)
:FName("")
,FFilePath("")
,FSize(0)
,FIndex(0)
,FTopNode(true)
,FExpand(false)
,FIcon(ICON_NOFILE)
,FSubNodes(NULL)
,FParent(NULL)
{
    FDateTime = System::TDateTime::CurrentDateTime();
}
//---------------------------------------------------------------------------
__fastcall CFileNode::CFileNode(const TSearchRec& SearchRec, const AnsiString& ParentPath, int Index)
:FSubNodes(NULL)
,FIcon(ICON_DIR)
,FExpand(false)
,FIndex(Index)
,FTopNode(true)
,FParent(NULL)
{
    FName = SearchRec.Name;
    FSize = IntToStr(SearchRec.Size);
    FDateTime = FileDateToDateTime(SearchRec.Time);
    FFilePath = ParentPath + FName;
}
//---------------------------------------------------------------------------
__fastcall CFileNode::~CFileNode(void)
{
    if( FSubNodes != NULL )
    {
        ClearSubNodes();
        delete FSubNodes;
    }
}
//---------------------------------------------------------------------------
void __fastcall CFileNode::BuildSubNodeList(void)
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
        CFileNode* Node = NULL;
        int Index = 0;
        AnsiString Ex;
        do
        {
            if( (SearchRec.Attr & SearchAttr) == SearchRec.Attr )
            {
                Ex = SearchRec.Name.SubString( SearchRec.Name.Length()-3, 4 );
                if( stricmp(Ex.c_str(), ".mpx") != 0 && stricmp(Ex.c_str(), ".mp3") != 0 )
                    continue;
                if( (Node = new CFileNode(SearchRec, Path, Index)) != NULL )
                {
                    Node->FIcon = ICON_FILE;
                    Node->FTopNode = false;
                    Node->FParent = this;
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
void __fastcall CFileNode::ClearSubNodes(void)
{
    CFileNode* SubNode = NULL;
    if( FSubNodes->Count > 0 )
        FIcon = ICON_DIR;
    for( int i = FSubNodes->Count - 1; i >= 0; --i )
    {
        if( (SubNode = (CFileNode*)FSubNodes->Items[i]) != NULL )
        {
            delete SubNode;
            SubNode = NULL;
            FSubNodes->Delete(i);
        }
    }
}
//---------------------------------------------------------------------------
//
//  class TFileExpandList
//
//---------------------------------------------------------------------------
__fastcall TFileExpandList::TFileExpandList(TComponent* Owner)
:TCustomListView(Owner)
,FRoot("c:\\")
,FNodes(NULL)
,FSubNodeLeft(20)
{
    InitControl();
}
//---------------------------------------------------------------------------
__fastcall TFileExpandList::~TFileExpandList()
{
    if( FNodes != NULL )
    {
        ClearNodes();
        delete FNodes;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::ClearNodes(void)
{
    CFileNode* Node = NULL;
    for( int i = FNodes->Count - 1; i >= 0; --i )
    {
        if( (Node = (CFileNode*)FNodes->Items[i]) != NULL )
        {
            delete Node;
            Node = NULL;
            FNodes->Delete(i);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::InitControl(void)
{
    TListColumn* pColumn;

    ReadOnly            = true;
    ViewStyle           = vsReport;

    pColumn             = Columns->Add();
    pColumn->Caption    = "名稱";
    pColumn             = Columns->Add();
    pColumn->Caption    = "大小";
    pColumn             = Columns->Add();
    pColumn->Caption    = "修改日期";

    OnCustomDrawItem = DoCustomDrawItem;
    OnDblClick = DoDblClick;
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::Loaded(void)
{
    SetRoot( FRoot );
    this->Column[0]->Width = ( Width / 5 * 2 ) - 4;
    this->Column[1]->Width = ( Width / 5 ) - 4;
    this->Column[2]->Width = ( Width / 5 * 2 ) - 4;
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::SetRoot(AnsiString RootPath)
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
void __fastcall TFileExpandList::ResetTopNodes(void)
{
    ClearListView();
    BuildTopNodeList();
    AddTopNodeIntoListView();
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::ClearListView(void)
{
    if( FNodes != NULL )
    {
        ClearNodes();
        Items->Clear();
    }
    else
    {
        FNodes = new TList;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::BuildTopNodeList(void)
{
    TSearchRec SearchRec;
    int SearchAttr = faAnyFile;
    SearchAttr &= ~faHidden;
    SearchAttr &= ~faSysFile;

    if( FindFirst(FRoot + "*", SearchAttr, SearchRec) == 0 )
    {
        CFileNode* Node = NULL;
        int Index = 0;
        AnsiString Ex;
        do
        {
            if( (SearchRec.Attr & SearchAttr) == SearchRec.Attr )
            {
                if( (Node = new CFileNode(SearchRec, FRoot, Index)) != NULL )
                {
                    if( Node->FName != "." && Node->FName != ".." )
                    {
                        if( (SearchRec.Attr & faDirectory) == 0 )
                        {
                            Ex = SearchRec.Name.SubString( SearchRec.Name.Length()-3, 4 );
                            if( stricmp(Ex.c_str(),".mpx") != 0 && stricmp(Ex.c_str(),".mp3") != 0 )
                                continue;
                            Node->FIcon = ICON_FILE;
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
        FNodes->Sort( CompType );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::AddTopNodeIntoListView(void)
{
    TListItem* NewItem = NULL;
    CFileNode* Node = NULL;
    int NodeCount = FNodes->Count;

    for( int i = 0; i < NodeCount; ++i )
    {
        if( (Node = (CFileNode*)FNodes->Items[i]) != NULL )
        {
            NewItem = Items->Add();
            NewItem->Data = (void*)Node;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::DoCustomDrawItem(
      TCustomListView *Sender, TListItem *Item, TCustomDrawState State,
      bool &DefaultDraw)
{
    Graphics::TBitmap * Buffer = new Graphics::TBitmap();
    TRect ItemRect = Item->DisplayRect( drBounds );

    Buffer->Width  = ItemRect.Width();
    Buffer->Height = ItemRect.Height();

    if( Item->Selected == true )//if( State.Contains( cdsSelected ) )
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

    Buffer->Canvas->FillRect( Rect(0,0,Buffer->Width,Buffer->Height) );
    if( Font->Size > 2 )
        Buffer->Canvas->Font->Size = Font->Size - 2;
    DrawCaption( Buffer->Canvas, Item );
    DoCustomDrawSubItem( Sender, Buffer->Canvas, Item );
    Sender->Canvas->Draw( 0, ItemRect.Top, Buffer );

    delete Buffer;
    DefaultDraw = false;
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::DrawCaption(TCanvas * canvas, TListItem* Item)
{
    TRect Rect = Item->DisplayRect( drLabel );
    Rect.Bottom = Rect.Height();
    Rect.Top = 0;

    CFileNode* Node = (CFileNode*)Item->Data;
    ICON_INDEX Icon = ICON_DIR;
    if( Node != NULL )
    {
        Icon = Node->FIcon;
        if( Node->FTopNode == true )
            Node->FIndex = Item->Index;
        else
            Rect.Left += FSubNodeLeft;
    }
    if( SmallImages != NULL )
        SmallImages->Draw( canvas, Rect.Left - SmallImages->Width - 1, 0, Icon, true );

    Item->Caption = ((CFileNode*)Item->Data)->FName;
    DrawTextEx( canvas->Handle, Item->Caption.c_str(), Item->Caption.Length(), &Rect, DT_LEFT|DT_END_ELLIPSIS, NULL );
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::DoCustomDrawSubItem(TCustomListView* Sender, TCanvas* canvas, TListItem* Item)
{
    AnsiString str = ((CFileNode*)Item->Data)->FSize;
    DrawSubItem( canvas, 1, str, DT_RIGHT|DT_END_ELLIPSIS );
    str = ((CFileNode*)Item->Data)->FDateTime;
    DrawSubItem( canvas, 2, str, DT_LEFT|DT_END_ELLIPSIS );
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::DrawSubItem( TCanvas* canvas, int Index, const AnsiString& DrawString, UINT TextFormat )
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
void __fastcall TFileExpandList::DoDblClick(TObject *Sender)
{
    if( Selected != NULL )
    {
        CFileNode* Node = (CFileNode*)Items->Item[Selected->Index]->Data;
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
    }
    this->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::LoadFiles(CFileNode* Node)
{
    int AddIndex = Node->FIndex + 1;
    int NodeCount = Node->FSubNodes->Count;
    TListItem* NewItem = NULL;
    CFileNode* SubNode = NULL;

    for( int i = 0; i < NodeCount; ++i )
    {
        if( (SubNode = (CFileNode*)(Node->FSubNodes->Items[i])) != NULL )
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
void __fastcall TFileExpandList::RemoveFiles(CFileNode* Node)
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
AnsiString __fastcall TFileExpandList::GetSelectedName(void)
{
    TListItem* SelItem = this->Selected;
    AnsiString FileName = "";
    if( SelItem != NULL )
    {
        CFileNode* SelNode = (CFileNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            FileName = SelNode->FName;
        }
    }
    return FileName;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFileExpandList::GetSelectedPath(void)
{
    TListItem* SelItem = this->Selected;
    AnsiString FilePath = "";
    if( SelItem != NULL )
    {
        CFileNode* SelNode = (CFileNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            FilePath = SelNode->FFilePath;
        }
    }
    return FilePath;
}
//---------------------------------------------------------------------------
bool __fastcall TFileExpandList::DeleteSelectedFile(void)
{
    TListItem* SelItem = this->Selected;
    if( SelItem != NULL )
    {
        CFileNode* SelNode = (CFileNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            int Result = MoveToTrashBox( SelNode->FFilePath );
            if( Result == 0 )
                return true;
            else if( Result == 1 )
            {
                if( SelNode->FTopNode == false )
                {
                    SelNode->FParent->FSubNodes->Remove( SelNode );
                }
                this->Delete( SelItem );
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
int __fastcall TFileExpandList::MoveToTrashBox(const AnsiString& FilePath)
{
    SHFILEOPSTRUCT op;
    char b[MAX_PATH];

	strcpy(b, FilePath.c_str());
	b[ FilePath.Length() + 1] = 0;
	ZeroMemory( &op, sizeof(SHFILEOPSTRUCT) );
	op.wFunc = FO_DELETE;
	op.pFrom = b;
	op.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI;
	if( SHFileOperation(&op) == 0 )
    {
        if( FileExists(FilePath) == true || DirectoryExists(FilePath) == true )
            return 0;
        else
            return 1;
    }
    return -1;
}
//---------------------------------------------------------------------------
ICON_INDEX __fastcall TFileExpandList::GetSelectedType(void)
{
    TListItem* SelItem = this->Selected;
    if( SelItem != NULL )
    {
        CFileNode* SelNode = (CFileNode*)SelItem->Data;
        if( SelNode != NULL )
        {
            return SelNode->FIcon;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall TFileExpandList::NewFolder(const AnsiString& FolderName)
{
    AnsiString FolderPath = FRoot + FolderName;
    if( DirectoryExists(FolderPath) == false && FileExists(FolderPath) == false )
        ForceDirectories( FolderPath );

    CFileNode* Node = new CFileNode();
    Node->FName = FolderName;
    Node->FFilePath = FolderPath;
    TListItem* NewItem = this->Items->Insert( 0 );
    NewItem->Data = (void*)Node;
    this->SetFocus();
    NewItem->Selected = true;
}
//---------------------------------------------------------------------------

