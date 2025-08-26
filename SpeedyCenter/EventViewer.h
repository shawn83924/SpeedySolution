//---------------------------------------------------------------------------

#ifndef EventViewerH
#define EventViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TEventForm : public TForm
{
__published:	// IDE-managed Components
    TListView *EventListView;
    TStatusBar *StatusBar;
    TToolBar *ToolBar1;
    TToolButton *ToolButton1;
    TToolButton *ToolButton2;
    TSaveDialog *SaveDialog;
	TImageList *StatImageList;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ToolButton2Click(TObject *Sender);
    void __fastcall ToolButton1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TEventForm(TComponent* Owner);
    __fastcall ~TEventForm( void );
    void __fastcall AddEvent( const AnsiString& Host, const AnsiString& Type, const AnsiString& Msg  );
    void __fastcall Save( const AnsiString& FileName );
    void __fastcall Load( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TEventForm *EventForm;
//---------------------------------------------------------------------------
#endif
