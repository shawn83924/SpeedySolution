//---------------------------------------------------------------------------

#ifndef PurchaseListH
#define PurchaseListH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
typedef char ICON_INDEX;
//---------------------------------------------------------------------------
class CNode
{
public:
    AnsiString FName;
    int        FSize;
    AnsiString FMediaLength;
    AnsiString FDateTime;
    AnsiString FFilePath;
    ICON_INDEX FIcon;
    bool       FTopNode;
    int        FIndex;
    bool       FExpand;
    TList*     FSubNodes;
    CNode*     FParent;

    //        __fastcall  CNode(void) {}
            __fastcall  CNode(const TSearchRec& SearchRec, const AnsiString& ParentPath, int Index);
    virtual __fastcall ~CNode(void);
    void    __fastcall  GetMediaLength(void);
    void    __fastcall  BuildSubNodeList(void);
    void    __fastcall  ClearSubNodes(void);
    void    __fastcall  GetFileTimeString(void);
};
//---------------------------------------------------------------------------
class PACKAGE TPurchaseList : public TListView
{
private:
    AnsiString  FRoot;
    TList*      FNodes;
    TListItem*  FLastItem;
private:
    void __fastcall InitControl(void);
    void __fastcall SetRoot(AnsiString RootPath);
    void __fastcall Loaded(void);
    void __fastcall ResetTopNodes(void);
    void __fastcall ClearListView(void);
    void __fastcall BuildTopNodeList(void);
    void __fastcall AddTopNodeIntoListView(void);
    void __fastcall DoCustomDrawItem(TCustomListView* Sender, TListItem* Item, TCustomDrawState State, bool &DefaultDraw);
    void __fastcall DrawCaption(TCanvas * canvas, TListItem* Item);
    void __fastcall DoCustomDrawSubItem(TCustomListView * Sender, TCanvas * canvas, TListItem* Item);
    void __fastcall DrawSubItem( TCanvas* canvas, int Index, const AnsiString& DrawString, UINT TextFormat );
    void __fastcall DoDblClick(TObject *Sender);
    void __fastcall DoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall LoadFiles(CNode* Node);
    void __fastcall RemoveFiles(CNode* Node);
public:
               __fastcall  TPurchaseList(TComponent* Owner);
               __fastcall ~TPurchaseList(void);
    ICON_INDEX __fastcall  GetSelectedType(void);
    AnsiString __fastcall  GetSelectedPath(void);
    ICON_INDEX __fastcall  GetItemType(int Index);
    int        __fastcall  GetSelectedSize(void);
    AnsiString __fastcall  GetItemPath(int Index);
__published:
    __property AnsiString Root = { read = FRoot, write = SetRoot };
    __property            Align;
    __property            Font;
    __property            Items;
    __property            SmallImages;
    __property            PopupMenu;
    __property            DragCursor;
    __property            DragKind;
    __property            DragMode;
    __property            OnDragDrop;
    __property            OnDragOver;
    __property            MultiSelect;
};
//---------------------------------------------------------------------------
#endif
