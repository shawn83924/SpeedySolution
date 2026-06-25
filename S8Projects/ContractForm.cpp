//---------------------------------------------------------------------------
#include <vcl.h>
#include "FMTConfig.h"
#include "main.h"
#pragma hdrstop
#include <WideStrUtils.hpp>
#include "ContractViewer.h"
#include "ContractList.h"
#include "ContractForm.h"
#include "OrderMessageDefine.h"
#include "UnifyDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TContractInfoForm *ContractInfoForm;
TContractInfoForm *SelectContractForm = NULL;
//---------------------------------------------------------------------------
const int    MSG_PUMP_TIMES = 50;
const int    FONT_SIZE = 10;
const String FONT_NAME( L"微軟正黑體" );
//---------------------------------------------------------------------------
extern TCMarketDataStore* gMarketDataStore;
//---------------------------------------------------------------------------
const TColor COL_CONTRACT_NONE   = clWhite;
const TColor COL_CONTRACT_NORMAL = clCream;
const TColor COL_CONTRACT_SHOWED = (TColor) 0x00EAEAEA;
const TColor COL_CONTRACT_SELECT = (TColor) 0x00DA9B11;
const TColor COL_GRID_LINE       = (TColor) 0x00F2F2F2;
//---------------------------------------------------------------------------
__fastcall TContractInfoForm::TContractInfoForm(TComponent* Owner)
:TForm(Owner)
,FPicker( false )
,FPickExchange( L"TAIFEX" )
,FPickSymbol( "" )
,FSupportFutures( false )
,FSupportOptions( false )
,FSupportStock( false )
,FInit( false )
,FChanging( false )
,STR_FUTURES( L"期貨")
,STR_OPTIONS( L"選擇權" )
,STR_STOCK( L"股票" )
,STR_WARRANT( L"認購權證" )
,STR_PUT_WARRANT( L"認售權證" )
,STR_CALL( L"買權" )
,STR_PUT( L"賣權" )
,FFutSelPage( -1 )
,FFutSelIndex( -1 )
{
	///< New search key-word string list
	FSearchKeys = new TStringList();
	FSearchResult = new TStringList();
	FDisplayList = FSearchResult;
	ResultListBox->Count = 0;
	///< New Exchange code list
	FOptExchangeCode = new TStringList();
	FFutExchangeCode = new TStringList();

	for( int i = 0; i < EXCHANGE_MAX; i ++ )
	{
		FOptChineseSerials[i] = NULL;
		for( int j = 0; j < TYPE_MAX; j++ )
		{
			FFutSerials[i][j]     = NULL;
			FChineseSerials[i][j] = NULL;
			FMaturityDate[i][j]   = NULL;
			FFutRowCount[i][j]    = 0;
			FFutColCount[i][j]    = 0;
		}
	}
	RoundFormEx->Lockbox = false;
	RoundFormEx->Enable = false;
	BorderStyle = bsNone;
}
//---------------------------------------------------------------------------
__fastcall TContractInfoForm::TContractInfoForm(TComponent* Owner, bool ForSelect )
:TForm(Owner)
,FPicker( true )
,FPickExchange( L"TAIFEX" )
,FPickSymbol( "" )
,FSupportFutures( false )
,FSupportOptions( false )
,FSupportStock( false )
,FInit( false )
,STR_FUTURES( L"期貨")
,STR_OPTIONS( L"選擇權" )
,STR_STOCK( L"股票" )
,STR_WARRANT( L"權證" )
,STR_PUT_WARRANT( L"認售權證" )
,STR_CALL( L"買權" )
,STR_PUT( L"賣權" )
{
	///< New search key-word string list
	FSearchKeys = new TStringList();
	FSearchResult = new TStringList();
	FDisplayList = FSearchResult;
	ResultListBox->Count = 0;
	///< New Exchange code list
	FOptExchangeCode = new TStringList();
	FFutExchangeCode = new TStringList();

	for( int i = 0; i < EXCHANGE_MAX; i ++ )
	{
		FOptChineseSerials[i] = NULL;
		for( int j = 0; j < TYPE_MAX; j++ )
		{
			FFutSerials[i][j]     = NULL;
			FChineseSerials[i][j] = NULL;
			FMaturityDate[i][j]   = NULL;
			FFutRowCount[i][j]    = 0;
			FFutColCount[i][j]    = 0;
		}
	}
	Position = poScreenCenter;
	FutDrawGrid->Options   = FutDrawGrid->Options>>goRangeSelect;
	FutDrawGrid->PopupMenu = NULL;
	OptDrawGrid->Options   = OptDrawGrid->Options>>goRangeSelect;
	OptDrawGrid->PopupMenu = NULL;
	ResultListBox->PopupMenu = NULL;
	this->BorderStyle = bsSizeable;
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_MDIACTIVATE:
			 Msg.Result = 1;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SwitchPage( int index )
{
	if( index == 0 )
	{
		PageControl->ActivePage = FutTabSheet;
		if( FPicker == true )
			ClientHeight        = FFutPageHeight;
		Width                   = FFutPageWidth;
		Constraints->MaxWidth   = FFutPageWidth + 10;
		Constraints->MinWidth   = FFutPageWidth;
		FutTabButton->Selected   = true;
		OptTabButton->Selected   = false;
		StockTabButton->Selected = false;
	}
	else if( index == 1 )
	{
		PageControl->ActivePage = OptTabSheet;
		if( FPicker == true )
			ClientHeight        = FOptPageHeight;
		Width                   = FOptPageWidth;
		Constraints->MaxWidth   = FOptPageWidth + 10;
		Constraints->MinWidth   = FOptPageWidth;
		FutTabButton->Selected   = false;
		OptTabButton->Selected   = true;
		StockTabButton->Selected = false;
	}
	else if( index == 2 )
	{
		PageControl->ActivePage = StockTabSheet;
		if( FPicker == true )
			ClientHeight        = 595;
		Width                   = 675;
		Constraints->MaxWidth   = 675;
		Constraints->MinWidth   = 675;
		FutTabButton->Selected   = false;
		OptTabButton->Selected   = false;
		StockTabButton->Selected = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddFuturesSymbol( int Index, BasicInformation* Info )
{
	TFutProdType Type = gMarketDataStore->TAIFEXProductType( Info->GetProductID() );
	int ti = TypeIndex( Type );
	UFC::AnsiString MaturityMonthYear( Info->GetMaturityDate() );
	UFC::AnsiString Prefix ( Info->GetProductID() );

	FSymbolList.Add( Prefix + MaturityMonthYear, new UFC::AnsiString( Info->GetSymbol() ) );
	if( FFutSymSet[ti].Exists( Prefix ) == FALSE )
	{
		FFutSymSet[ti].Add( Prefix );
		FFutSerials[ Index ][ti]->Add( Prefix.c_str() );
		FChineseSerials[ Index ][ti]->Add( gMarketDataStore->GetChinesePrefix( Info->GetExchange(), Prefix ) );
	}
	if( FFutMaturitySet[ti].Exists( MaturityMonthYear ) == FALSE )
	{
		FFutMaturitySet[ti].Add( MaturityMonthYear );
		FMaturityDate[Index][ti]->Add( MaturityMonthYear.c_str() );
	}
	FSupportFutures = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddOptionsSymbol( int OptExIndex, BasicInformation* Info, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString SymbolKey;
	UFC::AnsiString Serial( Info->GetProductID() );
	UFC::AnsiString MaturityMonthYear( Info->GetMaturityDate() );
	UFC::AnsiString StkPx;
	TOptionsSerial* OptionsSerial;
	int             StkPxDigi = gMarketDataStore->GetStrikePriceDigit( Info->GetExchange(), Symbol );
	char            CallPut   = (Info->GetCallPut() == cpCall) ? 'C' :'P';

	StkPx.Printf("%5.*f", StkPxDigi, Info->GetStrikePrice() );
	if( (OptionsSerial = FOptionsSerialList[ OptExIndex ].GetObjectByKey(Serial)) == NULL )
	{
		OptionsSerial = new TOptionsSerial();
		FOptionsSerialList[ OptExIndex ].Add( Serial.c_str(), OptionsSerial );
		FOptChineseSerials[ OptExIndex ]->Add( Serial.c_str() );
	}
	SymbolKey.Printf( "%s%s%c%s",Serial.c_str(),StkPx.c_str(),CallPut, MaturityMonthYear.c_str() );
	FSymbolList.Add( SymbolKey, new UFC::AnsiString( Symbol ) );
	if( OptionsSerial->StrikePxs->IndexOf( StkPx.c_str() )== -1 )
		OptionsSerial->StrikePxs->Add( StkPx.c_str() );
	if( OptionsSerial->Maturity->IndexOf( MaturityMonthYear.c_str() )== -1 )
		OptionsSerial->Maturity->Add( MaturityMonthYear.c_str() );
	FSupportOptions = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SortTAIFEXFutSerials( int Index )
{
	int SIndex;

	if( (SIndex = FFutSerials[Index][0]->IndexOf( L"TXF" ))!= -1 )
	{
		FFutSerials[Index][0]->Exchange( SIndex, 0 );
		FChineseSerials[Index][0]->Exchange( SIndex, 0 );
	}
	if( (SIndex = FFutSerials[Index][0]->IndexOf( L"MXF" ))!= -1 )
	{
		FFutSerials[Index][0]->Exchange(SIndex,1);
		FChineseSerials[Index][0]->Exchange(SIndex,1);
	}
	if( (SIndex = FFutSerials[Index][0]->IndexOf( L"TMF" ))!= -1 )
	{
		FFutSerials[Index][0]->Exchange(SIndex,2);
		FChineseSerials[Index][0]->Exchange(SIndex,2);
	}
	if( (SIndex = FFutSerials[Index][0]->IndexOf( L"EXF" ))!= -1 )
	{
		FFutSerials[Index][0]->Exchange(SIndex,3);
		FChineseSerials[Index][0]->Exchange(SIndex,3);
	}
	if( (SIndex = FFutSerials[Index][0]->IndexOf( L"FXF" ))!= -1 )
	{
		FFutSerials[Index][0]->Exchange(SIndex,4);
		FChineseSerials[Index][0]->Exchange(SIndex,4);
	}
}
//---------------------------------------------------------------------------
// TXO 台指選擇權
// TEO 電子選擇權
// TFO 金融選擇權
// XIO 非金電選擇權
// MSO 摩根指選擇權
// GTO 櫃買選擇權
// TGO 黃金選擇權
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SortTAIFEXOptSerials( int Index )
{
	int SIndex;
	if( Index != -1 )
	{
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TXO" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 0 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TX1" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 1 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TX2" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 1 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TX3" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 1 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TX4" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 1 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TX5" ))!= -1 )
			FOptChineseSerials[Index]->Move( SIndex, 1 );
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TEO" ))!= -1 )
			FOptChineseSerials[Index]->Move(SIndex,3);
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"TFO" ))!= -1 )
			FOptChineseSerials[Index]->Move(SIndex,4);
		if( (SIndex = FOptChineseSerials[Index]->IndexOf( L"XIO" ))!= -1 )
			FOptChineseSerials[Index]->Move(SIndex,5);
	}
}
//---------------------------------------------------------------------------
int __fastcall TContractInfoForm::InitFuturesExchangeTab( const UFC::AnsiString& Exchange )
{
	int FutExIndex = -1;
	if( gMarketDataStore->ExchangeSupportFutures( Exchange ) == true )
	{
		String ExchangeUnicode( Exchange.c_str() );
		///< add Exchange code
		FFutExchangeCode->Add( ExchangeUnicode );
		///< Add Exchange Tab.
		MarketTabSet->Tabs->Add( gMarketDataStore->GetExchangeDisplayName( ExchangeUnicode ) );
		///< Get the exchange index.
		FutExIndex = MarketTabSet->Tabs->Count - 1;

		for( int i = 0; i < TYPE_MAX; i++ )
		{
			///< Clear Serials, Chinese Serial, Maturity Date list.
			if( FFutSerials[ FutExIndex ][i] != NULL )
				delete FFutSerials[ FutExIndex ][i];
			FFutSerials[ FutExIndex ][i] = new TStringList();
			if( FChineseSerials[ FutExIndex ][i] != NULL )
				delete FChineseSerials[ FutExIndex ][i];
			FChineseSerials[ FutExIndex ][i] = new TStringList();
			if( FMaturityDate[ FutExIndex ][i] != NULL )
				delete FMaturityDate[ FutExIndex ][i];
			FMaturityDate[ FutExIndex ][i] = new TStringList();
		}
	}
	return FutExIndex;
}
//---------------------------------------------------------------------------
int __fastcall TContractInfoForm::InitOptionsExchangeTab( const UFC::AnsiString& Exchange )
{
	int OptExIndex = -1;

	if( gMarketDataStore->ExchangeSupportOptions( Exchange ) == true )
	{
		String ExchangeUnicode( Exchange.c_str() );
		///< add Exchange code
		FOptExchangeCode->Add( ExchangeUnicode );
		///< Add Exchange Tab.
		OptMarketTabSet->Tabs->Add( gMarketDataStore->GetExchangeDisplayName( ExchangeUnicode ) );
		///< Get the exchange index.
		OptExIndex = OptMarketTabSet->Tabs->Count - 1;
		///< Clear Serials, Chinese Serial list.
		if( FOptionsSerialList[ OptExIndex ].ItemCount( ) > 0 )
		{
			for( register int i = 0; i < FOptionsSerialList[OptExIndex].ItemCount();i++ )
				 delete FOptionsSerialList[OptExIndex].GetItem(i);
			FOptionsSerialList[OptExIndex].Clear();
		}
		if( FOptChineseSerials[ OptExIndex ] != NULL )
			delete FOptChineseSerials[ OptExIndex ];
		FOptChineseSerials[ OptExIndex ] = new TStringList();
	}
	return OptExIndex;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddExchangeSymbols( const UFC::AnsiString& Exchange )
{
	UFC::PHashedList<UFC::AnsiString,BasicInformation*>* ExchangeSymbols;
	int FutExIndex = InitFuturesExchangeTab( Exchange );
	int OptExIndex = InitOptionsExchangeTab( Exchange );
	int FutTypeIndex = TypeComboBox->ItemIndex;

	if( FutExIndex == -1 && OptExIndex == -1 )
	{
		String Ex( Exchange.c_str() );
		String ErrMsg;

		ErrMsg.printf( L"交易所[%s]沒有期貨或選擇權商品.", Ex );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
		return;
    }
	if( MarketTabSet->Tabs->Count <= 1 )
		MarketTabSet->Visible = false;
	else
		MarketTabSet->Visible = true;
	if( OptMarketTabSet->Tabs->Count <= 1 )
		OptMarketTabSet->Visible = false;
	else
		OptMarketTabSet->Visible = true;
	if((ExchangeSymbols = gMarketDataStore->GetSymbolsByExchange( Exchange )) != NULL )
	{
		BasicInformation* Info = ExchangeSymbols->First();
		int               CCount = 1;

		while( Info != NULL )
		{
			if( Info->GetBearPrice( ) != 0.0 && ///< Bear price exists.
				Info->GetBullPrice( ) != 0.0 && ///< Bull price exists.
				Info->GetTradeFlag() == true && ///< Tradable
				Info->IsMultileg() == false )   ///< Not Multi-leg contract
			{
				if( Info->GetMarket() == nsOrderMessageDefine::mTWOptions ||///< XXF10000MY, TAIFEX Options symbol
					Info->GetMarket() == nsOrderMessageDefine::mForeignFutures ||
					Info->GetMarket() == nsOrderMessageDefine::mCNOptions )
					AddOptionsSymbol( OptExIndex, Info, Info->GetSymbol() );
				else
					AddFuturesSymbol( FutExIndex, Info );
				CCount++;
			}
			///< Next contract.
			Info = ExchangeSymbols->Next();
			if( CCount % MSG_PUMP_TIMES == 0 )
				Application->ProcessMessages();
		}
	}
	if( Exchange == "TAIFEX" || Exchange == "TFX" ) ///< TAIFEX, Sort the Futures serial list.
	{
		if( FSupportFutures == true )
			SortTAIFEXFutSerials( FutExIndex );
		if( FSupportOptions == true )
			SortTAIFEXOptSerials( OptExIndex );
	}
	///< Remove exchange with no tradable symbol.
	if( FutExIndex != -1 && FFutSymSet[FutTypeIndex].ItemCount( ) == 0 )
	{
		FFutExchangeCode->Delete( FutExIndex );
		MarketTabSet->Tabs->Delete( FutExIndex );
	}
	else
	{
		for( int i = 0; i < TYPE_MAX; i++ )
		{
			FFutRowCount[ FutExIndex ][ i ] = FFutSymSet[i].ItemCount( )+1;
			FFutColCount[ FutExIndex ][ i ] = FFutMaturitySet[i].ItemCount( )+1;
			/// <- Paul Debug.
			if( FMaturityDate[ FutExIndex ][ i ] != NULL )
				FMaturityDate[ FutExIndex ][ i ]->Sort();
		}
	}
}
//---------------------------------------------------------------------------
//
//	Add Stock Symbols
//
//---------------------------------------------------------------------------
TSymbolItem* __fastcall TContractInfoForm::AddContractNode( TTreeNode* ParentNode, BasicInformation* Info )
{
	TStringList*    SymList = (TStringList*)ParentNode->Data;
	TSymbolItem*    SymbolItem;

	if( SymList == NULL ) ///< Symbol list not exists
	{
		SymList = new TStringList();
		ParentNode->Data = SymList;
	}
	SymbolItem = new TSymbolItem( ParentNode, Info );
	SymList->AddObject( SymbolItem->DispName, SymbolItem );
	return  SymbolItem;
}
//---------------------------------------------------------------------------
TTreeNode* __fastcall TContractInfoForm::AddNode( TTreeNode* ParentNode, const String& Name, int ImgIndex )
{
	TTreeNode* ProdNode     = ContractTreeView->Items->AddChild( ParentNode, Name );
	ProdNode->ImageIndex    = ImgIndex;
	ProdNode->SelectedIndex = ImgIndex;
	return ProdNode;
}
//---------------------------------------------------------------------------
TTreeNode* __fastcall TContractInfoForm::GetProdNode( UFC::PHashedList<UFC::AnsiString, TTreeNode*>& TickerSymbolTable, TTreeNode *StockNode, BasicInformation* Info )
{
	UFC::AnsiString ProductID ( Info->GetProductID() );
	TTreeNode*      ProdNode;

	if( ProductID.Length() > 0 && ProductID != "00" && ProductID != "A1" && ProductID != "A2")
	{
		UFC::AnsiString ProductKey = ProductID + Info->GetExchange(); ///< 01TWSE
		UFC::AnsiString UTF8ChineseName( Info->GetChineseName() ) ;
		String          ChineseSector;

		if( (ProdNode = TickerSymbolTable.GetObjectByKey( ProductKey )) == NULL )
		{   ///< ProdID Node not exists
			if( IsUTF8String( Info->GetProductID().c_str() ) == true )
				ChineseSector = UTF8ToString( Info->GetProductID().c_str() );
			else
				ChineseSector = String( Info->GetProductID().c_str());
			ProdNode = AddNode( StockNode, ChineseSector, 1 );
			TickerSymbolTable.Add( ProductKey, ProdNode );
		}
		return ProdNode;
	}
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddStockNode( String Exchange )
{
	ContractTreeView->Items->BeginUpdate();
	UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExchangeSymbols;
	UFC::PHashedList<UFC::AnsiString, TTreeNode*>         TickerSymbolTable;
	TTreeNode* ExchangeNode = AddNode( NULL, Exchange, 0 );
	TTreeNode* StockNode    = AddNode( ExchangeNode, STR_STOCK, 1 );
	TTreeNode* CWarrantNode = AddNode( ExchangeNode, STR_WARRANT, 1 );
	TTreeNode* PWarrantNode = AddNode( ExchangeNode, STR_PUT_WARRANT, 1 );
	TTreeNode* ProdNode;
	AnsiString AnsiExchange( Exchange );

	ExchangeNode->Text = gMarketDataStore->GetExchangeDisplayName( Exchange );
	if((ExchangeSymbols = gMarketDataStore->GetSymbolsByExchange( AnsiExchange.c_str() )) != NULL )
	{
		BasicInformation* Info = ExchangeSymbols->First();
		TSymbolItem*      SymbolInfo;
		int               CCount = 1;

		while( Info != NULL )
		{
			if( Info->IsWarrant( ) == true ) ///< Warrant
			{
				if( Info->GetCallPut() == cpCall )
					SymbolInfo = AddContractNode( CWarrantNode, Info );
				else
					SymbolInfo = AddContractNode( PWarrantNode, Info );
				FSearchKeys->AddObject( SymbolInfo->DispName, SymbolInfo );
			}
			else if( Info->GetTradeFlag() == true ) ///< Stock
			{
				if( (ProdNode = GetProdNode( TickerSymbolTable, StockNode, Info )) != NULL )
				{
					SymbolInfo = AddContractNode( ProdNode, Info );
					FSearchKeys->AddObject( SymbolInfo->DispName, SymbolInfo );
				}
				//FSupportStock = true;
			}
			Info = ExchangeSymbols->Next();
			CCount++;
			if( CCount%MSG_PUMP_TIMES == 0 )
				Application->ProcessMessages();
		}
	}
	StockNode->AlphaSort( true );
	ExchangeNode->Expanded = true;
	ContractTreeView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
//
//  main enrty
//  call this function to build Futures/Options contracts grid.
//
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddSymbol( TLabel* StatusLabel )
{
	if( FInit == true )
		return;
	int ExCount = gMarketDataStore->ExchangeCount();

	//ProgressForm->Caption = Pmfnextstrings_NEXT_PROGRESS_PROCESS_CONTRACTS;
	FSearchKeys->Clear();
	///< Clear Futures symbols.
	MarketTabSet->Tabs->Clear();
	FFutExchangeCode->Clear();
	///< Clear Options symbols.
	SerialComboBox->Clear();
	OptMarketTabSet->Tabs->Clear();
	FOptExchangeCode->Clear();
	///< Add new symbols. for each exchange....
	for( register int i = 0; i < ExCount; i++ )
	{
		UFC::AnsiString Exchange( gMarketDataStore->GetExchange( i ) );
		String          ExchangeUnicode( Exchange.c_str() );

		//ProgressForm->MsgLabel->Caption =  gMarketDataStore->GetExchangeDisplayName( ExchangeUnicode );
		if( g_Config.SupportExchange( ExchangeUnicode ) == true )
		{
			try
			{
				if( StatusLabel != NULL )
				{
					String Msg;
					Msg.printf( L"整理交易所[%s]商品", ExchangeUnicode );
					StatusLabel->Caption = Msg;
					Application->ProcessMessages();
				}
				///< TAIWAN Stock
				if( Exchange == "TWSE" || Exchange == "OTC" )
					AddStockNode( ExchangeUnicode  );
				else ///< Options or Futures.
					AddExchangeSymbols( Exchange );
			}
			catch( UFC::Exception& ex )
			{
				TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ex.what() );
			}
		}
	}
	///< Init Futures page
	if( MarketTabSet->Tabs->Count >= 1 )
	{
		MarketTabSet->TabIndex = 0;
		FutDrawGrid->RowCount = FFutRowCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
		FutDrawGrid->ColCount = FFutColCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
		AdjuestFutSize( MarketTabSet->TabIndex );
	}
	///< Init Options page
	if( OptMarketTabSet->Tabs->Count >= 1 )
	{
		OptMarketTabSet->TabIndex = 0;
		SerialComboBox->Clear();
		for( int i=0;i< FOptChineseSerials[ 0 ]->Count; i ++ )
		{
			String OptCht = gMarketDataStore->GetChinesePrefix( FOptExchangeCode->Strings[0], FOptChineseSerials[ 0 ]->Strings[i] );
			SerialComboBox->Items->Add( OptCht );
		}
		SerialComboBox->ItemIndex = 0;
		SerialComboBoxChange( NULL );
	}
	//ProgressForm->Hide();
	int BtnLeft = 2;
	int FirstIndex = -1;

	FutTabButton->Visible = FSupportFutures;
	if( FSupportFutures == true )
	{
		FutTabButton->Left = BtnLeft;
		BtnLeft += FutTabButton->Width + 1;
		if( FirstIndex == -1 )
			FirstIndex = 0;
	}
	OptTabButton->Visible = FSupportOptions;
	if( FSupportOptions == true )
	{
		OptTabButton->Left = BtnLeft;
		BtnLeft += OptTabButton->Width + 1;
		if( FirstIndex == -1 )
			FirstIndex = 1;
	}
	StockTabButton->Visible = FSupportStock;
	if( FSupportStock == true )
	{
		StockTabButton->Left = BtnLeft;
		BtnLeft += StockTabButton->Width + 1;
		if( FirstIndex == -1 )
			FirstIndex = 2;
	}
	if( FirstIndex != -1 )
		SwitchPage( FirstIndex );
	FInit = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AdjuestFutSize( int Index )
{
	int ColWidth,MaxColWidth = 0;
	int HeightLimit = Screen->Height * 85 /100;

	FutDrawGrid->Canvas->Font->Name = FONT_NAME;
	FutDrawGrid->Canvas->Font->Size = FONT_SIZE;
	FutDrawGrid->DefaultColWidth    = FutDrawGrid->Canvas->TextWidth( L"_299912__");
	FutDrawGrid->DefaultRowHeight   = 30; //FutDrawGrid->Canvas->TextHeight( L"123456789") + 2;
	for( int i = 0; i < FChineseSerials[ Index ][ TypeComboBox->ItemIndex ]->Count; i ++ )
	{
		ColWidth = 	FutDrawGrid->Canvas->TextWidth( FChineseSerials[ Index ][ TypeComboBox->ItemIndex ]->Strings[ i ] )+ 2;
		if( ColWidth > MaxColWidth )
			MaxColWidth = ColWidth;
	}
	FutDrawGrid->ColWidths[0]       = MaxColWidth + 10;
	FFutPageWidth  = 18 + MaxColWidth + GetSystemMetrics( SM_CXVSCROLL ) + FutDrawGrid->DefaultColWidth*(FutDrawGrid->ColCount-1);
	FFutPageHeight = 43 + FutDrawGrid->DefaultRowHeight * FutDrawGrid->RowCount + MarketTabSet->Height;
	if( FFutPageHeight > HeightLimit )
		FFutPageHeight = HeightLimit;
	if( FFutPageWidth < 512 )
		FFutPageWidth = 512;
	if( FPicker == true )
	{
		ClientHeight = FFutPageHeight;
		Constraints->MinHeight = 100;
	}
	Width                  = FFutPageWidth;
	Constraints->MaxWidth  = FFutPageWidth+10;
	Constraints->MinWidth  = FFutPageWidth;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AdjuestOptSize( int  MaturityCount )
{
	int HeightLimit = Screen->Height * 85 /100;

	OptDrawGrid->Canvas->Font->Name = FONT_NAME;
	OptDrawGrid->Canvas->Font->Size = FONT_SIZE;
	OptDrawGrid->DefaultRowHeight   = 30; //OptDrawGrid->Canvas->TextHeight( L"123456789") + 2;
	FOptPageWidth  = 18 + GetSystemMetrics( SM_CXVSCROLL );
	FOptPageHeight = 43 + OptDrawGrid->DefaultRowHeight * OptDrawGrid->RowCount + OptDrawGrid->RowHeights[0];
	if( FOptPageHeight > HeightLimit )
		FOptPageHeight = HeightLimit;
	for( int i = 0; i < OptDrawGrid->ColCount; i ++ )
	{
		if( i == MaturityCount )
			OptDrawGrid->ColWidths[ i ] = OptDrawGrid->Canvas->TextWidth( L"台指選_") + GetSystemMetrics( SM_CXVSCROLL );
		else
			OptDrawGrid->ColWidths[ i ] = OptDrawGrid->Canvas->TextWidth( L"_99_");
		FOptPageWidth += OptDrawGrid->ColWidths[ i ];
	}
	if( FOptPageWidth < 220 )
		FOptPageWidth = 220;
	if( FPicker == true )
	{
		ClientHeight = FOptPageHeight;
		Constraints->MinHeight = 100;
	}
	Width                 = FOptPageWidth;
	Constraints->MaxWidth = FOptPageWidth+10;
	Constraints->MinWidth = FOptPageWidth;

}
//---------------------------------------------------------------------------
bool __fastcall TContractInfoForm::IsFutSelectedCell( int Col, int Row, TGridDrawState& State )
{
	if( FFutSelectSet.Exists( Col*1000 + Row ) == TRUE  )
		return true;
	if( State.Contains( gdSelected ) == TRUE  )
	{
		if( FutDrawGrid->Selection.Right  - FutDrawGrid->Selection.Left > 0 ||
			FutDrawGrid->Selection.Bottom - FutDrawGrid->Selection.Top  > 0 )
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol )
{
	TRIVERTEX        vert[ 2 ];
	GRADIENT_RECT    gRect;

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = GetRValue( (DWORD)UpCol)<<8;
	vert [0] .Green  = GetGValue( (DWORD)UpCol)<<8;
	vert [0] .Blue   = GetBValue( (DWORD)UpCol)<<8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = GetRValue( (DWORD)DownCol)<<8;
	vert [1] .Green  = GetGValue( (DWORD)DownCol)<<8;
	vert [1] .Blue   = GetBValue( (DWORD)DownCol)<<8;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FutDrawGridDrawCell(TObject *Sender, int ACol,
		  int ARow, TRect &Rect, TGridDrawState State)
{
	UFC::AnsiString FutSymbol,FutExchange;
	int Index = MarketTabSet->TabIndex;

	if( FChanging == true )
		return;
	if( ACol == 0 && ARow == 0 ) ///< Cell (0,0)
	{
		FutDrawGrid->Canvas->Pen->Color =  COL_GRID_LINE;
		FutDrawGrid->Canvas->MoveTo( Rect.right-1, Rect.bottom-1  );
		FutDrawGrid->Canvas->LineTo( Rect.right-1, Rect.top );
		return;
	}
	if( ACol == 0 && ARow > 0 ) ///< Draw Serials name
	{
		Vcl::Graphics::TTextFormat Formats;
		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
		String UnicodeText( FChineseSerials[Index][ TypeComboBox->ItemIndex ]->Strings[ ARow-1 ]  );
		TRect TextRect = Rect;
		TextRect.Left += 3;

		if( FFutSelIndex == ARow && FFutSelPage == TypeComboBox->ItemIndex )
		{
			TFontStyles FS;
			FS << fsUnderline;
			FutDrawGrid->Canvas->Font->Color = clNavy;
			FutDrawGrid->Canvas->Font->Style = FS;
		}
		FutDrawGrid->Canvas->TextRect( TextRect, UnicodeText, Formats );
		return;
	}
	if( ACol > 0 && ARow == 0 ) ///< Draw Maturity year month
	{
		String     UnicodeText( FMaturityDate[Index][ TypeComboBox->ItemIndex ]->Strings[ ACol-1 ]  );
		AnsiString Text = UnicodeText;

		DrawTextExA( FutDrawGrid->Canvas->Handle, (char*)Text.c_str(), Text.Length(),
			&Rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS, NULL );
		return;
    }
	ToFuturesSymbol(  ACol, ARow, FutExchange, FutSymbol );
	int offx = Rect.Left + (Rect.Width() - CheckImageList->Width )/2;
	int offy = Rect.Top + (Rect.Height() - CheckImageList->Height )/2;

	if( FutSymbol.Length() != 0 )
	{
		if( FPicker == false && IsFutSelectedCell( ACol, ARow, State ) == TRUE  ) ///< Selected cells
		{
			if( ContractViewerForm->IsDepthViewOpen( FutExchange.c_str(), FutSymbol.c_str( ) ) == true )
				FutDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SHOWED;
			else
				FutDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SELECT;
			FutDrawGrid->Canvas->FillRect( Rect );
			CheckImageList->Draw( FutDrawGrid->Canvas, offx, offy, 1 );
		}
		else
		{
			if( FPicker == false && ContractViewerForm->IsDepthViewOpen( FutExchange.c_str(), FutSymbol.c_str( ) ) == true )
			{
				FutDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SHOWED;//(TColor)RGB(215,228,242);
				FutDrawGrid->Canvas->FillRect( Rect );
				CheckImageList->Draw( FutDrawGrid->Canvas, offx, offy, 0 );
			}
			else
			{

				FutDrawGrid->Canvas->Brush->Color = COL_CONTRACT_NORMAL;
				FutDrawGrid->Canvas->FillRect( Rect );
				CheckImageList->Draw( FutDrawGrid->Canvas, offx, offy, 0 );
			}
		}
	}
	else ///< No Symbol exists.
	{
		FutDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_NONE;//(TColor)RGB(215,228,242);
		FutDrawGrid->Canvas->FillRect( Rect );
		CheckImageList->Draw( FutDrawGrid->Canvas, offx, offy, 2 );
	}
	FutDrawGrid->Canvas->Pen->Color =  COL_GRID_LINE;
	FutDrawGrid->Canvas->MoveTo( Rect.left, Rect.bottom-1  );
	FutDrawGrid->Canvas->LineTo( Rect.right-1, Rect.bottom-1  );
	FutDrawGrid->Canvas->LineTo( Rect.right-1, Rect.top );
}
//---------------------------------------------------------------------------
bool __fastcall TContractInfoForm::IsOptSelectedCell( int Col, int Row, TGridDrawState& State )
{
	if( FOptSelectSet.Exists( Col*1000 + Row ) == TRUE  )
		return true;
	if( State.Contains( gdSelected ) == TRUE  )
    {
        if( OptDrawGrid->Selection.Right  - OptDrawGrid->Selection.Left > 0 ||
 	        OptDrawGrid->Selection.Bottom - OptDrawGrid->Selection.Top  > 0 )
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptDrawGridDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State)
{
	int Index = OptMarketTabSet->TabIndex;

	if( FChanging == true )
		return;
	if( SerialComboBox->ItemIndex != -1 )
	{
		String          UnicodeSerial = FOptChineseSerials[Index]->Strings[ SerialComboBox->ItemIndex ];
		AnsiString      Serial = UnicodeSerial;
		UFC::AnsiString OptSymbol,OptExchange;
		TOptionsSerial* OptionsSerial;


		if( (OptionsSerial = FOptionsSerialList[Index].GetObjectByKey(Serial.c_str())) != NULL )
		{
			int MaturityCount = OptionsSerial->Maturity->Count;
			if( ACol == MaturityCount && ARow == 0 ) ///< Strike Price cell
			{
				AnsiString Text("履約價");
				DrawTextExA( OptDrawGrid->Canvas->Handle, (char*)Text.c_str(), Text.Length(),
					&Rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS, NULL );
				return;
			}
			if( ARow == 0 ) ///< Header row
			{
				AnsiString DspText;
				if( ACol < MaturityCount )
					DspText = OptionsSerial->Maturity->Strings[ MaturityCount -1 - ACol ];
				else if( ACol > MaturityCount )
					DspText = OptionsSerial->Maturity->Strings[ ACol - 1 - MaturityCount ];
				UFC::AnsiString AnsiText( DspText.c_str() );
				UFC::AnsiString Text = AnsiText.SubString( 4, 2 );

				DrawTextExA( OptDrawGrid->Canvas->Handle, (char*)Text.c_str(), Text.Length(),
					&Rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS, NULL );
				OptDrawGrid->Canvas->Pen->Color =  COL_GRID_LINE;
				OptDrawGrid->Canvas->MoveTo( Rect.right-1, Rect.bottom-1  );
				OptDrawGrid->Canvas->LineTo( Rect.right-1, Rect.top );
				return;
			}
			if( ACol == MaturityCount && ARow > 0 ) ///< Strike price col
			{
				AnsiString Text = OptionsSerial->StrikePxs->Strings[ ARow-1 ] ;

				OptDrawGrid->Canvas->Brush->Color = COL_GRID_LINE;
				OptDrawGrid->Canvas->FillRect( Rect );
				OptDrawGrid->Canvas->Brush->Style = bsClear;
				OptDrawGrid->Canvas->Font->Color = clBlack;
				DrawTextExA( OptDrawGrid->Canvas->Handle, (char*)Text.c_str(), Text.Length(),
					&Rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS, NULL );
				return;
			}
			int offx = Rect.Left + (Rect.Width() - CheckImageList->Width )/2;
			int offy = Rect.Top + (Rect.Height() - CheckImageList->Height )/2;

			ToOptionsSymbol( ACol, ARow, OptExchange, OptSymbol );
			if( OptSymbol.Length() != 0 )
			{
				if( FPicker == false && IsOptSelectedCell( ACol, ARow, State ) == TRUE )
				{
					if( ContractViewerForm->IsDepthViewOpen( OptExchange.c_str(), OptSymbol.c_str( ) ) == true )
						OptDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SHOWED;
					else
						OptDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SELECT;
					OptDrawGrid->Canvas->FillRect( Rect );
					CheckImageList->Draw( OptDrawGrid->Canvas, offx, offy, 1 );
				}
				else
				{
					if( FPicker == false && ContractViewerForm->IsDepthViewOpen( OptExchange.c_str(), OptSymbol.c_str( ) ) == true )
					{
						OptDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_SHOWED;//(TColor)RGB(215,228,242);
						OptDrawGrid->Canvas->FillRect( Rect );
						CheckImageList->Draw( OptDrawGrid->Canvas, offx, offy, 0 );
					}
					else
					{
						OptDrawGrid->Canvas->Brush->Color = COL_CONTRACT_NORMAL;
						OptDrawGrid->Canvas->FillRect( Rect );
						CheckImageList->Draw( OptDrawGrid->Canvas, offx, offy, 0 );
					}
				}
			}
			else
			{
				OptDrawGrid->Canvas->Brush->Color =  COL_CONTRACT_NONE;//(TColor)RGB(215,228,242);
				OptDrawGrid->Canvas->FillRect( Rect );
				CheckImageList->Draw( OptDrawGrid->Canvas, offx, offy, 2 );
			}
			OptDrawGrid->Canvas->Pen->Color =  COL_GRID_LINE;
			OptDrawGrid->Canvas->MoveTo( Rect.left, Rect.bottom-1  );
			OptDrawGrid->Canvas->LineTo( Rect.right-1, Rect.bottom-1  );
			OptDrawGrid->Canvas->LineTo( Rect.right-1, Rect.top );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SerialComboBoxChange(TObject *Sender)
{
	int Index = OptMarketTabSet->TabIndex;

	if( SerialComboBox->ItemIndex != -1 )
	{
		AnsiString      Serial = FOptChineseSerials[Index]->Strings[ SerialComboBox->ItemIndex ];
		TOptionsSerial* OptionsSerial;

		if( (OptionsSerial = FOptionsSerialList[Index].GetObjectByKey(Serial.c_str())) != NULL )
		{
			int MaturityCount = OptionsSerial->Maturity->Count;

			OptionsSerial->StrikePxs->Sort();
			OptionsSerial->Maturity->Sort();
			OptDrawGrid->RowCount = OptionsSerial->StrikePxs->Count + 1;
			OptDrawGrid->ColCount = MaturityCount *2 + 1;

			AdjuestOptSize( MaturityCount );
			//TRect SerialBoxRect = OptDrawGrid->CellRect( MaturityCount, 0 );
			//SerialBoxRect.Top = OptDrawGrid->Top;
			//SerialComboBox->BoundsRect = SerialBoxRect;
			OptDrawGrid->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FormShow(TObject *Sender)
{
	AddSymbol( NULL ); ///< Try to init all contracts.

	FutDrawGrid->Canvas->Font->Name = FONT_NAME;
	FutDrawGrid->Canvas->Font->Size = FONT_SIZE;
	OptDrawGrid->Canvas->Font->Name = FONT_NAME;
	OptDrawGrid->Canvas->Font->Size = FONT_SIZE;

	if( PageControl->ActivePage == FutTabSheet /*&& FutTabSheet->TabVisible == true*/  )
	{
		Width                 = FFutPageWidth;
		if( FPicker == true )
			ClientHeight          = FFutPageHeight;
		Constraints->MaxWidth = FFutPageWidth+10;
		Constraints->MinWidth = FFutPageWidth;
	}
	else if( PageControl->ActivePage == OptTabSheet /*&& OptTabSheet->TabVisible == true*/ )
	{
		Width                 = FOptPageWidth;
		if( FPicker == true )
			ClientHeight          = FOptPageHeight;
		Constraints->MaxWidth = FOptPageWidth+10;
		Constraints->MinWidth = FOptPageWidth;
	}
	else if( PageControl->ActivePage == StockTabSheet /*&& StockTabSheet->TabVisible == true*/ )
	{
		Width                 = 615;
		if( FPicker == true )
			ClientHeight          = 595;
		Constraints->MaxWidth = 615;
		Constraints->MinWidth = 615;
	}

}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ToOptionsSymbol( int Col, int Row, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol )
{
	if( SerialComboBox->ItemIndex != -1 )
	{
		int             Index = OptMarketTabSet->TabIndex;
		AnsiString      Exchg( FOptExchangeCode->Strings[ Index ] );
		String          UnicodeSerial = FOptChineseSerials[ Index ]->Strings[ SerialComboBox->ItemIndex ];
		String          CpyUnicodeSerial( UnicodeSerial );
		AnsiString      Serial = UnicodeSerial;
		TOptionsSerial* OptionsSerial;

		if( (OptionsSerial = FOptionsSerialList[ Index ].GetObjectByKey(Serial.c_str())) != NULL )
		{
			int MaturityCount = OptionsSerial->Maturity->Count;
			if( Col != MaturityCount && Row > 0 )///< Skip fix cells
			{
				UFC::AnsiString  SymbolKey;
				AnsiString       Maturity;
				AnsiString       StkPx = OptionsSerial->StrikePxs->Strings[ Row-1 ];
				char             CallPut;
				UFC::AnsiString* ExSym;

				if( Col < MaturityCount )
				{
					Maturity = OptionsSerial->Maturity->Strings[ MaturityCount -1 - Col ];
					CallPut = 'C';
				}
				else if( Col > MaturityCount )
				{
					Maturity = OptionsSerial->Maturity->Strings[ Col - 1 - MaturityCount ];
					CallPut = 'P';
				}
				SymbolKey.Printf( "%s%s%c%s",Serial.c_str(),StkPx.c_str(),CallPut, Maturity.c_str() );
				if( ( ExSym = FSymbolList.GetObjectByKey( SymbolKey )) != NULL )
				{
					Exchange = Exchg.c_str();
					Symbol   = ExSym->c_str();
					return;
				}
			}
		}
	}
	Exchange = "";
	Symbol   = "";
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ToFuturesSymbol( int Col, int Row, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol )
{
	if( Col > 0 && Row > 0 && Col < FutDrawGrid->ColCount && Row < FutDrawGrid->RowCount )
	{
		AnsiString Exchg( FFutExchangeCode->Strings[ MarketTabSet->TabIndex ] );
		AnsiString Serial( FFutSerials[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ]->Strings[ Row - 1 ]  );
		AnsiString Maturity( FMaturityDate[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ]->Strings[ Col - 1 ]  );
		AnsiString Key = Serial + Maturity;

		UFC::AnsiString* ExSym = FSymbolList.GetObjectByKey( Key.c_str( ));
		if( ExSym != NULL )
		{
			Exchange = Exchg.c_str();
			Symbol   = ExSym->c_str();
			return;
		}
	}
	Exchange = "";
	Symbol   = "";
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OpenFuturesDepth( int Col, int Row, bool Open, bool NewIfExist )
{
	UFC::AnsiString FutSymbol,FutExchange;

	ToFuturesSymbol(  Col, Row, FutExchange, FutSymbol );
	if( FutSymbol.Length() != 0 )
	{
		if( Open == true )
			ContractViewerForm->OpenOrderBookForm( FutExchange.c_str(), FutSymbol.c_str( ),NewIfExist );
		else
			ContractViewerForm->CloseOrderBookForm( FutExchange.c_str(), FutSymbol.c_str( ) );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OpenOptionsDepth( int Col, int Row, bool Open, bool NewIfExist )
{
	UFC::AnsiString OptSymbol,OptExchange;

	ToOptionsSymbol(  Col, Row, OptExchange, OptSymbol );
	if( OptSymbol.Length() != 0 )
	{
		if( Open == true )
			ContractViewerForm->OpenOrderBookForm( OptExchange.c_str(), OptSymbol.c_str( ),NewIfExist );
		else
			ContractViewerForm->CloseOrderBookForm( OptExchange.c_str(), OptSymbol.c_str( ));
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FutDrawGridDblClick(TObject *Sender)
{
	int             Col,Row;
	TPoint          CurPos = Mouse->CursorPos;
	UFC::AnsiString FutSymbol,FutExchange;

	CurPos = FutDrawGrid->ScreenToClient( CurPos );
	FutDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
	ToFuturesSymbol(  Col, Row, FutExchange, FutSymbol );
	if( FPicker == true )
	{
		FPickExchange = FutExchange.c_str();
		FPickSymbol   = FutSymbol.c_str();
		if( FPickSymbol.Length( ) > 0 && FPickExchange.Length( ) > 0 )
			ModalResult   = mrOk;
	}
	else
	{
		if( FutSymbol.Length() != 0 ) ///< Open default account (index = 0)
			ContractViewerForm->OpenOrderBookForm( FutExchange.c_str(), FutSymbol.c_str( )  );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptDrawGridDblClick(TObject *Sender)
{
	int    Col,Row;
	TPoint CurPos = Mouse->CursorPos;
	UFC::AnsiString OptSymbol,OptExchange;

	CurPos = OptDrawGrid->ScreenToClient( CurPos );
	OptDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
	ToOptionsSymbol(  Col, Row, OptExchange, OptSymbol );
	if( FPicker == true )
	{
		FPickExchange = OptExchange.c_str();
		FPickSymbol   = OptSymbol.c_str();
		if( FPickSymbol.Length( ) > 0 && FPickExchange.Length( ) > 0 )
			ModalResult   = mrOk;
	}
	else
	{
		if( OptSymbol.Length() != 0 )
			ContractViewerForm->OpenOrderBookForm( OptExchange.c_str(), OptSymbol.c_str( ), true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ShowDepthMenuItemClick(TObject *Sender)
{
	MainForm->SettingPanel( false );
	if( PageControl->ActivePage == FutTabSheet )
	{
		for( int Row = FutDrawGrid->Selection.Top; Row <= FutDrawGrid->Selection.Bottom;Row ++ )
		{
			for( int Col = FutDrawGrid->Selection.Left; Col <= FutDrawGrid->Selection.Right;Col ++ )
			{
				 OpenFuturesDepth( Col, Row, true, true  );
				 Application->ProcessMessages();
			}
		}
		for( int i = 0;i < FFutSelectSet.ItemCount(); i++ )
		{
			int Key = FFutSelectSet.GetItem( i );
			OpenFuturesDepth( Key/1000, Key%1000, true, false );
			Application->ProcessMessages();
		}
	}
	else if( PageControl->ActivePage == OptTabSheet )
	{
		for( int Row = OptDrawGrid->Selection.Top; Row <= OptDrawGrid->Selection.Bottom;Row ++ )
		{
			for( int Col = OptDrawGrid->Selection.Left; Col <= OptDrawGrid->Selection.Right;Col ++ )
			{
				 OpenOptionsDepth( Col, Row, true, true  );
				 Application->ProcessMessages();
			}
		}
		for( int i = 0;i < FOptSelectSet.ItemCount(); i++ )
		{
			int Key = FOptSelectSet.GetItem( i );
			OpenOptionsDepth( Key/1000, Key%1000, true, false );
			Application->ProcessMessages();
		}
	}
	else if( PageControl->ActivePage == StockTabSheet )
	{
		ResultListBoxDblClick( ShowDepthMenuItem );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ChartMenuItemClick(TObject *Sender)
{
	UFC::AnsiString FutExchange,Symbol,OptExchange;

	if( PageControl->ActivePage == FutTabSheet )
	{
		ToFuturesSymbol(  FutDrawGrid->Selection.Right, FutDrawGrid->Selection.Bottom, FutExchange, Symbol );
		if( Symbol.Length() != 0 )
			ContractViewerForm->OpenKBarForm( FutExchange.c_str(), Symbol.c_str() );
	}
	else if( PageControl->ActivePage == OptTabSheet )
	{
		ToOptionsSymbol(  OptDrawGrid->Selection.Right, OptDrawGrid->Selection.Bottom, OptExchange, Symbol );
		if( Symbol.Length() != 0 )
			ContractViewerForm->OpenKBarForm( OptExchange.c_str(), Symbol.c_str() );
	}
	else if( PageControl->ActivePage == StockTabSheet )
	{
		if( ResultListBox->ItemIndex != -1 )
		{
			TSymbolItem* Item = (TSymbolItem*)ResultListBox->Items->Objects[ResultListBox->ItemIndex];
			if( Item != NULL )
			{
				BasicInformation* Info = Item->Info;
				if( Info != NULL )
					ContractViewerForm->OpenKBarForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str());
			}
		}
	}
	MainForm->SettingPanel( false );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::CloseDepthMenuItemClick(TObject *Sender)
{
	if( PageControl->ActivePage == FutTabSheet )
    {
       	for( int Row = FutDrawGrid->Selection.Top; Row <= FutDrawGrid->Selection.Bottom;Row ++ )
	    	for( int Col = FutDrawGrid->Selection.Left; Col <= FutDrawGrid->Selection.Right;Col ++ )
				 OpenFuturesDepth( Col, Row, false, false );
		for( int i = 0;i < FFutSelectSet.ItemCount(); i++ )
		{
			int Key = FFutSelectSet.GetItem( i );
			OpenFuturesDepth( Key/1000, Key%1000, false, false );
		}
	}
	else if( PageControl->ActivePage == OptTabSheet )
	{
		for( int Row = OptDrawGrid->Selection.Top; Row <= OptDrawGrid->Selection.Bottom;Row ++ )
			for( int Col = OptDrawGrid->Selection.Left; Col <= OptDrawGrid->Selection.Right;Col ++ )
				 OpenOptionsDepth( Col, Row, false, false  );
		for( int i = 0;i < FOptSelectSet.ItemCount(); i++ )
		{
			int Key = FOptSelectSet.GetItem( i );
			OpenOptionsDepth( Key/1000, Key%1000, false, false );
		}
	}
	else if( PageControl->ActivePage == StockTabSheet )
	{
		if( ResultListBox->ItemIndex != -1 )
		{
			TSymbolItem* Item = (TSymbolItem*)ResultListBox->Items->Objects[ResultListBox->ItemIndex];
			if( Item != NULL )
			{
				BasicInformation* Info = Item->Info;
				if( Info != NULL )
					ContractViewerForm->CloseOrderBookForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str());
			}
		}
	}
	MainForm->SettingPanel( false );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::AddToMDListMenuItemClick(TObject *Sender)
{
	TMenuItem* MenuItem = dynamic_cast<TMenuItem*>(Sender);

    if( MenuItem != NULL )
    {
		UFC::AnsiString FutExchange,Symbol,OptExchange;

		if( PageControl->ActivePage == FutTabSheet )
	    {
    	   	for( int Row = FutDrawGrid->Selection.Top; Row <= FutDrawGrid->Selection.Bottom;Row ++ )
        	{
	    		for( int Col = FutDrawGrid->Selection.Left; Col <= FutDrawGrid->Selection.Right;Col ++ )
            	{
					ToFuturesSymbol(  Col, Row, FutExchange, Symbol );
					if( Symbol.Length() != 0 )
						ContractViewerForm->AddCustomSymbol( MenuItem->Tag, FutExchange.c_str(), Symbol.c_str());
				}
			}
			for( int i = 0;i < FFutSelectSet.ItemCount(); i++ )
			{
				int Key = FFutSelectSet.GetItem( i );

				ToFuturesSymbol( Key/1000, Key%1000, FutExchange, Symbol );
				if( Symbol.Length() != 0 )
					ContractViewerForm->AddCustomSymbol( MenuItem->Tag, FutExchange.c_str(), Symbol.c_str());
			}
	    }
    	else if( PageControl->ActivePage == OptTabSheet )
	    {
    	   	for( int Row = OptDrawGrid->Selection.Top; Row <= OptDrawGrid->Selection.Bottom;Row ++ )
            {
	    		for( int Col = OptDrawGrid->Selection.Left; Col <= OptDrawGrid->Selection.Right;Col ++ )
                {
					ToOptionsSymbol(  Col, Row, OptExchange,Symbol );
					if( Symbol.Length() != 0 )
						ContractViewerForm->AddCustomSymbol( MenuItem->Tag, OptExchange.c_str(), Symbol.c_str());
				}
			}
			for( int i = 0;i < FOptSelectSet.ItemCount(); i++ )
			{
				int Key = FOptSelectSet.GetItem( i );
				ToOptionsSymbol( Key/1000, Key%1000, OptExchange,Symbol );
				if( Symbol.Length() != 0 )
					ContractViewerForm->AddCustomSymbol( MenuItem->Tag, OptExchange.c_str(), Symbol.c_str());
			}
		}
		else if( PageControl->ActivePage == StockTabSheet )
		{
			if( ResultListBox->ItemIndex != -1 )
			{
				TSymbolItem* Item = (TSymbolItem*)ResultListBox->Items->Objects[ResultListBox->ItemIndex];
				if( Item != NULL )
				{
					BasicInformation* Info = Item->Info;
					if( Info != NULL )
						ContractViewerForm->AddCustomSymbol( MenuItem->Tag, Info->GetExchange().c_str(), Info->GetSymbol().c_str());
				}
			}
		}
		MainForm->SettingPanel( false );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FutDrawGridMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	int             Col,Row,Key;
	TPoint          CurPos = Mouse->CursorPos;

	if( Shift.Contains( ssLeft ) )
    {
	    CurPos = FutDrawGrid->ScreenToClient( CurPos );
 		FutDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
	    Key = Col*1000 + Row;
       	if( !FFutSelectSet.Exists( Key ) )
        	FFutSelectSet.Add( Key );
   	    else
    	    FFutSelectSet.Delete( Key );
	    if( !Shift.Contains( ssCtrl  ) && FFutSelectSet.ItemCount() > 1 )
        {
		    FFutSelectSet.Clear();
        	FFutSelectSet.Add( Key );
        }
   	    FutDrawGrid->Invalidate();
	}
	else if( Shift.Contains( ssRight ) )
		PopupMenu->Tag = 0;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptDrawGridMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	int             Col,Row,Key;
	TPoint          CurPos = Mouse->CursorPos;

	if( Shift.Contains( ssLeft ) )
	{
		CurPos = OptDrawGrid->ScreenToClient( CurPos );
		OptDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
		Key = Col*1000 + Row;
		if( !FOptSelectSet.Exists( Key ) )
			FOptSelectSet.Add( Key );
		else
			FOptSelectSet.Delete( Key );
		if( !Shift.Contains( ssCtrl  ) && FOptSelectSet.ItemCount() > 1 )
		{
			FOptSelectSet.Clear();
			FOptSelectSet.Add( Key );
		}
		OptDrawGrid->Invalidate();
	}
	else if( Shift.Contains( ssRight ) )
		PopupMenu->Tag = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TContractInfoForm::SelectSymbol( String& Exchange, String& Symbol, int Page )
{
	TModalResult       Result;
	String             SelEx, SelSym;
	TBorderIcons      Icons;

	Icons.Clear();
	Icons << biSystemMenu;
	SelectContractForm->AddSymbol( NULL );
	SelectContractForm->FPickSymbol = "";
	SelectContractForm->Caption = L"請選擇商品";
	SelectContractForm->SwitchPage( Page );
	Result = SelectContractForm->ShowModal();
	SelEx  = SelectContractForm->FPickExchange;
	SelSym = SelectContractForm->FPickSymbol;
	if( Result == mrOk )
	{
		Exchange = SelEx;
		Symbol = SelSym;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TContractInfoForm::SelectSymbol( String OrigExchange, String OrigSymbol, String& Exchange, String& Symbol )
{
	BasicInformation* Info;
	TModalResult      Result;
	int               TabPage;
	TBorderIcons      Icons;

	Icons.Clear();
	Icons << biSystemMenu;
	SelectContractForm->AddSymbol( NULL  );
	SelectContractForm->FPickSymbol = "";
	SelectContractForm->Caption = L"請選擇商品";
	if( (Info = gMarketDataStore->GetBasicInformation( OrigExchange, OrigSymbol, false )) != NULL )
	{
		switch( (MarketEnum)Info->GetMarket() )
		{
			case nsOrderMessageDefine::mCNFutures:   ///< GLQH   China  Futures
			case nsOrderMessageDefine::mTWFutures:   ///< TAIFEX Taiwan Futures
			case nsOrderMessageDefine::mForeignFutures: ///< PATS          Futures
							  SelectContractForm->SwitchPage( 0 );
							  TabPage = SelectContractForm->MarketTabSet->Tabs->IndexOf( gMarketDataStore->GetExchangeDisplayName( OrigExchange ) );
							  if( TabPage != -1 )
								  SelectContractForm->MarketTabSet->TabIndex = TabPage;
							  else
								  SelectContractForm->MarketTabSet->TabIndex = 0;
							  SelectContractForm->ExchangeChangeTimerTimer( NULL );
							  break;
			case nsOrderMessageDefine::mCNOptions:
			case nsOrderMessageDefine::mForeignOptions: ///< PATS   Options
			case nsOrderMessageDefine::mTWOptions:   ///< TAIFEX Options
							  SelectContractForm->SwitchPage( 1 );
							  TabPage = SelectContractForm->OptMarketTabSet->Tabs->IndexOf( gMarketDataStore->GetExchangeDisplayName( OrigExchange ) );
							  if( TabPage != -1 )
								  SelectContractForm->OptMarketTabSet->TabIndex = TabPage;
							  else
								  SelectContractForm->OptMarketTabSet->TabIndex = 0;
							  SelectContractForm->OptExchangeChangeTimerTimer( NULL );
							  break;///< TAIFEX Taiwan Options
			case nsOrderMessageDefine::mTSE: ///< TSEC   Taiwan equity
			case nsOrderMessageDefine::mOTC: ///< OTC    Taiwan
							  SelectContractForm->SwitchPage( 2 );
							  SelectContractForm->SearchEdit->Text = OrigSymbol;
							  SelectContractForm->SearchGlyphButtonClick(NULL);
							  break;
			default:          SelectContractForm->SwitchPage( 2 );
							  break;
		}
	}
	Result   = SelectContractForm->ShowModal();
	Exchange = SelectContractForm->FPickExchange;
	Symbol   = SelectContractForm->FPickSymbol;
	if( Result == mrOk )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SaveProperties( const String& Profile )
{
	if(	PageControl->ActivePage == FutTabSheet )
		g_Config.SetDesktopInteger( Profile, "ContractViewerPage", 0 );
	else if(	PageControl->ActivePage == OptTabSheet )
		g_Config.SetDesktopInteger( Profile, "ContractViewerPage", 1 );
	else
		g_Config.SetDesktopInteger( Profile, "ContractViewerPage", 2 );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::LoadProperties( const String& Profile )
{
	int ShowPage = g_Config.GetDesktopInteger( Profile, "ContractViewerPage", 0 );
	SwitchPage( ShowPage );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FutDrawGridClick(TObject *Sender)
{
	if( FPicker == true )
	{
		int             Col,Row;
		TPoint          CurPos = Mouse->CursorPos;
		UFC::AnsiString FutExchange,FutSymbol;

		CurPos = FutDrawGrid->ScreenToClient( CurPos );
		FutDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
		ToFuturesSymbol( Col, Row, FutExchange, FutSymbol );
		FPickSymbol = FutSymbol.c_str();
		FPickExchange = FutExchange.c_str();
		if( FPickSymbol.Length( ) > 0 && FPickExchange.Length( ) > 0 )
			ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptDrawGridClick(TObject *Sender)
{
	if( FPicker == true )
	{
		int    Col,Row;
		TPoint CurPos = Mouse->CursorPos;
		UFC::AnsiString OptExchange,OptSymbol;

		CurPos = OptDrawGrid->ScreenToClient( CurPos );
		OptDrawGrid->MouseToCell( CurPos.x, CurPos.y, Col, Row );
		ToOptionsSymbol(  Col, Row, OptExchange, OptSymbol );
		FPickSymbol   = OptSymbol.c_str();
		FPickExchange = OptExchange.c_str();
		if( FPickSymbol.Length( ) > 0 && FPickExchange.Length( ) > 0 )
			ModalResult   = mrOk;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::MarketTabSetChange(TObject *Sender, int NewTab,
		  bool &AllowChange)
{
	if( NewTab < MarketTabSet->Tabs->Count && NewTab >=0 )
	{
		FChanging = true;
		ExchangeChangeTimer->Enabled = true;
		AllowChange = true;
	}
	else
		AllowChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ExchangeChangeTimerTimer(TObject *Sender)
{
	ExchangeChangeTimer->Enabled = false;
	FChanging = false;
	if( PageControl->ActivePage == FutTabSheet )
	{
		FutDrawGrid->RowCount = FFutRowCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
		FutDrawGrid->ColCount = FFutColCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
		AdjuestFutSize( MarketTabSet->TabIndex );
	}
	FutDrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptMarketTabSetChange(TObject *Sender, int NewTab,
		  bool &AllowChange)
{
	if( NewTab < OptMarketTabSet->Tabs->Count && NewTab >=0 )
	{
		FChanging = true;
		OptExchangeChangeTimer->Enabled = true;
		AllowChange = true;
	}
	else
		AllowChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::OptExchangeChangeTimerTimer(TObject *Sender)
{
	OptExchangeChangeTimer->Enabled = false;
	FChanging = false;
	if( PageControl->ActivePage == OptTabSheet )
	{
		int Ex = OptMarketTabSet->TabIndex;

		SerialComboBox->Clear();
		//SerialComboBox->Items->AddStrings( FOptChineseSerials[ Ex ] );
		for( int i=0;i< FOptChineseSerials[ 0 ]->Count; i ++ )
		{
			String OptCht = gMarketDataStore->GetChinesePrefix( FOptExchangeCode->Strings[Ex], FOptChineseSerials[Ex]->Strings[i] );
			SerialComboBox->Items->Add( OptCht );
		}
		SerialComboBox->ItemIndex = 0;
		SerialComboBoxChange( NULL );
	}
	OptDrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SearchGlyphButtonClick(TObject *Sender)
{
	String SearchStr = SearchEdit->Text;

	FSearchResult->Clear();
	if( SearchStr.Length( ) > 0 )
	{
		SearchGlyphButton->Enabled = false;
		SearchEdit->Enabled   = false;
		Screen->Cursor = crHourGlass;
		for( register int i=0;i< FSearchKeys->Count; i ++ )
		{
			if( FSearchKeys->Strings[ i ].Pos( SearchStr ) != 0 )
				FSearchResult->AddObject( FSearchKeys->Strings[i], FSearchKeys->Objects[i] );
			if( i % 100 == 0 )
				Application->ProcessMessages();
		}
		Screen->Cursor = crDefault;
		SearchGlyphButton->Enabled = true;
		SearchEdit->Enabled   = true;
	}
	FDisplayList = FSearchResult;
	ResultListBox->Count = FSearchResult->Count;
	SearchEdit->Text = L"搜尋股票";
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SearchEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if( Key == VK_RETURN )
		SearchGlyphButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ResultListBoxContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled)
{
	int CurserIndex;

	if( ResultListBox->ItemIndex != -1 &&
		(CurserIndex = ResultListBox->ItemAtPos( MousePos,true)) != -1 &&
		ResultListBox->Selected[ CurserIndex ] )
	{
		TSymbolItem* Item = (TSymbolItem*)ResultListBox->Items->Objects[ CurserIndex ];
		if( Item != NULL)
		{
			BasicInformation* Info = Item->Info;
			if( Info != NULL )
			{
				if( Info->GetTradeFlag() )
				{
					ShowDepthMenuItem->Enabled  = true;
					CloseDepthMenuItem->Enabled = true;
					ChartMenuItem->Enabled      = true;
				}
				else
				{
					ShowDepthMenuItem->Enabled  = false;
					CloseDepthMenuItem->Enabled = false;
					ChartMenuItem->Enabled      = false;
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
void __fastcall TContractInfoForm::ResultListBoxDblClick(TObject *Sender)
{
	if( ResultListBox->ItemIndex != -1 )
	{
		TSymbolItem* Item = (TSymbolItem*)ResultListBox->Items->Objects[ResultListBox->ItemIndex];
		if( Item != NULL )
		{
			BasicInformation* Info = Item->Info;
			if( FPicker == true )
			{
				FPickExchange = Info->GetExchange().c_str();
				FPickSymbol   = Info->GetSymbol().c_str();
				if( FPickSymbol.Length( ) > 0 && FPickExchange.Length( ) > 0 )
					ModalResult   = mrOk;
			}
			else
				ContractViewerForm->OpenOrderBookForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str(), true );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ContractTreeViewEditing(TObject *Sender, TTreeNode *Node,
		  bool &AllowEdit)
{
	AllowEdit = false;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ResultListBoxClick(TObject *Sender)
{
	TSymbolItem* SymbolNode;

	if( ResultListBox->ItemIndex != -1 )
	{
		if((SymbolNode = (TSymbolItem*)ResultListBox->Items->Objects[ResultListBox->ItemIndex])!= NULL )
		{
			SymbolNode->ProdNode->MakeVisible();
			ContractTreeView->Selected = SymbolNode->ProdNode;
			ContractTreeView->SetFocus();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::ContractTreeViewClick(TObject *Sender)
{
	TTreeNode* SymbolNode = ContractTreeView->Selected;

	if( SymbolNode != NULL)
	{
		TStringList* SymList = (TStringList*)SymbolNode->Data;
		if( SymList != NULL)
		{
			FDisplayList = SymList;
			ResultListBox->Count = SymList->Count;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::ResultListBoxData(TWinControl *Control, int Index,
		  UnicodeString &Data)
{
	if( Index >= 0 && Index < FDisplayList->Count )
		Data = FDisplayList->Strings[ Index ];
	else
		Data = L"---";
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::ResultListBoxDataObject(TWinControl *Control, int Index,
		  TObject *&DataObject)
{
	if( Index >= 0 && Index < FDisplayList->Count )
		DataObject = FDisplayList->Objects[ Index ];
	else
		DataObject = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::TypeComboBoxChange(TObject *Sender)
{
	FutDrawGrid->RowCount = FFutRowCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
	FutDrawGrid->ColCount = FFutColCount[ MarketTabSet->TabIndex ][ TypeComboBox->ItemIndex ];
	AdjuestFutSize( MarketTabSet->TabIndex );
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::LoadCustomNames( void )
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
void __fastcall TContractInfoForm::FutTabButtonClick(TObject *Sender)
{
	SwitchPage( 0 );
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::OptTabButtonClick(TObject *Sender)
{
	SwitchPage( 1 );
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::StockTabButtonClick(TObject *Sender)
{
	SwitchPage( 2 );
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::SearchEditMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	SearchEdit->Text = L"";
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::FutSerachEditMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	FutSerachEdit->Text = L"";
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SelectrFutSerials( int Page, int Index )
{
	FFutSelPage  = Page;
	FFutSelIndex = Index;
	TypeComboBox->ItemIndex = Page;
	TypeComboBoxChange( NULL );
	FutDrawGrid->TopRow = Index;
}
//---------------------------------------------------------------------------
void __fastcall TContractInfoForm::SearchFutButtonClick(TObject *Sender)
{
	String SearchStr = FutSerachEdit->Text;

	for( int index = 0; index < MarketTabSet->Tabs->Count; index ++)
	{
		register int i;
		for( i = 0; i < TypeComboBox->Items->Count; i ++)
		{
			TStringList* Names = FChineseSerials[index][ i ];
			for( int j = 0; j < Names->Count; j ++)
			{
				String UnicodeText( Names->Strings[j] );
				if( UnicodeText.Pos( SearchStr ) != 0 )
				{
					SelectrFutSerials( i, j + 1 );
					return;
				}
			}
		}
		for( i = 0; i < TypeComboBox->Items->Count; i ++)
		{
			TStringList* Names = FFutSerials[index][ i ];
			for( int j = 0; j < Names->Count; j ++)
			{
				String UnicodeText( Names->Strings[j] );
				if( UnicodeText.Pos( SearchStr ) != 0 )
				{
					SelectrFutSerials( i, j + 1 );
					return;
				}
			}
		}
	}
	TStringList* Names = FFutSerials[0][4]; ///< Stock Futures
	if( Names != NULL)
	{
		for( int i = 0; i < Names->Count; i ++)
		{
			AnsiString      ProdID( Names->Strings[i] );
			UFC::AnsiString ProductID( ProdID.c_str() );
			TFutProdType    Type = gMarketDataStore->TAIFEXProductType( ProductID );

			if( Type == fptStock )
			{
				UFC::AnsiString StockID;

				gMarketDataStore->TAIFEXStockProduct( ProductID, StockID );
				String UniStockID( StockID.c_str() );

				if( UniStockID.Pos( SearchStr ) != 0 )
				{
					SelectrFutSerials( 4, i + 1 );
					return;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TContractInfoForm::FutSerachEditKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift)
{
	if( Key == VK_RETURN )
		SearchFutButtonClick( this );
}
//---------------------------------------------------------------------------

