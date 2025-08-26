//---------------------------------------------------------------------------

#ifndef SortListFormH
#define SortListFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UFC.h"
//---------------------------------------------------------------------------
class PACKAGE TSortForm : public TForm
{
__published:	// IDE-managed Components
	TListBox *ListBox1;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);

private:	// User declarations
	UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FSymbolList;
public:		// User declarations
	__fastcall TSortForm(TComponent* Owner);
	__fastcall ~TSortForm( void );
	void __fastcall AddSymbols( String& Symbol, String& DisplayName );
	void __fastcall GetSymbolOrder( UFC::PStringList& );
};
//---------------------------------------------------------------------------
//extern PACKAGE TSortForm *SortForm;
//---------------------------------------------------------------------------
#endif
