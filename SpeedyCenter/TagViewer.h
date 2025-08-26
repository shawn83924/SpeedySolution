//---------------------------------------------------------------------------

#ifndef TagViewerH
#define TagViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>
#include "..\UFC\iniFile.h"
//---------------------------------------------------------------------------
class TTagForm : public TForm
{
__published:	// IDE-managed Components
    TListView *TagListView;
    TPanel *Panel1;
    TLabel *MsgTypeLabel;
    void __fastcall TagListViewColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall TagListViewCompare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
private:	// User declarations
    UFC::UiniFile* FTagToName;
    UFC::UiniFile* FTagValues;
    int            FColumnToSort;
    int            FSortDir;
public:		// User declarations
    void __fastcall ReplaceChar( const AnsiString& Msg, char From, char To );
    __fastcall TTagForm(TComponent* Owner);
	void __fastcall SetFIXMessage( const AnsiString& Msg );
	void __fastcall SetTWSEFIXMessage( const AnsiString& Msg );
	void __fastcall SetTMPMessage( const AnsiString& Msg );
};
//---------------------------------------------------------------------------
extern PACKAGE TTagForm *TagForm;
//---------------------------------------------------------------------------
#endif
