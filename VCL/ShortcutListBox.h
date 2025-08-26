//---------------------------------------------------------------------------

#ifndef ShortcutListBoxH
#define ShortcutListBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <FileCtrl.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class CShortcutFile
{
public:
    AnsiString FName;
    AnsiString FDisplay;
    AnsiString FShortcutPath;
    AnsiString FRealPath;
    int        FSerial;
    __fastcall CShortcutFile(const TSearchRec& SearchRec, const AnsiString& RootPath);
    void __fastcall ChangeSerial(int NewSerial);
    void __fastcall GetRealPath(void);
};
//---------------------------------------------------------------------------
class PACKAGE TShortcutListBox : public TListBox
{
protected:
    AnsiString FRoot;
    TImageList* FIcon;
    
public:
                   __fastcall TShortcutListBox(TComponent* Owner);
    void           __fastcall Reset(void);
    void           __fastcall ChangeFileIndex(int OldIndex, int NewIndex);
    void           __fastcall SetRoot(AnsiString RootPath);
    AnsiString     __fastcall GetFilePath(int Index);
    CShortcutFile* __fastcall GetFile(int Index);
    bool           __fastcall DeleteFileByIndex(int Index);

private:
    void __fastcall DoDrawItem(TWinControl *Control, int Index, const TRect &Rect, TOwnerDrawState State);
    void __fastcall Loaded(void);
    int  __fastcall MoveToTrashBox(const AnsiString& FilePath);

__published:
    //__property AnsiString Root = { read = FRoot, write = SetRoot };
    __property TImageList* Icon = { read = FIcon, write = FIcon };
};
//---------------------------------------------------------------------------
#endif


