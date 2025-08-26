//---------------------------------------------------------------------------

#include <basepch.h>

#pragma hdrstop

#include "RoundForm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRoundForm *)
{
    new TRoundForm(NULL);
}
//---------------------------------------------------------------------------
__fastcall TRoundForm::TRoundForm(TComponent* Owner)
: TComponent( Owner ),
  FOldFormResize( NULL ),
  FOldFormShow( NULL )
{
    FOwnerForm = dynamic_cast< TForm* >( Owner );
    if( !FOwnerForm )
        throw Exception( "The owner of a TRoundForm must be a form" );
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::Loaded( void )
{
    if( !ComponentState.Contains( csDesigning ) )
    {
        FOldFormResize =  FOwnerForm->OnResize;
        FOldFormShow   =  FOwnerForm->OnShow;
        FOwnerForm->OnResize = ResizeForm;
        FOwnerForm->OnShow   = ShowForm;
    }
}
//---------------------------------------------------------------------------
__fastcall TRoundForm::~TRoundForm( void )
{
    if( !ComponentState.Contains( csDesigning ) )
    {
        if( FOldFormResize != NULL )
            FOwnerForm->OnResize = FOldFormResize;
        if( FOldFormShow != NULL )
            FOwnerForm->OnShow   = FOldFormShow;
        if(!FOwnerForm->ComponentState.Contains(csDestroying))
            ResetFormShape( );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::ShowForm( TObject *Sender )
{
    if( FOldFormShow != NULL )
        FOldFormShow( Sender );
    SetFormRound( FRadius );
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::ResizeForm( TObject *Sender )
{
    if( FOldFormResize != NULL )
        FOldFormResize( Sender );
    SetFormRound( FRadius );
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::SetRadius( int NewRadius )
{
    if( FRadius != NewRadius )
    {
        FRadius = NewRadius;
        if( !ComponentState.Contains( csDesigning ) )
            SetFormRound( FRadius );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::SetFormRound( int Radius )
{
     HRGN   WinRgn;

     WinRgn = CreateRoundRectRgn( 0, 0, FOwnerForm->Width+1, FOwnerForm->Height+1, 2*Radius, 2*Radius );
     SetWindowRgn( FOwnerForm->Handle , WinRgn , true );
     DeleteObject( WinRgn );
}
//---------------------------------------------------------------------------
void __fastcall TRoundForm::ResetFormShape( void )
{
     HRGN WinRgn;

     WinRgn = CreateRectRgn( 0,0, FOwnerForm->Width+1, FOwnerForm->Height+1 );
     SetWindowRgn( FOwnerForm->Handle , WinRgn , true );
     DeleteObject( WinRgn );
}
//---------------------------------------------------------------------------
namespace Roundform
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TRoundForm)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
 