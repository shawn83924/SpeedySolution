//---------------------------------------------------------------------------

#include <basepch.h>

#pragma hdrstop

#include "PlayList.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TPlayList *)
{
    new TPlayList(NULL);
}
//---------------------------------------------------------------------------
namespace Playlist
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TPlayList)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
//
//  class TPlayList
//
//---------------------------------------------------------------------------
const int tUnitSize = 260;
//---------------------------------------------------------------------------
__fastcall TPlayList::TPlayList(TComponent* Owner)
:TComponent(Owner)
,FFiles(new TList)
,FMaxSize(0)
,FMaxCount(0)
,FCurrentSize(0)
,FView(NULL)
{
}
//---------------------------------------------------------------------------

__fastcall TPlayList::~TPlayList(void)
{
    delete FFiles;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Clear(void)
{
    int ListCount = FFiles->Count;
    CPLFile* plFile = NULL;
    for( int i = ListCount - 1; i >= 0; --i )
    {
        if( (plFile = (CPLFile*)(FFiles->Items[i])) != NULL )
        {
            delete plFile;
            plFile = NULL;
        }
        FFiles->Delete(i);
    }
    FCurrentSize = 0;

    if( FView != NULL )
    {
        FView->Items->Count = 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Loaded(void)
{
    if( FView != NULL )
    {
        if( FView->Columns->Count > 0 )
            FView->Columns->Clear();

        TListColumn* pColumn = NULL;
        pColumn = FView->Columns->Add();
        pColumn->Caption = "¦WºÙ";
        pColumn->Width = (FView->Width / 4 * 3) - 1;

        pColumn = FView->Columns->Add();
        pColumn->Caption = "¤j¤p";
        pColumn->Width = (FView->Width / 4) - 1;

        FView->ViewStyle = vsReport;
        FView->ReadOnly = true;
        FView->OwnerData = true;
        FView->OwnerDraw = false;
        FView->OnData = DoData;
    }
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::DoData(TObject *Sender, TListItem *Item)
{
    CPLFile* plFile = (CPLFile*)(FFiles->Items[Item->Index]);
    Item->Caption = ExtractFileName( plFile->FPath );
    Item->SubItems->Add( plFile->FSize );
    Item->ImageIndex = 0;
}
//---------------------------------------------------------------------------

int __fastcall TPlayList::Open(const AnsiString& fName)
{
    if( FileExists(fName) )
    {
        TFileStream* fStream = new TFileStream( fName, fmOpenRead );
        int fCount = 0;
        fStream->Read( &fCount, 4 );
        if( (fStream->Size - 4) != (fCount * (MAX_PATH + 4)) )
        {
            delete fStream;
            return -1;
        }

        Clear();
        char fPath[MAX_PATH];
        CPLFile* plFile = NULL;
        for( int i = 0; i < fCount; ++i )
        {
            plFile = new CPLFile;
            fStream->Read( &(plFile->FSize), 4 );
            fStream->Read( fPath, MAX_PATH );
            plFile->FPath = AnsiString(fPath);
            AddIntoList( plFile );
        }

        if( FView != NULL )
        {
            FView->Items->Count = fCount;
            FView->Repaint();
        }

        delete fStream;
        return fCount;
    }
    return -1;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Save(const AnsiString& fName)
{
    int fCount = FFiles->Count;
    if( fCount == 0 )
        return;

    if( FileExists(fName) == true )
        DeleteFile( fName );

    TFileStream* fStream = new TFileStream( fName, fmCreate );
    fStream->Write( &fCount, 4 );
    CPLFile* plFile = NULL;
    char fPath[MAX_PATH];
    for( int i = 0; i < fCount; ++i )
    {
        if( (plFile = (CPLFile*)FFiles->Items[i]) != NULL )
        {
            fStream->Write( &(plFile->FSize), 4 );
            strcpy( fPath, plFile->FPath.c_str() );
            fStream->Write( fPath, MAX_PATH );
            plFile = NULL;
        }
    }
    delete fStream;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Add(const AnsiString& fPath)
{
    TFileStream* fStream = new TFileStream( fPath, fmOpenRead );
    int fSize = fStream->Size;
    CPLFile* plFile = new CPLFile;
    plFile->FSize = fSize;
    plFile->FPath = fPath;
    Add( plFile );
    delete fStream;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Add(CPLFile* plFile)
{
    if( Exist(plFile->FPath) == true )
        return;

    AddIntoList( plFile );
    if( FView != NULL )
    {
        FView->Items->Count = FFiles->Count;
        FView->Repaint();
    }
}
//---------------------------------------------------------------------------

bool __fastcall TPlayList::AddIntoList(CPLFile* plFile)
{
    int fSize = plFile->FSize;
    AnsiString fPath = plFile->FPath;

    if( FMaxCount > 0 )
    {
        if( FMaxCount <= FFiles->Count )
            return false;
    }

    if( FMaxSize > 0 )
    {
        if( (FCurrentSize + fSize) > FMaxSize )
            return false;
    }

    //if( FileExists(fPath) )
    //{
        FFiles->Add( (void*)plFile );
        FCurrentSize += fSize;
        return true;
    //}

    //return false;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::Delete(int Index, bool DelReal)
{
    if( Index < FFiles->Count )
    {
        CPLFile* plFile = (CPLFile*)(FFiles->Items[Index]);
        if( plFile != NULL )
        {
            AnsiString fPath = plFile->FPath;
            FCurrentSize -= plFile->FSize;
            delete plFile;
            FFiles->Delete( Index );

            if( DelReal == true )
                DeleteFile( fPath );

            if( FView != NULL )
            {
                FView->Items->Count = FFiles->Count;
                FView->ItemIndex = -1;
                FView->Repaint();
            }
        }
    }
}
//---------------------------------------------------------------------------

bool __fastcall TPlayList::Exist(const AnsiString& fPath)
{
    int fCount = FFiles->Count;
    CPLFile* plFile = NULL;
    for( int i = 0; i < fCount; ++i )
    {
        if( (plFile = (CPLFile*)(FFiles->Items[i])) != NULL )
        {
            if( plFile->FPath == fPath )
                return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::MoveUp(int Index)
{
    if( Index > 0 )
    {
        FFiles->Move( Index, Index - 1 );
        if( FView != NULL )
        {
            TListItem* pItem = FView->Items->Item[Index-1];
            pItem->Selected = true;
            pItem->Focused = true;
            FView->Repaint();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::MoveDown(int Index)
{
    if( Index < (FFiles->Count - 1) )
    {
        FFiles->Move( Index, Index + 1 );
        if( FView != NULL )
        {
            TListItem* pItem = FView->Items->Item[Index+1];
            pItem->Selected = true;
            pItem->Focused = true;
            FView->Repaint();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::MoveToTop(int Index)
{
    if( Index > 0 )
    {
        FFiles->Move( Index, 0 );
        if( FView != NULL )
        {
            TListItem* pItem = FView->Items->Item[0];
            pItem->Selected = true;
            pItem->Focused = true;
            FView->Repaint();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::MoveToEnd(int Index)
{
    int EndIndex = FFiles->Count - 1;
    if( Index < EndIndex )
    {
        FFiles->Move( Index, EndIndex );
        if( FView != NULL )
        {
            TListItem* pItem = FView->Items->Item[EndIndex];
            pItem->Selected = true;
            pItem->Focused = true;
            FView->Repaint();
        }
    }
}
//---------------------------------------------------------------------------

CPLFile* __fastcall TPlayList::GetFile(int Index)
{
    CPLFile* plFile = NULL;
    if( Index < FFiles->Count )
    {
        plFile = (CPLFile*)(FFiles->Items[Index]);
    }
    return plFile;
}
//---------------------------------------------------------------------------

void __fastcall TPlayList::LoadFromPlayListFile(const AnsiString& fPath)
{
    TPlayList* pPlayList = new TPlayList(NULL);
    int fCount;
    CPLFile* plFile = NULL;
    if( (fCount = pPlayList->Open(fPath)) > 0 )
    {
        for( int i = 0; i < fCount; ++i )
        {
            if( (plFile = pPlayList->GetFile(i)) != NULL )
            {
                if( Exist(plFile->FPath) == false )
                    this->AddIntoList( plFile );
            }
        }

        if( FView != NULL )
        {
            FView->Items->Count = FFiles->Count;
            FView->Repaint();
        }
    }
    delete pPlayList;
}
//---------------------------------------------------------------------------

