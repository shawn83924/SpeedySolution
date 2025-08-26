#include "GlobalVariable.h"

//------------------------------------------------------------------------------
//Specific setting
//------------------------------------------------------------------------------
void LoadSpecificSetting()
{
	if ( gSymbolListMode == "IBF" )
		SetSpecificSetting_IBF();
	else if ( gSymbolListMode == "ML" ) //MasterLink
		SetSpecificSetting_ML();	
	else if ( gSymbolListMode == "MDBS" )
		LoadSymbolList();
	else
	{
		UFC::BufferedLog::Printf(" Can't Load Unknown Mode(%s)",  gSymbolListMode.c_str());
		UFC::BufferedLog::FlushToFile();
		exit(1);
	}
		
}
//------------------------------------------------------------------------------
void AddSubscribeFutureSymbolToSymbolList(UFC::AnsiString CQGProdcutID, UFC::AnsiString MonthCount )
{
	if ( MonthCount.Length() > 0 )
	{
		int monthCount_int = MonthCount.ToInt();
		UFC::AnsiString symbolPrefix;
		for (int i = 1; i <= monthCount_int; i++)
		{		
			symbolPrefix.Printf("%s??%d", CQGProdcutID.c_str(), i); //If requested symbol is a symbol prefix that follows '?N' or '??N' (where N is number, e.g. EP?2) 
																//   then it is resolved to Nth nearest contract.
			gSymbolList.push(symbolPrefix);
			gSubscribedCQGFutureAndSpreadCount ++;
		}
	}
}
//------------------------------------------------------------------------------
void AddSubscribeOptionSymbolToSymbolList( SymbolInfo* ProductInfo )
{
	if ( ProductInfo->GetOptionMonthCount() > 0 )
	{
		gSubscribeOptionProductList.Add( ProductInfo->GetCQGSymbol(), ProductInfo );
		gSubscribedCQGOptionCount = gSubscribedCQGOptionCount + ProductInfo->GetOptionMonthCount();
	}
}
//------------------------------------------------------------------------------