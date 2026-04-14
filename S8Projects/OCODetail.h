//---------------------------------------------------------------------------

#ifndef OCODetailH
#define OCODetailH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "OCODataList.h"
#include <Vcl.ComCtrls.hpp>
#include "RoundFormEx.h"
#include "FMFHeader.h"
//---------------------------------------------------------------------------
class TOCODetailForm : public TForm , public TClientForm
{
__published:	// IDE-managed Components
	TOCODataList *OCODataList;
	TRoundFormEx *RoundFormEx;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TOCODetailForm(TComponent* Owner);
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfList; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )         { return this; }
	virtual int         __fastcall GetGroup( void )        { return this->Tag; }

	virtual void        __fastcall Release( void );
	virtual void        __fastcall SetVisible( bool IsVisible );

};
//---------------------------------------------------------------------------
extern PACKAGE TOCODetailForm *OCODetailForm;
//---------------------------------------------------------------------------
#endif
