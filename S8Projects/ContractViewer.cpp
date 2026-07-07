//---------------------------------------------------------------------------
#include <vcl.h>
#include "FMTConfig.h"
#include "ContractForm.h"
#include <WideStrUtils.hpp>
#pragma hdrstop
#include "ExecutionViewerForm.h"
#include "ContractList.h"
#include "ChartForm.h"
#include "TBarForm.h"
#include "ContractViewer.h"
#include "main.h"
#include "UnifyDlg.h"
#include "UnifyUtility.h"
#include "TrainingDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OrderBookList"
#pragma link "MarketDataStore"
#pragma link "MarketDataList"
#pragma link "RoundFormEx"
//---------------------------------------------------------------------------
#pragma link "GraphButton"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TContractViewerForm *ContractViewerForm;
extern TCMarketDataStore* gMarketDataStore;
extern bool   			  gIsExpired;
//---------------------------------------------------------------------------
UFC::Int32 BaseYear;
UFC::Int32 Remainder;
const int MSG_PUMP_TIMES = 50;
//---------------------------------------------------------------------------
int __fastcall SortCompare(TStringList* List, int Index1, int Index2)
{
	TSymbolItem* Item1 = (TSymbolItem*)List->Objects[ Index1 ];
	TSymbolItem* Item2 = (TSymbolItem*)List->Objects[ Index2 ];

	if( Item1->IsContract() == true && Item2->IsContract() == true)
	{
		if( Item1->Info->GetMarket() == ::mTSE || Item1->Info->GetMarket() == ::mOTC )
			return  Item1->Symbol.Compare( Item2->Symbol );
		else
			return Item1->Info->GetMaturityDate().AnsiCompare( Item2->Info->GetMaturityDate());
	}
	return 0;
}
//---------------------------------------------------------------------------
int CALLBACK CompareFunc(long lParam1, long lParam2, long Reverse)
{
	TTreeNode *Node1 = reinterpret_cast<TTreeNode *>(lParam1);
	TTreeNode *Node2 = reinterpret_cast<TTreeNode *>(lParam2);
	if ((Node1 == NULL) || (Node2 == NULL))
		return 0;
	AnsiString      Str1 = Node1->Text;
	AnsiString      Str2 = Node2->Text;
	UFC::AnsiString ID1  = Str1.c_str();
	UFC::AnsiString ID2  = Str2.c_str();

	if( ID1 == "Time Spread" )
		return 1;
	else if( ID2 == "Time Spread")
		return -1;

	if( ID1.Length() == 3 )
		return -1;
	else if( ID2.Length() == 3 )
		return 1;

	if( ID1[2] == 'F' && ID1.Length() >3 && ID2.Length() > 3 )
	{
		UFC::AnsiString YearMonth1, YearMonth2;

		if( Remainder == 9 && ID1[4] == '0' )
			YearMonth1.Printf( "%d%02d" , BaseYear +(int)( ID1[4] -'0' ) + 10, (int)( ID1[3] - 'A' + 1 ) );
		else if( ( Remainder == 0 && ID1[4] == '9' ) || ( Remainder == 0 && ID1[4] == '8' ) )
			YearMonth1.Printf( "%d%02d" , BaseYear +(int)( ID1[4] -'0' ) - 10, (int)( ID1[3] - 'A' + 1 ) );
		else
			YearMonth1.Printf( "%d%02d" , BaseYear +(int)( ID1[4] -'0' ), (int)( ID1[3] - 'A' + 1 ) );

		if( Remainder == 9 && ID2[4] == '0' )
			YearMonth2.Printf( "%d%02d" , BaseYear +(int)( ID2[4] -'0' ) + 10, (int)( ID2[3] - 'A' + 1 ) );
		else if( ( Remainder == 0 && ID2[4] == '9' ) || ( Remainder == 0 && ID2[4] == '8' ) )
			YearMonth2.Printf( "%d%02d" , BaseYear +(int)( ID2[4] -'0' ) - 10, (int)( ID2[3] - 'A' + 1 ) );
		else
			YearMonth2.Printf( "%d%02d" , BaseYear +(int)( ID2[4] -'0' ), (int)( ID2[3] - 'A' + 1 ) );

		UFC::Int32 Ref1 = YearMonth1.ToInt();
		UFC::Int32 Ref2 = YearMonth2.ToInt();

		if( Ref1 < Ref2 )
			return -1;
		else if( Ref1 > Ref2 )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
//---------------------------------------------------------------------------
//
//	class TForm1
//
//---------------------------------------------------------------------------
__fastcall TContractViewerForm::TContractViewerForm(TComponent* Owner)
: TForm(Owner)
,STR_FUTURES( L"期貨")
,STR_OPTIONS( L"選擇權" )
,STR_STOCK( L"股票" )
,STR_WARRANT( L"權證" )
,STR_CALL( L"買權" )
,STR_PUT( L"賣權" )
,FTBarOpened( false )
,FOnSelectSymbol( NULL )
{
	UFC::UDate Date;

	Date.setCurrent();
	BaseYear = ( Date.getYear() / 10 ) * 10;
	Remainder = Date.getYear() % 10;

	FTWSESearchKeys = new TStringList();
	FOTCSearchKeys = new TStringList();
	FTWSEWarrantSearchKeys = new TStringList();
	FOTCWarrantSearchKeys = new TStringList();
	FTAIFEXFuturesSearchKeys = new TStringList();
	FTAIFEXOptionsSearchKeys = new TStringList();
	FResult = new TStringList();
	FBufferBmp = new Graphics::TBitmap();
	AddCatalog( );
	ResultListBox->Count = 0;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddCatalog( void )
{
	FTWSESearchKeys->AddObject( L"上市", new TSymbolItem( L"上市" ) );
	FOTCSearchKeys->AddObject( L"上櫃", new TSymbolItem( L"上櫃" ) );
	FTWSEWarrantSearchKeys->AddObject( L"上市權證", new TSymbolItem( L"上市權證" ) );
	FOTCWarrantSearchKeys->AddObject( L"上櫃權證", new TSymbolItem( L"上櫃權證" ) );
	FTAIFEXFuturesSearchKeys->AddObject( L"期貨", new TSymbolItem( L"期貨" ) );
	FTAIFEXOptionsSearchKeys->AddObject( L"選擇權", new TSymbolItem( L"選擇權" ) );
}
//---------------------------------------------------------------------------
// TAIFEX, 台灣期交所
// CFFEX,中國金融期貨交易所
// SHFE, 上海期貨交易所
// CZCE, 鄭州商品交易所
// DCE,  大連商品交易所
// CME,  芝加哥交易所
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddSymbols(  TLabel* StatusLabel  )
{
	CMarketDataStore = gMarketDataStore;
	for( int i = 0; i < CMarketDataStore->ExchangeCount(); i++ )
	{
		String Exchange( CMarketDataStore->GetExchange(i).c_str() );

		if( g_Config.SupportExchange( Exchange ) == true )
		{
			if( StatusLabel != NULL )
			{
				String Msg;
				Msg.printf( L"建立[%s]商品索引", Exchange );
				StatusLabel->Caption = Msg;
				Application->ProcessMessages();
			}
			if( Exchange == L"TAIFEX" || Exchange == "TFX" )
				AddTAIFEX( Exchange );
			else if(  Exchange == "TWSE" || Exchange == "OTC"  )
				AddStock( Exchange );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ClearList( TStringList* ClearKeys )
{
	for( register int i = 0;i< ClearKeys->Count; i ++ )
		delete (TSymbolItem*)ClearKeys->Objects[i];
	ClearKeys->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ClearSymbols( void )
{
	ClearList( FTWSESearchKeys );
	ClearList( FOTCSearchKeys );
	ClearList( FTWSEWarrantSearchKeys );
	ClearList( FOTCWarrantSearchKeys );
	ClearList( FTAIFEXFuturesSearchKeys );
	ClearList( FTAIFEXOptionsSearchKeys );
	AddCatalog();
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddTAIFEX( String Exchange )
{
	UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExchangeSymbols;
	AnsiString AnsiExchange( Exchange );

	if((ExchangeSymbols = CMarketDataStore->GetSymbolsByExchange( AnsiExchange.c_str() )) != NULL )
	{
		BasicInformation* Info = ExchangeSymbols->First();
		int               CCount = 1;

		while( Info != NULL )
		{
			UFC::AnsiString Symbol( Info->GetSymbol() );
			UFC::AnsiString StockID;
			String          DispName, Stock;
			TFutProdType    Type = CMarketDataStore->TAIFEXProductType( Info->GetProductID() );

			if( Info->GetMarket() == nsOrderMessageDefine::mTWFutures ) ///< Futures
			{
				if( Symbol.AnsiPos('/') < 0 && Symbol.Length() >= 5 ) ///< Time/Price spread product
				{
					if( Type == fptStock )
					{
						CMarketDataStore->TAIFEXStockProduct( Info->GetProductID(), StockID );
						Stock = StockID.c_str();
						DispName.printf( L"[%s] %s %s", String( Info->GetSymbol().c_str()) ,Info->GetDisplayName(), Stock );
					}
					else
						DispName.printf( L"[%s] %s", String( Info->GetSymbol().c_str()) ,Info->GetDisplayName() );
					FTAIFEXFuturesSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
				}
			}
			else ///< Options
			{
				if( Symbol.Length() >= 5 )
				{
					if( Type == fptStock )
					{
						//CMarketDataStore->TAIFEXStockProduct( Info->GetProductID(), StockID );
						//Stock = StockID.c_str();
						//DispName.printf( L"[%s] %s %s", String( Info->GetSymbol().c_str()) ,Info->GetDisplayName(), Stock );
						//FTAIFEXOptionsSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
					}
					else
					{
						DispName.printf( L"[%s] %s", String(Info->GetSymbol().c_str()) ,Info->GetDisplayName() );
						FTAIFEXOptionsSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
					}
				}
			}
			Info = ExchangeSymbols->Next();
			CCount++;
			if( CCount%MSG_PUMP_TIMES == 0 )
				Application->ProcessMessages();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddStock( String Exchange )
{
	UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExchangeSymbols;
	AnsiString AnsiExchange( Exchange );

	if((ExchangeSymbols = CMarketDataStore->GetSymbolsByExchange( AnsiExchange.c_str() )) != NULL )
	{
		String            ChineseName,DispName;
		BasicInformation* Info = ExchangeSymbols->First();
		int               CCount = 1;

		while( Info != NULL )
		{
			UFC::AnsiString UTF8ChineseName( Info->GetChineseName() ) ;

			ChineseName = UTF8ToString( UTF8ChineseName.c_str() );
			DispName.printf(L"[%s] %s", String(Info->GetSymbol().c_str()) ,ChineseName );
			if( Info->IsWarrant( ) == true )
			{
				if( Info->GetMarket() == ::mTSE )
					FTWSEWarrantSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
				else
					FOTCWarrantSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
			}
			else if( Info->GetTradeFlag() == true )
			{
				UFC::AnsiString ProductID ( Info->GetProductID() );

				if( ProductID.Length() > 0 && ProductID != "00" && ProductID != "A1" && ProductID != "A2")
				{
					if( Info->GetMarket() == ::mTSE )
						FTWSESearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
					else
						FOTCSearchKeys->AddObject( DispName, new TSymbolItem( Info ) );
				}
			}
			Info = ExchangeSymbols->Next();
			CCount++;
			if( CCount%MSG_PUMP_TIMES == 0 )
				Application->ProcessMessages();
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TContractViewerForm::OpenTBarOrderBookForm( String Exchange, String Symbol, int Index )
{
	TDepthForm*  SelectedForm = NULL;
	int          rtnIndex = -1;

	if( Index == -1 || FDepthForms.ItemCount() <= Index ) ///< Open New
	{
		SelectedForm = OpenOrderBookForm( Exchange, Symbol, true );
		rtnIndex = FDepthForms.IndexOf( SelectedForm );
	}
	else
	{
		SelectedForm = FDepthForms[ Index ] ;
		if( SelectedForm->GetGroup() != 1 ) ///< Not in Options page
		{
			SelectedForm = OpenOrderBookForm( Exchange, Symbol, true );
			rtnIndex = FDepthForms.IndexOf( SelectedForm );
		}
		else
		{
			rtnIndex = Index;
			if( SelectedForm != NULL )
				SelectedForm->ChangeSymbol( Exchange, Symbol );
		}
	}
	if( SelectedForm != NULL )
	{
		SelectedForm->SetFocus();
		SelectedForm->BringToFront();
	}
	return rtnIndex;
}
//---------------------------------------------------------------------------
TDepthForm* __fastcall TContractViewerForm::NewDepthForm( TWinControl* Owner, int Page, UFC::List<TDepthForm*>* DepthFormList, AnsiString& Exchange, AnsiString& Symbol )
{
	TDepthForm*  NewForm;
	int          left,top;
	String       Profile;

	NewForm = new TDepthForm( Owner, Page, Exchange, Symbol ); ///< Create Depth form
	NewForm->Tag = FDepthForms.ItemCount();
	DepthFormList->Add( NewForm );   ///< Add to depth list by exchange, symbol.
	FDepthForms.Add( NewForm );      ///< Add to all depth form list
	NewForm->OnClose = OnClientClose;///< Handle on close event
	FFormList.Add( NewForm );        ///< Add to all form list.
	ActiveClient( NewForm );         ///< Set to active form.
	ClientPos( mdfDepth, Page, left, top );
	Profile = L"SpeedyUnify\\Depth" + IntToStr( NewForm->Tag );
	NewForm->ShowDepth( left , top, Profile );
	return NewForm;
}
//---------------------------------------------------------------------------
bool __fastcall TContractViewerForm::CheckSymbol( const String& Ex, String& Sym )
{
	BasicInformation* Info;
	if( (Info = CMarketDataStore->GetBasicInformation( Ex,Sym, false )) != NULL)
	{
		Sym = Info->GetSymbol().c_str();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::SaveProperties( void )
{
	String Name;
	int KBarC = 0;
	int ListC = 0;

	for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		TDepthForm* SaveForm = FDepthForms.GetItem( i );

		Name = L"SpeedyUnify\\Depth" + IntToStr( i );
		g_Config.SetDesktopString(  Name, L"Exchange", SaveForm->GetExchange() );
		g_Config.SetDesktopString(  Name, L"Symbol",   SaveForm->GetSymbolAlias() );
		g_Config.SetDesktopInteger( Name, L"Page",     SaveForm->GetGroup() );
		g_Config.SetDesktopFormPos( Name, L"Form",     SaveForm->RoundFormEx );
		SaveForm->SaveProperty( Name );
	}
	g_Config.SetDesktopInteger(  "SpeedyUnify", "DepthFormCount", FDepthForms.ItemCount());
	for( int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfKBar )
		{
			TLineChartForm* SaveForm = dynamic_cast<TLineChartForm*>(FFormList[ i ]->GetTForm());
			if( SaveForm != NULL)
			{
				Name = L"SpeedyUnify\\KBar" + IntToStr( KBarC );
				g_Config.SetDesktopString(  Name, L"Exchange", SaveForm->GetExchange() );
				g_Config.SetDesktopString(  Name, L"Symbol",   SaveForm->GetSymbolAlias() );
				g_Config.SetDesktopInteger( Name, L"Page",     SaveForm->GetGroup() );
				g_Config.SetDesktopFormPos( Name, L"Form",     SaveForm->RoundFormEx );
				SaveForm->SaveProperties( Name );
				KBarC++;
			}
		}
		else if( FFormList[i]->Type() == mdfList )
		{
			TContractListForm* SaveForm = dynamic_cast<TContractListForm*>(FFormList[ i ]->GetTForm());
			if( SaveForm != NULL)
			{
				Name = L"SpeedyUnify\\List" + IntToStr( ListC );
				g_Config.SetDesktopInteger( Name, L"Page",     SaveForm->GetGroup() );
				g_Config.SetDesktopFormPos( Name, L"Form",     SaveForm->RoundFormEx );
				SaveForm->SaveProperties( Name );
				ListC++;
			}
		}
		else if( FFormList[i]->Type() == mdfExec )
		{
			TExecutionForm* SaveForm = dynamic_cast<TExecutionForm*>(FFormList[ i ]->GetTForm());
			SaveForm->SaveProperties();
		}
	}
	g_Config.SetDesktopInteger( "SpeedyUnify", "KBarFormCount", KBarC );
	g_Config.SetDesktopInteger( "SpeedyUnify", "ListFormCount", ListC );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadProperties( void )
{
	String Name,Exchange,Symbol,Msg;
	int DepthFormCount = g_Config.GetDesktopInteger( "SpeedyUnify", "DepthFormCount", 0 );
	int KBarFormCount  = g_Config.GetDesktopInteger( "SpeedyUnify", "KBarFormCount", 0 );
	int ListFormCount  = g_Config.GetDesktopInteger( "SpeedyUnify", "ListFormCount", 0 );
	int Total = DepthFormCount + KBarFormCount + ListFormCount;
	int LoadCount = 0;
    int Page;

	MainForm->LoadingProgress( 5, L"讀取券商設定...");
	Application->ProcessMessages();
	CloseAllForm();
	///< Load DepthForms
	for( int i = 0; i < DepthFormCount; i ++ )
	{
		Name   	 = L"SpeedyUnify\\Depth" + IntToStr( i );
		Page     = g_Config.GetDesktopInteger( Name, L"Page", 0 );
		Exchange = g_Config.GetDesktopString( Name,  L"Exchange", "" );
		Symbol 	 = g_Config.GetDesktopString( Name,  L"Symbol", "" );
		if( CheckSymbol( Exchange, Symbol ) == true )
		{
			AnsiString  exchange(Exchange);
			AnsiString  SymbolAlias(Symbol);
			TDepthForm* NewDepthForm = OpenOrderBookForm( exchange.c_str(), SymbolAlias.c_str(), true, Page );
			if( NewDepthForm != NULL)
			{
				NewDepthForm->LoadProperty( Name );
				NewDepthForm->DefaultMonitor = dmDesktop;
				g_Config.GetDesktopFormPos( Name, "Form", NewDepthForm->RoundFormEx, true );
			}
			LoadCount++;
			Msg.printf( L"讀取閃電下單視窗%d設定...", i + 1);
			MainForm->LoadingProgress( (int)((double)LoadCount*100/(double)Total), Msg );
			Application->ProcessMessages();
		}
	}
	if( FDepthForms.ItemCount() > 0 )
		FDepthForms[0]->PrintStopSetting();
	///< Load Chart Forms
	for( int i = 0; i < KBarFormCount; i ++ )
	{
		Name   	 = L"SpeedyUnify\\KBar" + IntToStr( i );
		Page     = g_Config.GetDesktopInteger( Name, L"Page", 0 );
		Exchange = g_Config.GetDesktopString( Name,  L"Exchange", "" );
		Symbol 	 = g_Config.GetDesktopString( Name,  L"Symbol", "" );
		if( CheckSymbol( Exchange, Symbol ) == true )
		{
			AnsiString  exchange(Exchange);
			AnsiString  SymbolAlias(Symbol);
			TLineChartForm* NewKBarForm = OpenKBarForm( exchange.c_str(), SymbolAlias.c_str(), Page );
			if( NewKBarForm != NULL )
			{
				NewKBarForm->DefaultMonitor = dmDesktop;
				g_Config.GetDesktopFormPos( Name, "Form", NewKBarForm->RoundFormEx, true );
				LoadCount++;
				Msg.printf( L"讀取K線圖視窗%d設定...", i + 1);
				MainForm->LoadingProgress( (int)((double)LoadCount*100/(double)Total), Msg );
				WinSleep( 50, 1 );
				NewKBarForm->LoadProperties( Name );
			}
		}
	}
	///< Load Market Data List Forms
	for( int i = 0; i < ListFormCount; i ++ )
	{
		Name  = L"SpeedyUnify\\List" + IntToStr( i );
		Page  = g_Config.GetDesktopInteger( Name, L"Page", 0 );
		TContractListForm* NewListForm = OpenMarketDataListForm( Page );
		if( NewListForm != NULL)
		{
			NewListForm->LoadProperties( Name );
			g_Config.GetDesktopFormPos( Name, "Form", NewListForm->RoundFormEx, true );
		}
		LoadCount++;
		Msg.printf( L"讀取行情表視窗%d設定...", i + 1);
		MainForm->LoadingProgress( (int)((double)LoadCount*100/(double)Total), Msg );
		Application->ProcessMessages();
	}
	///< Show loading finished.
	MainForm->LoadingProgress( 100, L"讀取完成!" );
	for( int i = 0; i < 20; i++)
	{
		Application->ProcessMessages();
		UFC::SleepMS( 10 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CloseOrderBookForm( String Exchange, String Symbol )
{
	UFC::List<TDepthForm*>*  SymbolDepthFormList;
	TDepthForm*              DelForm;

	if( (SymbolDepthFormList = SearchListFromMap( Exchange, Symbol ) )!= NULL )
	{
		for( int i = SymbolDepthFormList->ItemCount()-1; i >= 0; i-- )
		{
			DelForm = SymbolDepthFormList->GetItem( i );
			DelForm->Close();
		}
		ContractInfoForm->FutDrawGrid->Invalidate();
		ContractInfoForm->OptDrawGrid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ChangeSymbol( const String& OrigEx, const String& OrigSymbol,
												   const String& NewEx, const String& NewSymbol,
												   TDepthForm* ChangeForm )
{
	UFC::List<TDepthForm*>*  SymbolDepthFormList;
	TDepthForm*              MoveForm;

	if( ChangeForm != NULL )
	{
		UFC::List<TDepthForm*>*  SymbolDepthFormList;
		AnsiString               OrigKey,NewKey;
		String                   UnicodeKey;

		UnicodeKey.printf( L"%s.%s", OrigEx.c_str(), OrigSymbol.c_str() );
		OrigKey = UnicodeKey;
		UnicodeKey.printf( L"%s.%s", NewEx.c_str(), NewSymbol.c_str() );
		NewKey = UnicodeKey;
		if((SymbolDepthFormList = FSymbolFormMap.GetObjectByKey( OrigKey.c_str() )) != NULL )
		{   ///< Remove the old one.
			for( int i = 0; i < SymbolDepthFormList->ItemCount(); i++ )
			{
				if( ChangeForm == SymbolDepthFormList->GetItem( i ) )
				{
					SymbolDepthFormList->Delete( i );
					break;
				}
			}
			if( SymbolDepthFormList->ItemCount( )== 0 )
			{
				FSymbolFormMap.DeleteByKey( OrigKey.c_str() );
				delete SymbolDepthFormList;
			}
			if( (SymbolDepthFormList=FSymbolFormMap.GetObjectByKey( NewKey.c_str() ))==NULL )
			{
				SymbolDepthFormList = new UFC::List<TDepthForm*>();
				FSymbolFormMap.Add( NewKey.c_str(), SymbolDepthFormList );///< Add to map
            }
    		SymbolDepthFormList->Add( ChangeForm );
	        ContractInfoForm->FutDrawGrid->Invalidate();
    	    ContractInfoForm->OptDrawGrid->Invalidate();
        }
    }
}
//---------------------------------------------------------------------------
bool __fastcall TContractViewerForm::IsDepthViewOpen( const String& Exchange, const String& Symbol )
{
	String UnicodeKey;

	UnicodeKey.printf( L"%s.%s", Exchange.c_str(), Symbol.c_str() );
	AnsiString AnsiKey = UnicodeKey;
	if( FSymbolFormMap.GetObjectByKey( AnsiKey.c_str() ) != NULL )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CloseDepthForms( void )
{
	while( FDepthForms.ItemCount() > 0  )
	{
		TForm* DelForm = FDepthForms.GetItem( 0 );
		if( DelForm != NULL )
			DelForm->Close();
	}
	FDepthForms.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::EnableDepthForms( bool IsEnable )
{
	for( int i = 0; i < FDepthForms.ItemCount(); i++ )
	{
		TForm* Form = FDepthForms[ i ];
		if( Form != NULL )
			Form->Enabled = IsEnable;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadHotkey( TDepthForm* Who )
{
	for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		TDepthForm* UpdateForm = FDepthForms.GetItem( i );
		if( UpdateForm != NULL && UpdateForm != Who )
			UpdateForm->LoadHotkey();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::UpdateContract(  const String& Exchange  )
{
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::EnableAutoLots( bool IsEnable )
{
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::EnableStepOrder( bool IsEnable )
{
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::EnableDelBeforeNew( bool IsEnable )
{
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CenterPx( TObject *Sender )
{
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::UpdateBalance( const String& Ex, const String& Sym, double Balance )
{
	UFC::List<TDepthForm*>*  SymbolDepthFormList;
	TDepthForm*              UpdateForm;

	if((SymbolDepthFormList = SearchListFromMap( Ex, Sym ) ) != NULL )
	{
		for( int i = 0;i < SymbolDepthFormList->ItemCount(); i ++ )
		{
			UpdateForm = SymbolDepthFormList->GetItem( i );
			UpdateForm->UpdateBalance( Balance );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ContractTreeViewEditing(TObject *Sender, TTreeNode *Node,
		  bool &AllowEdit)
{
	AllowEdit = false;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ShowDepthMenuItemClick(TObject *Sender)
{
	if( ResultListBox->ItemIndex != -1 )
	{
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ ResultListBox->ItemIndex ];
		if( Item->Info != NULL)
		{
			BasicInformation* Info = Item->Info;
			OpenOrderBookForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str(), true );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CloseDepthMenuItemClick(TObject *Sender)
{
	if( ResultListBox->ItemIndex != -1 )
	{
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ ResultListBox->ItemIndex ];
		if( Item->Info != NULL)
		{
			BasicInformation* Info = Item->Info;
			CloseOrderBookForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ChartMenuItemClick(TObject *Sender)
{
	if( ResultListBox->ItemIndex != -1 )
	{
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ ResultListBox->ItemIndex ];
		if( Item->Info != NULL)
		{
			BasicInformation* Info = Item->Info;
			OpenKBarForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddToMDListMenuItemClick(TObject *Sender)
{
	TMenuItem* MenuItem = dynamic_cast<TMenuItem*>(Sender);
	if( ResultListBox->ItemIndex != -1 && MenuItem != NULL )
	{
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ ResultListBox->ItemIndex ];
		if( Item->Info != NULL)
		{
			BasicInformation* Info = Item->Info;
			ContractViewerForm->AddCustomSymbol( MenuItem->Tag, Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		}
	}
}
//---------------------------------------------------------------------------
UFC::List<TDepthForm*>* __fastcall TContractViewerForm::SearchListFromMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	return FSymbolFormMap.GetObjectByKey( Key );
}
//---------------------------------------------------------------------------
UFC::List<TDepthForm*>* __fastcall TContractViewerForm::SearchListFromMap( String Exchange, String Symbol )
{
	AnsiString Ex  = Exchange;
	AnsiString Sym = Symbol;
	UFC::AnsiString Key;

	Key.Printf( "%s.%s", Ex.c_str(), Sym.c_str() );
	return FSymbolFormMap.GetObjectByKey( Key );
}

//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddToMap( String Exchange, String Symbol, UFC::List<TDepthForm*>* AddList )
{
	AnsiString Ex  = Exchange;
	AnsiString Sym = Symbol;
	UFC::AnsiString Key;

	Key.Printf( "%s.%s", Ex.c_str(), Sym.c_str() );
	FSymbolFormMap.Add( Key, AddList );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddToMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UFC::List<TDepthForm*>* AddList )
{
	UFC::AnsiString Key;

	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	FSymbolFormMap.Add( Key, AddList );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::RemoveFromMap( String Exchange, String Symbol, UFC::List<TDepthForm*>* DelList )
{
	AnsiString Ex  = Exchange;
	AnsiString Sym = Symbol;
	UFC::AnsiString Key;

	Key.Printf( "%s.%s", Ex.c_str(), Sym.c_str() );
	FSymbolFormMap.DeleteByKey( Key );
	if( DelList != NULL )
		delete DelList;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::RemoveFromMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UFC::List<TDepthForm*>* DelList )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	FSymbolFormMap.DeleteByKey( Key );
	if( DelList != NULL )
		delete DelList;
}
//---------------------------------------------------------------------------
TDepthForm* __fastcall TContractViewerForm::DepthFormInPage( UFC::List<TDepthForm*>* ListPtr,  int Page )
{
	if( Page == -1 )
	{
		if( ListPtr->ItemCount() > 0 )
			return ListPtr->GetItem( 0 );
	}
	else
	{
		for( register int i = 0;i < ListPtr->ItemCount(); i ++ )
		{
			if( ListPtr->GetItem( i )->GetGroup() == Page )
				return ListPtr->GetItem( i );
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
bool __fastcall TContractViewerForm::IsListExists( String Exchange, String Symbol, int Page )
{
	UFC::List<TDepthForm*>* ListPtr = SearchListFromMap( Exchange, Symbol );
	if( ListPtr != NULL  )
	{
		if( DepthFormInPage( ListPtr, Page ) != NULL)
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TContractViewerForm::IsListExists( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int Page )
{
	UFC::List<TDepthForm*>* ListPtr = SearchListFromMap( Exchange, Symbol );
	if( ListPtr != NULL  )
	{
		if( DepthFormInPage( ListPtr, Page ) != NULL)
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::Search( const String& SearchStr, TStringList *SearchKeys )
{
	TStringList* TempResult = new TStringList();
	for( register int i = 1;i< SearchKeys->Count; i ++ )
	{
		if( SearchKeys->Strings[ i ].Pos( SearchStr ) != 0 )
			TempResult->AddObject( SearchKeys->Strings[i], SearchKeys->Objects[i] );
	}
	TempResult->CustomSort( SortCompare );
	if( TempResult->Count > 0 )
	{
		FResult->AddObject( SearchKeys->Strings[0], SearchKeys->Objects[0] );
		FResult->AddStrings( TempResult );
	}
	delete TempResult;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::SearchGlyphButtonClick(TObject *Sender)
{
	String SearchStr = SearchEdit->Text.UpperCase();

	FResult->Clear();
	if( SearchStr.Length( ) > 0 )
	{
		Screen->Cursor = crHourGlass;
		//Search( SearchStr, FTWSESearchKeys );
		//Search( SearchStr, FOTCSearchKeys );
		//Search( SearchStr, FTWSEWarrantSearchKeys );
		//Search( SearchStr, FOTCWarrantSearchKeys );
		Search( SearchStr, FTAIFEXFuturesSearchKeys );
		Search( SearchStr, FTAIFEXOptionsSearchKeys );
		Screen->Cursor = crDefault;
	}
	ResultListBox->Count = FResult->Count;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::SearchEditKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift)
{
	if( Key == VK_RETURN )
		SearchGlyphButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxDblClick(TObject *Sender)
{
	int Index = ResultListBox->ItemIndex;
	TSymbolItem* Item;

	if( Index != -1 )
	{
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ Index ];
		if( Item->Info != NULL)
		{
			if( FOnSelectSymbol != NULL )
				FOnSelectSymbol( this, satSymbol, Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str() );
			Hide();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxContextPopup(TObject *Sender, TPoint &MousePos,
		  bool &Handled)
{
	int CurserIndex;
	TSymbolItem* Item;

	if( ResultListBox->ItemIndex != -1 &&
		(CurserIndex = ResultListBox->ItemAtPos( MousePos, true)) != -1 &&
		ResultListBox->Selected[ CurserIndex ] )
	{
		if( (Item = (TSymbolItem*)FResult->Objects[ CurserIndex ]) != NULL )
		{
			BasicInformation* Info = Item->Info;
			if( Info != NULL )
			{
				if( Info->GetTradeFlag() )
				{
					if( IsDepthViewOpen( Info->GetExchange().c_str() ,Info->GetSymbol().c_str() ) )
					{
						ShowDepthMenuItem->Enabled = false;
						CloseDepthMenuItem->Enabled = true;
					}
					else
					{
						ShowDepthMenuItem->Enabled = true;
						CloseDepthMenuItem->Enabled = false;
					}
				}
				else
				{
					ShowDepthMenuItem->Enabled = false;
					CloseDepthMenuItem->Enabled = false;
					ChartMenuItem->Enabled = false;
				}
				Handled = false;
				PopupMenu->Tag = 1;
			}
			else
				Handled = true;
		}
		else
			Handled = true;
	}
	else
		Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxData(TWinControl *Control, int Index,
          UnicodeString &Data)
{
	if( Index >= 0 && Index < FResult->Count )
		Data = FResult->Strings[ Index ];
	else
		Data = L"---";
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxDataObject(TWinControl *Control,
		  int Index, TObject *&DataObject)
{
	if( Index >= 0 && Index < FResult->Count )
		DataObject = FResult->Objects[ Index ];
	else
		DataObject = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxDrawItem(TWinControl *Control, int Index,
		  TRect &Rect, TOwnerDrawState State)
{
	TRect PaintRect = TRect( 0, 0, Rect.Width(), Rect.Height() );
	if( FBufferBmp->Height != Rect.Height() )
		FBufferBmp->Height = Rect.Height();
	if( FBufferBmp->Width != Rect.Width() )
		FBufferBmp->Width = Rect.Width();

	if( Index >= 0 && Index < FResult->Count )
	{
		TRect SymbolRect = TRect( 0, 0, 120, Rect.Height() );
		TRect NameRect = TRect( 120, 0, Rect.Width(), Rect.Height() );
		TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ Index ];
		if( Item->Info != NULL)
		{
			TTextFormat Formats;

			if( State.Contains( odSelected ) )
			{
				FBufferBmp->Canvas->Brush->Color = clNavy;
				FBufferBmp->Canvas->Font->Color  = clWhite;
			}
			else
			{
				FBufferBmp->Canvas->Brush->Color = clWhite;
				FBufferBmp->Canvas->Font->Color  = clNavy;
			}
			FBufferBmp->Canvas->FillRect( PaintRect );
			Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
			FBufferBmp->Canvas->TextRect( SymbolRect, Item->Symbol, Formats );
			FBufferBmp->Canvas->TextRect( NameRect, Item->ChineseName, Formats );
			IconImageList->Draw( FBufferBmp->Canvas, Rect.Width()-16, 0, 1, true );
			IconImageList->Draw( FBufferBmp->Canvas, Rect.Width()-32, 0, 0, true );
		}
		else
		{
			TTextFormat Formats;

			FBufferBmp->Canvas->Brush->Color = clBlack;
			FBufferBmp->Canvas->Font->Color  = clWhite;
			SymbolRect.Bottom = SymbolRect.Bottom + 10;
			FBufferBmp->Canvas->RoundRect( SymbolRect, 10, 10);
			SymbolRect.Bottom = SymbolRect.Bottom - 10;
			Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
			FBufferBmp->Canvas->TextRect( SymbolRect, Item->Symbol, Formats );
			FBufferBmp->Canvas->Brush->Color = clWhite;
			FBufferBmp->Canvas->Font->Color  = clWhite;
			FBufferBmp->Canvas->FillRect( NameRect );
			FBufferBmp->Canvas->Pen->Color = clBlack;
			FBufferBmp->Canvas->Pen->Width = 2;
			FBufferBmp->Canvas->MoveTo( 0, Rect.Height() -1 );
			FBufferBmp->Canvas->LineTo( Rect.Width(), Rect.Height() -1 );
		}
	}
	else
	{
		FBufferBmp->Canvas->Font->Color  = clWhite;
		FBufferBmp->Canvas->FillRect( PaintRect );
	}
	ResultListBox->Canvas->Draw( Rect.Left, Rect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y)
{
	TPoint Pos( X , Y);
	int Index = ResultListBox->ItemAtPos( Pos , true );
	if( Index != -1 )
	{
		if( X > ResultListBox->Width - 32 )
		{
			if( Index >= 0 && Index < FResult->Count )
			{
				TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ Index ];
				if( Item->Info != NULL)
				{
					Screen->Cursor = crHandPoint;
					return;
				}
			}
		}
	}
	Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxMouseLeave(TObject *Sender)
{
	Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::OnDepthFormClose( TDepthForm* DelForm )
{
	if( DelForm != NULL )
	{
		UFC::List<TDepthForm*>*  SymbolDepthFormList;
		String                   DelExAndSym;
		AnsiString               AnsiExAndSym;

		DelExAndSym.printf( L"%s.%s", DelForm->GetExchange().c_str(), DelForm->GetSymbol().c_str() );
		AnsiExAndSym = DelExAndSym;

		for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
		{
			if( FDepthForms[i] == DelForm )
			{
				FDepthForms.Delete( i );
				break;
			}
		}
		if((SymbolDepthFormList = FSymbolFormMap.GetObjectByKey( AnsiExAndSym.c_str() )) != NULL )
		{
			for( int i = 0;i < SymbolDepthFormList->ItemCount(); i ++ )
			{
				if( SymbolDepthFormList->GetItem( i ) == DelForm )
				{
					SymbolDepthFormList->Delete( i );
					break;
				}
			}
			if( SymbolDepthFormList->ItemCount() == 0 )
			{
				FSymbolFormMap.DeleteByKey( AnsiExAndSym.c_str() );
				delete SymbolDepthFormList;
			}
		}
		for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
			FDepthForms[i]->Tag = i;
		ContractInfoForm->FutDrawGrid->Invalidate();
		ContractInfoForm->OptDrawGrid->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::OnClientClose(TObject *Sender, TCloseAction &Action)
{
	TForm*       DelForm = (TForm*) Sender;

	for( int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( DelForm == FFormList[i]->GetTForm() ) ///< Bingo! Close this form.
		{
			TClientForm* Del = FFormList[i];

			if( Del->Type() == mdfDepth ) ///< Depth form
			{
				TDepthForm* CloseDepth = (TDepthForm*)Del;

				if( CloseDepth->CanClose() == false ) ///< Has working order, still close it ?
				{
					Action = System::Uitypes::TCloseAction::caNone;
					return;
				}
				OnDepthFormClose( CloseDepth );
			}
			FFormList.Delete( i );
			if( Del->IsActive() == true )
				ActiveNextClient( Del->GetGroup() );
			Del->Release();
			Action = caFree;
			return;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ActiveNextClient( int Group )
{
	bool First = false;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Group )
		{
			if( First == false )
			{
				FFormList[i]->SetActive( true );
				First = true;
			}
			else
				FFormList[i]->SetActive( false );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ShowPage( int Page )
{
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Page )
			FFormList[i]->SetVisible( true );
		else
			FFormList[i]->SetVisible( false );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ActiveClient( TClientForm* ActForm )
{
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == ActForm->GetGroup() )
			FFormList[i]->SetActive( FFormList[i] == ActForm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ClientPos( TMDFormType Type, int Group, int& L, int& T )
{
	int CCount = ClientCount( Type, Group );
	T = ( CCount%10 )* 25;
	L = ( CCount/10)*200 + ( CCount%10 )* 25;
}
//---------------------------------------------------------------------------
int __fastcall TContractViewerForm::ClientCount( TMDFormType Type, int Group )
{
	int ClientCount = 0;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Group && FFormList[i]->Type() == Type )
			ClientCount ++;
	}
	return ClientCount;
}
//---------------------------------------------------------------------------
int __fastcall TContractViewerForm::ClientCount( TMDFormType Type )
{
	int ClientCount = 0;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if(  FFormList[i]->Type() == Type )
			ClientCount ++;
	}
	return ClientCount;
}
//---------------------------------------------------------------------------
TContractListForm*  __fastcall TContractViewerForm::FirstContractListForm( int Group )
{
	TContractListForm* ListForm;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Group && FFormList[i]->Type() == mdfList )
		{
			ListForm = dynamic_cast<TContractListForm*>( FFormList[i]->GetTForm() );
			return ListForm;
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
TExecutionForm*  __fastcall TContractViewerForm::FirstExecutionForm( int Group )
{
	TExecutionForm* ExecForm;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Group && FFormList[i]->Type() == mdfExec )
		{
			ExecForm = dynamic_cast<TExecutionForm*>( FFormList[i]->GetTForm() );
			return ExecForm;
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
TOCODetailForm* __fastcall TContractViewerForm::FirstOCODetailForm( int Group )
{
	TOCODetailForm* detailForm;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->GetGroup() == Group && FFormList[i]->Type() == mdfOCO)
		{
			detailForm = dynamic_cast<TOCODetailForm*>( FFormList[i]->GetTForm() );
			return detailForm;
		}
	}
    return NULL;
}
//---------------------------------------------------------------------------
TExecutionForm*  __fastcall TContractViewerForm::FirstExecutionForm( void )
{
	TExecutionForm* ExecForm;
	for( register int i = 0; i < FFormList.ItemCount(); i++ )
	{
		if( FFormList[i]->Type() == mdfExec )
		{
			ExecForm = dynamic_cast<TExecutionForm*>( FFormList[i]->GetTForm() );
			return ExecForm;
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
void  __fastcall TContractViewerForm::OpenTBarForm( void )
{
	if( FTBarOpened == false )
	{
		TOptionsTBarForm *TBarForm;
		TBarForm = new TOptionsTBarForm( MainForm->PageControl->Pages[1], 0 );
		TBarForm->Tag     = MainForm->PageControl->ActivePageIndex;
		TBarForm->Parent  = MainForm->PageControl->ActivePage;
		TBarForm->BorderStyle = bsNone;
		TBarForm->Align   = alClient;
		TBarForm->OnClose = OnClientClose;
		TBarForm->RoundFormEx->Enable = false;
		FFormList.Add( TBarForm );
		TBarForm->LoadProperties( "TBarDef" );
		TBarForm->Show();
		TBarForm->SendToBack();
		FTBarOpened = true;
	}
}
//---------------------------------------------------------------------------
void  __fastcall TContractViewerForm::OpenExecutionForm( void )
{
	int Page = MainForm->PageControl->ActivePageIndex;

//	if(  ClientCount( mdfExec ) == 0 )
	if( ClientCount( mdfExec, Page ) == 0 )
	{
		TExecutionForm* NewExecForm;

		NewExecForm = new TExecutionForm( this );
		NewExecForm->Tag     = Page;
		NewExecForm->Parent  = /*NULL;*/MainForm->PageControl->ActivePage;
		NewExecForm->OnClose = OnClientClose;
		FFormList.Add( NewExecForm );
		NewExecForm->LoadProperties();
		NewExecForm->Show();
		ActiveClient( NewExecForm );
	}
	else
	{
		TExecutionForm* OldExecForm  = FirstExecutionForm( Page );

		OldExecForm->SetFocus();
		OldExecForm->BringToFront();
		OldExecForm->Show();
		ActiveClient( OldExecForm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::OpenOCODetailForm( void )
{
	int Page = MainForm->PageControl->ActivePageIndex;
	TOCODetailForm* detailForm;
	if( ClientCount( mdfOCO, Page ) == 0 )
	{
		detailForm = new TOCODetailForm( this );
		detailForm->Tag = Page;
		detailForm->Parent = MainForm->PageControl->ActivePage;
		detailForm->OnClose = OnClientClose;
		FFormList.Add( detailForm );
	}
	else
	{
		detailForm = FirstOCODetailForm( Page );
		detailForm->SetFocus();
		detailForm->BringToFront();
	}

	detailForm->Show();
	detailForm->SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CloseExecutionForm( void )
{
	TForm* DelForm;

	for( int i = FFormList.ItemCount() -1; i >= 0; i -- )
	{
		if(  FFormList[i]->Type() == mdfExec )
		{
			DelForm = FFormList[i]->GetTForm();
			DelForm->Close();
			Application->ProcessMessages();
		}
	}
	UFC::SleepMS( 200 );
}
//---------------------------------------------------------------------------
TDepthForm* __fastcall TContractViewerForm::OpenOrderBookForm( String Exchange, String Symbol, bool NewIfExist, int Page )
{
	UFC::List<TDepthForm*>*  SymbolDepthFormList;
	TDepthForm*  SelectedForm;
	AnsiString	 exchange( Exchange );
	AnsiString	 symbol( Symbol );
	int          PageIndex = Page;
	TWinControl* ActivePage;
	int          MaxCount = 20;

	if( gIsExpired == true )
		MaxCount = 2;
	if( FDepthForms.ItemCount() < MaxCount )
	{
		if( (gMarketDataStore->GetBasicInformation( exchange.c_str(), symbol.c_str(), false ))!= NULL )
		{
			if( PageIndex == -1 )
				PageIndex = MainForm->PageControl->ActivePageIndex;
			ActivePage = MainForm->PageControl->Pages[ PageIndex ];
			if( ActivePage == NULL )
				ActivePage = MainForm->PageControl->ActivePage;
			if( IsListExists( exchange.c_str(), symbol.c_str(), PageIndex ) == false ) ///< Not opened
			{ 	///< Create list of same symbol.
				SymbolDepthFormList = new UFC::List<TDepthForm*>();
				///< Add to map
				AddToMap( Exchange, Symbol, SymbolDepthFormList );
				///< Create Depth form
				SelectedForm = NewDepthForm( ActivePage, PageIndex,SymbolDepthFormList, exchange, symbol );
			}
			else ///< Already exist
			{
				if( (SymbolDepthFormList = SearchListFromMap( exchange.c_str(), symbol.c_str() )) != NULL )
				{
					if( NewIfExist == false ) ///< Already exists, Focus it.
					{
						if( (SelectedForm = DepthFormInPage( SymbolDepthFormList,  PageIndex )) != NULL )
						{
							SelectedForm->SetFocus();
							SelectedForm->BringToFront();
							SelectedForm->Show();
							ActiveClient( SelectedForm );
						}
					}
					else ///< Clone an new one, Create new Depth form
						SelectedForm = NewDepthForm( ActivePage, PageIndex, SymbolDepthFormList, exchange, symbol );
				}
			}
			if( ContractInfoForm->Visible == true )
			{
				ContractInfoForm->FutDrawGrid->Invalidate();
				ContractInfoForm->OptDrawGrid->Invalidate();
			}
			return  SelectedForm;
		}
	}
	else
	{
		if( gIsExpired == true )
		{
			TTrainingDlgForm::MessageDlg( L"謝謝您的試用!想開更多的閃電下單", L"請訂閱正式版." );
		}
		else
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"基於效能考量,最多只能開20個閃電下單" );
	}
	return NULL;
}
//---------------------------------------------------------------------------
TContractListForm*  __fastcall TContractViewerForm::OpenMarketDataListForm( int Page, bool Load )
{
	int MaxCount = 3;

	if( gIsExpired == true )
		MaxCount = 1;
	if( ClientCount( mdfList ) < MaxCount )
	{
		TContractListForm *ListForm;
		int left,top;
		int PageIndex = Page;
		TWinControl* ActivePage;

		if( PageIndex == -1 )
			PageIndex = MainForm->PageControl->ActivePageIndex;
		ActivePage = MainForm->PageControl->Pages[ PageIndex ];

		ListForm = new TContractListForm( ActivePage, ClientCount( mdfList ) );
		ListForm->Tag     = PageIndex;
		ListForm->Parent  = ActivePage;
		ListForm->OnClose = OnClientClose;
		ClientPos( mdfList, PageIndex, left, top );
		ListForm->Init( L"TAIFEX", ClientCount( mdfList ) );
		FFormList.Add( ListForm );
		ActiveClient( ListForm );
		if( Load == true )
		{
			String Name = L"SpeedyUnify\\List" + IntToStr( ListForm->GetID() );
			ListForm->LoadProperties( Name );
			g_Config.GetDesktopFormPos( Name, "Form", ListForm->RoundFormEx, true );
			ListForm->Show();
		}
		else
		{
			ListForm->Left = left;
			ListForm->Top = top;
			ListForm->Show();
		}
		return ListForm;
	}
	else
	{
		if( gIsExpired == true )
		{
			TTrainingDlgForm::MessageDlg( L"謝謝您的試用!想開更多的行情表", L"請訂閱正式版." );
		}
		else
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"基於效能考量,最多只能開三個行情表" );
		return NULL;
	}
}
//---------------------------------------------------------------------------
TLineChartForm*  __fastcall TContractViewerForm::OpenKBarForm( const String& Ex, const String& Sym, int Page )
{
	int MaxCount = 5;

	if( gIsExpired == true )
		MaxCount = 1;
	if( ClientCount( mdfKBar ) < MaxCount )
	{
		TLineChartForm* LineChartForm;
		int left,top;
		int PageIndex = Page;
		TWinControl* ActivePage;

		if( PageIndex == -1 )
			PageIndex = MainForm->PageControl->ActivePageIndex;
		ActivePage = MainForm->PageControl->Pages[ PageIndex ];
		LineChartForm = new TLineChartForm( ActivePage );
		LineChartForm->Tag     = PageIndex;
		LineChartForm->Parent  = ActivePage;
		LineChartForm->OnClose = OnClientClose;
		LineChartForm->LoadSetting();
		ClientPos( mdfKBar, PageIndex, left, top );
		FFormList.Add( LineChartForm );
		ActiveClient( LineChartForm );
		LineChartForm->Left = left;
		LineChartForm->Top = top;
		LineChartForm->Show();
		LineChartForm->RequestGraph( Ex, Sym );
		LineChartForm->LoadProperties( "Default" );
		return LineChartForm;
	}
	else
	{
		if( gIsExpired == true )
		{
			TTrainingDlgForm::MessageDlg( L"謝謝您的試用!想開更多個K線圖", L"請訂閱正式版." );
		}
		else
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"基於效能考量,最多只能開五個K線圖" );
		return NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CloseAllForm( void )
{
	TForm* DelForm;

	for( int i = FFormList.ItemCount() -1; i >= 0; i -- )
	{
		DelForm = FFormList[i]->GetTForm();
		DelForm->Close();
		Application->ProcessMessages();
	}
	UFC::SleepMS( 200 );
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	TPoint Pos( X , Y);
	int Index = ResultListBox->ItemAtPos( Pos , true );
	if( Index != -1 )
	{
		TRect Rect = ResultListBox->ItemRect( Index );
		if( X > Rect.Right - 32 )
		{
			if( Index >= 0 && Index < FResult->Count )
			{
				TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ Index ];
				if( Item->Info != NULL)
				{
					if( X > Rect.Right - 32 && X < Rect.Right - 16) //OpenOrderBookForm( Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str(), true );
					{
						if( FOnSelectSymbol != NULL )
							FOnSelectSymbol( this, satDepth, Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str() );
					}
					else //OpenKBarForm( Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str() );
					{
						if( FOnSelectSymbol != NULL )
							FOnSelectSymbol( this, satKChart, Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str() );
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::AddCustomSymbol( int CustomListIndex, const String& Exchange, const String& Symbol )
{
	int Page = MainForm->PageControl->ActivePageIndex;
	TContractListForm* DefContractList;

	if( ClientCount( mdfList, Page ) == 0)
		DefContractList = OpenMarketDataListForm( Page );
	else
		DefContractList = FirstContractListForm( Page );
	if( DefContractList != NULL)
	{
		DefContractList->AddSymbol( CustomListIndex, Exchange, Symbol );
		DefContractList->ToCustomList( CustomListIndex );
		DefContractList->Show();
		DefContractList->SaveCustomList( "SpeedyUnify" );
		LoadCustomList( DefContractList, CustomListIndex );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::CenterPx( TDepthForm* Who )
{
	for( register int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		if( FDepthForms[ i ] != Who )
			FDepthForms[ i ]->OrderBookList->CenterFillPx( true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadDepthColor( TDepthForm* Who )
{
	for( register int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		if( FDepthForms[ i ] != Who )
			FDepthForms[ i ]->LoadColor();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::OrderStoreReady( void )
{
	for( register int i = 0;i < FDepthForms.ItemCount(); i ++ )
		FDepthForms[ i ]->OnOrderStoreReady();
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadStopSetting( TClientForm* Who )
{
	for( register int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		if( FDepthForms[ i ] != Who )
			FDepthForms[ i ]->LoadStopSetting();
		else
			FDepthForms[ i ]->PrintStopSetting();
	}
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfKBar )
		{
			TLineChartForm* ReloadForm = dynamic_cast<TLineChartForm*>(FFormList[ i ]->GetTForm());
			if( ReloadForm != NULL && ReloadForm != Who )
				ReloadForm->LoadStopSetting();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadChartColor( TLineChartForm* Who )
{
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfKBar )
		{
			TLineChartForm* ReloadForm = dynamic_cast<TLineChartForm*>(FFormList[ i ]->GetTForm());
			if( ReloadForm != NULL && ReloadForm != Who )
				ReloadForm->LoadColor();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadChartKSetting( TLineChartForm* Who )
{
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfKBar )
		{
			TLineChartForm* ReloadForm = dynamic_cast<TLineChartForm*>(FFormList[ i ]->GetTForm());
			if( ReloadForm != NULL && ReloadForm != Who )
				ReloadForm->LoadKBarSetting();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadContractListColor( TContractListForm* Who )
{
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfList )
		{
			TContractListForm* ReloadForm = dynamic_cast<TContractListForm*>(FFormList[ i ]->GetTForm());
			if( ReloadForm != NULL && ReloadForm != Who )
				ReloadForm->LoadColor();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadCustomList( TContractListForm* Who, int Index )
{
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() != mdfList )
			continue;

		TContractListForm* ContractListForm = dynamic_cast<TContractListForm*>(FFormList[ i ]->GetTForm());
		if( ContractListForm == NULL || ContractListForm == Who )
			continue;

		ContractListForm->LoadCustomList( Index );
	}

	for( register int i = 0; i< FDepthForms.ItemCount(); i++)
	{
		TDepthForm* DepthForm = dynamic_cast<TDepthForm*>(FDepthForms[ i ]->GetTForm());
		if( DepthForm == NULL )
			continue;

		DepthForm->LoadCustomList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadCustomNames( void )
{
	for( int i = 0; i < CUSTOM_LIST_COUNT; i++ )
	{
		String KeyName,DefName, CustomName;
		TMenuItem* MItem = AddToMDListMenuItem->Items[i];

		DefName.printf( L"自選商品-%d", i + 1 );
		KeyName.printf( L"Name%d", i + 1 );
		CustomName = g_Config.GetStringProperty( "CustomNames",KeyName, DefName );
		MItem->Caption = CustomName;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::LoadCustomName( TContractListForm* Who )
{
	LoadCustomNames();///< TContractViewerForm Load Custom Names
	if( ContractInfoForm != NULL)
		ContractInfoForm->LoadCustomNames();
	if( SelectContractForm != NULL)
		SelectContractForm->LoadCustomNames();
	///< TContractListForm Load Custom Names
	for( register int i = 0;i < FFormList.ItemCount(); i ++ )
	{
		if( FFormList[i]->Type() == mdfList )
		{
			TContractListForm* ReloadForm = dynamic_cast<TContractListForm*>(FFormList[ i ]->GetTForm());
			if( ReloadForm != NULL && ReloadForm != Who )
				ReloadForm->LoadCustomNames( );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::RegisterOrderStore( void )
{
	for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		TDepthForm* SaveForm = FDepthForms.GetItem( i );

		SaveForm->RegisterOrderStore();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::UnregisterOrderStore( void )
{
	for( int i = 0;i < FDepthForms.ItemCount(); i ++ )
	{
		TDepthForm* SaveForm = FDepthForms.GetItem( i );

		SaveForm->UnregisterOrderStore();
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::FormDeactivate(TObject *Sender)
{
	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::ResultListBoxKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift)
{
	if( Key == VK_RETURN )
	{
		if( FResult->Count >= 2 )
		{
			TSymbolItem* Item = (TSymbolItem*)FResult->Objects[ 1 ];
			if( Item->Info != NULL &&  FOnSelectSymbol != NULL )
				FOnSelectSymbol( this, satSymbol, Item->Info->GetExchange().c_str(), Item->Info->GetSymbol().c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractViewerForm::FormShow(TObject *Sender)
{
	ResultListBox->SetFocus();
}
//---------------------------------------------------------------------------

