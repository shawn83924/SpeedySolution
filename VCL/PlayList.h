//---------------------------------------------------------------------------

#ifndef PlayListH
#define PlayListH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>

//---------------------------------------------------------------------------
class CPLFile
{
public:
    int        FSize;
    AnsiString FPath;
    __fastcall CPLFile(void) : FSize(0), FPath("") {}
};
//---------------------------------------------------------------------------
class PACKAGE TPlayList : public TComponent
{
protected:
    TList*     FFiles;
    int        FMaxSize;
    int        FMaxCount;
    int        FCurrentSize;
    TListView* FView;
private:
    bool     __fastcall  Exist(const AnsiString& fPath);
    void     __fastcall  Loaded(void);
    bool     __fastcall  AddIntoList(CPLFile* plFile);
    void     __fastcall  DoData(TObject *Sender, TListItem *Item);
public:
             __fastcall  TPlayList(TComponent* Owner);
    virtual  __fastcall ~TPlayList(void);
public:
    int      __fastcall  Open(const AnsiString& fName);
    void     __fastcall  Save(const AnsiString& fName);
    void     __fastcall  Add(const AnsiString& fPath);
    void     __fastcall  Add(CPLFile* plFile);
    void     __fastcall  Delete(int Index, bool DelReal);
    void     __fastcall  Clear(void);
    void     __fastcall  MoveUp(int Index);
    void     __fastcall  MoveDown(int Index);
    void     __fastcall  MoveToTop(int Index);
    void     __fastcall  MoveToEnd(int Index);
    CPLFile* __fastcall  GetFile(int Index);
    int      __fastcall  FileCount(void) { return FFiles->Count; }
    int      __fastcall  GetCurrentSize(void) { return FCurrentSize; }
    void     __fastcall  LoadFromPlayListFile(const AnsiString& fPath);
__published:
    __property int MaxSize = { read = FMaxSize, write = FMaxSize };
    __property int MaxCount = { read = FMaxCount, write = FMaxCount };
    __property TListView* ListView = { read = FView, write = FView };
};
//---------------------------------------------------------------------------
#endif
 