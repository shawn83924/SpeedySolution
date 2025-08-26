//---------------------------------------------------------------------------

#ifndef RoundFormH
#define RoundFormH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <math.h>
//---------------------------------------------------------------------------
const float DEGToRAD =  3.141592654 / 180.0;
//---------------------------------------------------------------------------
class PACKAGE TRoundForm : public TComponent
{
private:
    TForm*          FOwnerForm;
    int             FRadius;
    TNotifyEvent    FOldFormResize;
    TNotifyEvent    FOldFormShow;
private:
    void __fastcall ResetFormShape( void );
    void __fastcall SetRadius( int NewRadius );
    void __fastcall SetFormRound( int Radius );
    void __fastcall ShowForm( TObject *Sender );
    void __fastcall ResizeForm( TObject *Sender );
    virtual void __fastcall Loaded( void );
public:
    __fastcall TRoundForm(TComponent* Owner);
    __fastcall ~TRoundForm( void );
__published:
    __property int Radius = { read = FRadius, write = SetRadius };
};
//---------------------------------------------------------------------------
#endif
 