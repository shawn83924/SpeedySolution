//---------------------------------------------------------------------------

#ifndef OCODataStoreH
#define OCODataStoreH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include "OrderStore.h"
//---------------------------------------------------------------------------
class PACKAGE TOCODataStore : public TCustomControl
{
private:
protected:
public:
	__fastcall TOCODataStore(TComponent* Owner);
__published:
};
//---------------------------------------------------------------------------
class OCOPair
{
public:
	int OrderQty1;
	double ConditionPrice1;
	SideEnum OrderSide1;
	int OrderQty2;
	double ConditionPrice2;
	SideEnum OrderSide2;
	String Symbol;
    UnicodeString __fastcall GetStringField(int index);
};
#endif
