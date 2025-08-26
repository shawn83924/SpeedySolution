//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "ShortcutListBox.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TShortcutListBox *)
{
    new TShortcutListBox(NULL);
}
//---------------------------------------------------------------------------
namespace Shortcutlistbox
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TShortcutListBox)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
//
//  class CShortcutFile
//
//---------------------------------------------------------------------------
__fastcall CShortcutFile::CShortcutFile(const TSearchRec& SearchRec, const AnsiString& RootPath)
:FName(SearchRec.Name)
,FDisplay("")
,FShortcutPath("")
,FRealPath("")
,FSerial(0)
{
    FSerial = FName.SubString( 1, 3 ).ToInt();
    FDisplay = FName.SubString( 4, FName.Length() - 3 );
    FShortcutPath = RootPath + FName;
    GetRealPath();
}
//---------------------------------------------------------------------------
void __fastcall CShortcutFile::ChangeSerial(int NewSerial)
{
    FSerial = NewSerial;
    AnsiString str;
    str.printf( "%03d", FSerial );
    FName = str + FDisplay;
}
//---------------------------------------------------------------------------
void __fastcall CShortcutFile::GetRealPath(void)
{
    char p[512];
    TFileStream* f = new TFileStream( FShortcutPath, fmOpenReadWrite );
    f->Read( p, 512 );
    delete f;
    FRealPath = AnsiString(p);
}
//---------------------------------------------------------------------------
//
//  class TShortcutListBox
//
//---------------------------------------------------------------------------
__fastcall TShortcutListBox::TShortcutListBox(TComponent* Owner)
:TListBox(Owner)
,FRoot("")
,FIcon(NULL)
{
    this->Style = lbOwnerDrawFixed;
    this->OnDrawItem = DoDrawItem;
}
//---------------------------------------------------------------------------

void __fastcall TShortcutListBox::DoDrawItem(TWinControl *Control, int Index,
      const TRect &Rect, TOwnerDrawState State)
{
    TRect ItemRect;
    if( State.Contains( odSelected ) )
    {
        Canvas->Font->Color = clWhite;
        Canvas->Brush->Color = clNavy;
        Canvas->Pen->Color = clNavy;
    }
    else
    {
        Canvas->Font->Color = clBlack;
        Canvas->Brush->Color = clWhite;
        Canvas->Pen->Color = clWhite;
    }
    Canvas->FillRect( Rect );
    ItemRect.Left = Rect.Left;
    ItemRect.Top = Rect.Top;
    ItemRect.Right = Rect.Right;
    ItemRect.Bottom = Rect.Bottom;
    CShortcutFile* ShortcutFile = NULL;

    if( FIcon != NULL )
    {
        FIcon->Draw( this->Canvas, ItemRect.Left + 1, ItemRect.Top, 0, true );
        ItemRect.Left += FIcon->Width + 3;
    }
    AnsiString Str;
    if( (ShortcutFile = (CShortcutFile*)(this->GetItemData(Index))) != NULL )
    {
        Str = ShortcutFile->FDisplay;
        DrawTextEx( this->Canvas->Handle, Str.c_str(), Str.Length(), &ItemRect,DT_LEFT|DT_END_ELLIPSIS, NULL );
    }
}
//---------------------------------------------------------------------------

void __fastcall TShortcutListBox::Loaded(void)
{
    this->Items->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TShortcutListBox::SetRoot(AnsiString RootPath)
{
    if( RootPath.Length() > 0 )
    {
        FRoot = RootPath;

        if( FRoot[ FRoot.Length() ] != '\\' )
            FRoot += "\\";
        Reset();
    }
}
//---------------------------------------------------------------------------

void __fastcall TShortcutListBox::Reset(void)
{
    this->Items->Clear();

    TSearchRec SearchRec;
    int SearchAttr = faAnyFile;
    SearchAttr &= ~faHidden;
    SearchAttr &= ~faSysFile;
    SearchAttr &= ~faDirectory;

    if( FindFirst(FRoot + "*", SearchAttr, SearchRec) == 0 )
    {
        CShortcutFile* File = NULL;
        int StrIndex;
        do
        {
            if( (SearchRec.Attr & SearchAttr) == SearchRec.Attr )
            {
                if( (File = new CShortcutFile(SearchRec,FRoot)) != NULL )
                {
                    StrIndex = this->Items->Add( File->FName );
                    this->SetItemData( StrIndex, (int)((void*)File) );
                }
            }

        } while( FindNext(SearchRec) == 0 );
        FindClose( SearchRec );
    }
    this->Sorted = true;
}
//---------------------------------------------------------------------------

void __fastcall TShortcutListBox::ChangeFileIndex(int OldIndex, int NewIndex)
{
    CShortcutFile* File = (CShortcutFile*)(this->GetItemData(OldIndex));
    if( NewIndex >= this->Count )
        NewIndex = this->Count - 1;
    CShortcutFile* File2 = (CShortcutFile*)(this->GetItemData(NewIndex));

    if( File != NULL )
    {
        AnsiString OldName = FRoot + File->FName;
        File->ChangeSerial( NewIndex );
        AnsiString NewName = FRoot + File->FName;
        RenameFile( OldName, NewName );

        if( File2 != NULL )
        {
            OldName = FRoot + File2->FName;
            File2->ChangeSerial( OldIndex );
            NewName = FRoot + File2->FName;
            RenameFile( OldName, NewName );
        }
        Reset();
        this->ItemIndex = NewIndex;
    }
}
//---------------------------------------------------------------------------

AnsiString __fastcall TShortcutListBox::GetFilePath(int Index)
{
    if( Index > -1 )
    {
        AnsiString str = FRoot + this->Items->Strings[ Index ];
        return str;
    }
    return AnsiString( "" );
}
//---------------------------------------------------------------------------

CShortcutFile* __fastcall TShortcutListBox::GetFile(int Index)
{
    CShortcutFile* File = NULL;
    if( Index > -1 )
    {
        File = (CShortcutFile*)(this->GetItemData(Index));
    }
    return File;
}
//---------------------------------------------------------------------------

bool __fastcall TShortcutListBox::DeleteFileByIndex(int Index)
{
    CShortcutFile* delFile = GetFile(Index);
    if( delFile == NULL )
        return true;
    
    SHFILEOPSTRUCT op;
    char b[MAX_PATH];
    AnsiString FilePath = FRoot + delFile->FName;
    int Result = MoveToTrashBox( FilePath );
    if( Result == 0 )
        return true;
    else if( Result == 1 )
    {
        this->Items->Delete( Index );
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

int __fastcall TShortcutListBox::MoveToTrashBox(const AnsiString& FilePath)
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


