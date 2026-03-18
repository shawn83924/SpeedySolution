//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "OCODataStore.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TOCODataStore *)
{
	new TOCODataStore(NULL);
}
//---------------------------------------------------------------------------
__fastcall TOCODataStore::TOCODataStore(TComponent* Owner)
	: TCustomControl(Owner)
{
}
//---------------------------------------------------------------------------
UnicodeString __fastcall OCOPair::GetStringField(int index)
{
	UnicodeString ResultString;
	switch(index)
	{
		case 1:
			ResultString = L"測試狀態";
			break;
		case 2:
			ResultString = Symbol;
		case 3:
			ResultString = IntToStr(OrderQty1);
			break;
		case 4:
			ResultString = FloatToStr(ConditionPrice1);
			break;
		case 5:
			if(OrderSide1 == nsOrderMessageDefine::sBuy)
				ResultString = L"委買";
			else
				ResultString = L"委賣";
			break;
		case 6:
			ResultString = IntToStr(OrderQty2);
			break;
		case 7:
			ResultString = FloatToStr(ConditionPrice2);
			break;
		case 8:
			if(OrderSide2 == nsOrderMessageDefine::sBuy)
				ResultString = L"委買";
			else
				ResultString = L"委賣";
			break;
	}

    return ResultString;
}
//---------------------------------------------------------------------------
namespace Ocodatastore
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOCODataStore)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
