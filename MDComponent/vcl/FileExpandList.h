//---------------------------------------------------------------------------

#ifndef FileExpandListH
#define FileExpandListH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <math.hpp>
//---------------------------------------------------------------------------
typedef char ICON_INDEX;
const ICON_INDEX ICON_DIR = 0;
const ICON_INDEX ICON_NOFILE = 1;
const ICON_INDEX ICON_EXPAND = 2;
const ICON_INDEX ICON_FILE = 3;
//---------------------------------------------------------------------------
class CFileNode
{
public:
    AnsiString FName;
    AnsiString FSize;
    AnsiString FDateTime;
    AnsiString FFilePath;
    ICON_INDEX FIcon;
    bool       FTopNode; 
    int        FIndex; 
    bool       FExpand;
    TList*     FSubNodes;
    CFileNode* FParent;

            __fastcall  CFileNode(void);
            __fastcall  CFileNode(const TSearchRec& SearchRec, const AnsiString& ParentPath, int Index);
    virtual __fastcall ~CFileNode(void);
    void    __fastcall  BuildSubNodeList(void);
    void __fastcall  ClearSubNodes(void);
};
//---------------------------------------------------------------------------
class PACKAGE TFileExpandList : public TCustomListView
{
private:
    AnsiString  FRoot;
    TList*      FNodes;
    int         FSubNodeLeft;

private:
    void __fastcall InitControl(void);
    void __fastcall SetRoot(AnsiString RootPath);
    void __fastcall Loaded(void);
    void __fastcall ResetTopNodes(void);

private:
    void __fastcall DoCustomDrawItem(TCustomListView* Sender, TListItem* Item, TCustomDrawState State, bool &DefaultDraw);
    void __fastcall DrawCaption(TCanvas * canvas, TListItem* Item);
    void __fastcall DoCustomDrawSubItem(TCustomListView * Sender, TCanvas * canvas, TListItem* Item);
    void __fastcall DrawSubItem( TCanvas* canvas, int Index, const AnsiString& DrawString, UINT TextFormat );
    void __fastcall ClearListView(void);
    void __fastcall DoDblClick(TObject *Sender);
    void __fastcall LoadFiles(CFileNode* Node);
    void __fastcall RemoveFiles(CFileNode* Node);
    void __fastcall ClearNodes(void);
    void __fastcall BuildTopNodeList(void);
    void __fastcall AddTopNodeIntoListView(void);
    int  __fastcall MoveToTrashBox(const AnsiString& FilePath);

public:
               __fastcall  TFileExpandList(TComponent* Owner);
               __fastcall ~TFileExpandList();
    AnsiString __fastcall  GetSelectedName(void);
    AnsiString __fastcall  GetSelectedPath(void);
    ICON_INDEX __fastcall  GetSelectedType(void);
    bool       __fastcall  DeleteSelectedFile(void);
    void       __fastcall  NewFolder(const AnsiString& FolderName);

__published:
    __property AnsiString Root = { read = FRoot, write = SetRoot };
    __property int        SubNodeLeft = { read = FSubNodeLeft, write = FSubNodeLeft };
    __property            OnDragDrop;
    __property            OnDragOver;
    __property            Align;
    __property            Font;
    __property            SmallImages;
    __property            DragCursor;
    __property            DragKind;
    __property            DragMode;
    __property            Items;
};
//---------------------------------------------------------------------------
#endif

