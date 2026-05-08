//---------------------------------------------------------------------------

#ifndef GraphToggleH
#define GraphToggleH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.WinXCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TGraphToggle : public TToggleSwitch
{
private:
	TPicture* FOffPicture;
	TPicture* FOnPicture;
protected:
	virtual void __fastcall Paint(void);
public:
	__fastcall TGraphToggle(TComponent* Owner);
	__fastcall ~TGraphToggle(void);
private:
	void __fastcall SetOffPicture(TPicture* picture);
	void __fastcall SetOnPicture(TPicture* picture);
	void __fastcall PictureChanged(TObject* Sender);
__published:
	__property AutoSize;
	__property TPicture* OffPicture = { read = FOffPicture, write = SetOffPicture};
	__property TPicture* OnPicture = { read = FOnPicture, write = SetOnPicture};
};
//---------------------------------------------------------------------------
#endif
