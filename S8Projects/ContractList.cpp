//------------------------------------------------------------------------------
#include <vcl.h>
#include "main.h"
#include "FMTConfig.h"
#include "ContractViewer.h"
//#include "OrderForm.h"
#pragma hdrstop
#include "ContractList.h"
#include "MarketDataStore.h"
#include "OrderBookList.h"
#include "OrderStore.h"
#include "UnifyDlg.h"
//#include "PMFNextStrings.hpp"
//#include "ContractListSettingForm.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MarketDataList"
#pragma link "GraphPanel"
#pragma link "GraphButton"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
TContractListForm *ContractListForm;
const String STR_CONTRACTLIST( L"行情表");
//------------------------------------------------------------------------------
extern TCMarketDataStore   *gMarketDataStore;
//------------------------------------------------------------------------------
TColor CLTextColor[2]    = {RGB(51,51,51),RGB(187,187,187)};
TColor CLOddRowColor[2]  = {RGB(255,255,255),RGB(53,53,53)};
TColor CLEvenRowColor[2] = {RGB(248,248,248),RGB(69,69,69)};
TColor CLRiseBKColor[2]  = {RGB(230,0,0),RGB(230,0,0)};
TColor CLFallBKColor[2]  = {RGB(33,140,4),RGB(33,140,4)};
TColor CLSymbolColor[2]  = {RGB(51,51,51),RGB(0,255,255)};
//------------------------------------------------------------------------------
UFC::List<BasicInformation*> TContractListForm::FCustomMDList[CUSTOM_LIST_COUNT];
//------------------------------------------------------------------------------
__fastcall TContractListForm::TContractListForm(TComponent* Owner, int ID )
:TForm(Owner)
,FFontSize( LIST_FONT_SIZE_DEF )
,FColumns( "3=100,16=135,17=164,4=66,1=100,5=99,2=100,6=99,10=100,11=100,12=100,7=66,13=99,18=192," )
{
	SetContractListFormID( ID );
	FFutExchanges = new TStringList();
	MarketDataList->Parent = this;
	RoundFormEx->Lockbox = false;
	ToolSV->UseAnimation = false;
	ToolSV->Opened = false;
	ToolSV->UseAnimation = true;
	DefaultMonitor = dmMainForm;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:	  Msg.Result = 1; return;
		case WM_MDIACTIVATE:  Msg.Result = 1; return;
		case WM_MOUSEACTIVATE:BringWindowToTop( Handle );
							  ContractViewerForm->ActiveClient( this );
							  Msg.Result = MA_ACTIVATE;
							  return;
	}
	inherited::WndProc( Msg );
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::SetContractListFormID( int ID )
{
	String FormName;

	FID = ID;
    if( FID == 0 )
		FormName = STR_CONTRACTLIST;///< 商品表
    else
		FormName.printf( L"%s-%d", STR_CONTRACTLIST, ID );
    Caption = FormName;
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::FormCreate(TObject *Sender)
{
	MarketDataList->Store = gMarketDataStore;
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::RemoveAllSymbols( void )
{
	///< Remove all old symbols
	MarketDataList->RemoveAllSymbol();
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::SerialComboBoxChange(TObject *Sender)
{
	UFC::PStringList Symbols;
	String Exchange;
	int ExchangeIndex = ExchangeComboBox->ItemIndex - CUSTOM_LIST_COUNT;

	if( ExchangeIndex >= 0 )
		Exchange = FFutExchanges->Strings[ ExchangeIndex ];
	else
		Exchange = FFutExchanges->Strings[ 0 ];
	String* ProductID = (String*)SerialComboBox->Items->Objects[SerialComboBox->ItemIndex];
	UFC::List<BasicInformation*>* SymbolList = (UFC::List<BasicInformation*>*)gMarketDataStore->GetMDSeries( Exchange, *ProductID );

	for( register int i = 0; i < SymbolList->ItemCount(); i++ )
	{
		BasicInformation* Info = SymbolList->GetItem(i);
		if( Info->IsMultileg( ) == false  )
			Symbols.Add( Info->GetSymbol() );
	}
	AddSymbols( Exchange, Symbols, true );
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::AddSymbols( String Exchange, UFC::PStringList& Symbols, bool NeedSort )
{
	MarketDataList->Sort = NeedSort;
	MarketDataList->BeginUpdate();
	///< Remove all old symbols
	MarketDataList->RemoveAllSymbol();
	///< Remove new symbols
	for( register int i = 0; i < Symbols.ItemCount(); i++ )
	{
		BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, Symbols[i].c_str(), false );
		if( Info != NULL )
			 MarketDataList->AddSymbol( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
	}
	MarketDataList->EndUpdate();
	FitTimer->Enabled = true;
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::AddSymbol( int index, String Exchange, String Symbol )
{
	UFC::AnsiString AddSym;
	BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false );

	if( Info != NULL )
	{
		if( ( FCustomMDList[index].IndexOf( Info ) ) == -1 )
			FCustomMDList[index].Add( Info );
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::RemoveSymbol( int index, String Exchange, String Symbol )
{
	int DelIndex;
	UFC::AnsiString DelSym;
	BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false );

	if( Info != NULL )
	{
		if( (DelIndex = FCustomMDList[index].IndexOf( Info )) != -1 )
			FCustomMDList[index].Delete( DelIndex );
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::ToCustomList( int index )
{
	if( index >= 0 && index < CUSTOM_LIST_COUNT )
	{
		ExchangeComboBox->ItemIndex = index;
        ExchangeComboBoxChange( this );
    }
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::Init( const String& Exchange, int ProdID )
{
	MarketDataList->SetColumns( FColumns );
	MarketDataList->OnColumnClick = OnColumnClick;
	if( FFutExchanges->Count == 0 )
	{
		for( register int i = 0; i < gMarketDataStore->ExchangeCount(); i++ )
		{
			String ExCode( gMarketDataStore->GetExchange( i ).c_str( ) );

			if( g_Config.SupportExchange( ExCode ) == true &&
				g_Config.FuturesExchange( ExCode ) )
			{
				FFutExchanges->Add( ExCode );
				ExchangeComboBox->Items->Add( gMarketDataStore->GetExchangeDisplayName( ExCode ) );
			}
		}
	}
	int ExIndex = FFutExchanges->IndexOf( Exchange );
	if( ExIndex == -1 ) ///< Exchange not found
		ExchangeComboBox->ItemIndex = 0;
	else   ///< Find Exchange
		ExchangeComboBox->ItemIndex = ExIndex + CUSTOM_LIST_COUNT;
	ExchangeComboBoxChange( this );
	if( ExchangeComboBox->ItemIndex >= CUSTOM_LIST_COUNT )
	{
		if( ProdID < SerialComboBox->Items->Count )
			SerialComboBox->ItemIndex = ProdID;
		else
			SerialComboBox->ItemIndex = 0;
		SerialComboBoxChange( this );
	}
	LoadColor( );
	LoadCustomNames();
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::OnColumnClick(TObject *Sender, TListColumn *Column)
{
	int clickedTag = Column->Tag;
	MarketDataList->SortByTag(clickedTag);
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::ForceSortSymbol( const String& Exchange,const String& SerieName, int Place )
{
	int    SIndex;
	String DspSerial = gMarketDataStore->GetChinesePrefix( Exchange, SerieName );

	if( (SIndex = SerialComboBox->Items->IndexOf( DspSerial ))!= -1 )
		SerialComboBox->Items->Exchange( SIndex, Place );
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::ForceSortSymbol( const String& Exchange )
{
	if( Exchange == L"TAIFEX" || Exchange == L"TFX" )
	{
		ForceSortSymbol( Exchange, L"TXF", 0 );
		ForceSortSymbol( Exchange, L"MXF", 1 );
		ForceSortSymbol( Exchange, L"TMF", 2 );
		ForceSortSymbol( Exchange, L"EXF", 3 );
		ForceSortSymbol( Exchange, L"FXF", 4 );
		ForceSortSymbol( Exchange, L"XIF", 5 );
		ForceSortSymbol( Exchange, L"GDF", 6 );
		ForceSortSymbol( Exchange, L"GTF", 7 );
	}
	else if( Exchange == "CME" )
	{
		ForceSortSymbol( Exchange, L"ADSS", 0 );
		ForceSortSymbol( Exchange, L"BPSS", 1 );
		ForceSortSymbol( Exchange, L"CDSS", 2 );
		ForceSortSymbol( Exchange, L"ECSS", 3 );
		ForceSortSymbol( Exchange, L"JYSS", 4 );
		ForceSortSymbol( Exchange, L"NKD",  5 );
		ForceSortSymbol( Exchange, L"SFSS", 6 );
		ForceSortSymbol( Exchange, L"ES",   7 );
		ForceSortSymbol( Exchange, L"NQ",   8 );
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::ClearSerialComboBox(  void )
{
	for( register int i = 0; i < SerialComboBox->Items->Count; i++ )
	{
		String* DelStr = (String*)SerialComboBox->Items->Objects[i];
		delete DelStr;
	}
	SerialComboBox->Items->Clear();
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::CustomListMode( bool Custom )
{
	if( Custom == true )
	{
		SerialComboBox->Visible = false;
		LargeButton->Left = SerialComboBox->Left;
		SmallButton->Left = LargeButton->Left + 38;
	}
	else
	{
		SerialComboBox->Visible = true;
		LargeButton->Left = SerialComboBox->Left +  SerialComboBox->Width + 2;
		SmallButton->Left = LargeButton->Left + 38;
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::ExchangeComboBoxChange(TObject *Sender)
{
	if( ExchangeComboBox->ItemIndex == -1 )
		return;
	if( ExchangeComboBox->ItemIndex < CUSTOM_LIST_COUNT )
	{
		BasicInformation* Info;
		ClearSerialComboBox();
		CustomListMode( true );
		MarketDataList->Sort = false;
		MarketDataList->BeginUpdate();
		MarketDataList->RemoveAllSymbol();
		for( register int i = 0; i < FCustomMDList[ ExchangeComboBox->ItemIndex ].ItemCount(); i++ )
		{
			if( (Info = FCustomMDList[ ExchangeComboBox->ItemIndex ].GetItem(i)) != NULL  )
				MarketDataList->AddSymbol( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );;///< Custom list not need sort.
		}
		MarketDataList->EndUpdate();
		FitTimer->Enabled = true;
	}
	else
	{
		int          ExchangeIndex = ExchangeComboBox->ItemIndex - CUSTOM_LIST_COUNT;
		TStringList* ProductIDList;
		String       Exchange;
		CustomListMode( false );
		ClearSerialComboBox( );
		if( ExchangeIndex >= 0 )
			Exchange = FFutExchanges->Strings[ ExchangeIndex ];
		else
			Exchange = FFutExchanges->Strings[ 0 ];
		if( (ProductIDList = (TStringList*)gMarketDataStore->GetFUTMDPrefix( Exchange )) != NULL )
		{
			for( register int i = 0; i < ProductIDList->Count; i++ )
			{
				String DisplayText = gMarketDataStore->GetChinesePrefix( Exchange, ProductIDList->Strings[i] );
				SerialComboBox->AddItem( DisplayText, (TObject*)new String( ProductIDList->Strings[i] ) );
			}
			ForceSortSymbol( Exchange );
			if( SerialComboBox->ItemIndex == -1 )
				SerialComboBox->ItemIndex = 0;
			SerialComboBoxChange( this );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::ChangeToPage( int ExchangeIndex, int SerialIndex )
{
	if( ExchangeIndex < 0 || ExchangeIndex >= ExchangeComboBox->Items->Count )
		return;
	ExchangeComboBox->ItemIndex = ExchangeIndex;
	if( ExchangeComboBox->ItemIndex < CUSTOM_LIST_COUNT )
	{
		BasicInformation* Info;
		SortButton->Enabled = true;
		SortButton->Invalidate();
		ClearSerialComboBox();
		SerialComboBox->Enabled = false;
		MarketDataList->Sort = false;
		MarketDataList->BeginUpdate();
		MarketDataList->RemoveAllSymbol();
		for( register int i = 0; i < FCustomMDList[ ExchangeComboBox->ItemIndex ].ItemCount(); i++ )
		{
			if( (Info = FCustomMDList[ ExchangeComboBox->ItemIndex ].GetItem(i)) != NULL  )
				MarketDataList->AddSymbol( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );;///< Custom list not need sort.
		}
		MarketDataList->EndUpdate();
		MarketDataList->Invalidate();
	}
	else
	{
		int          ExchangeIndex = ExchangeComboBox->ItemIndex - CUSTOM_LIST_COUNT;
		TStringList* ProductIDList;
		String       Exchange;

		SortButton->Enabled = false;
		SortButton->Invalidate();
		SerialComboBox->Enabled = true;
		ClearSerialComboBox( );
		if( ExchangeIndex >= 0 )
			Exchange = FFutExchanges->Strings[ ExchangeIndex ];
		else
			Exchange = FFutExchanges->Strings[ 0 ];
		if( (ProductIDList = (TStringList*)gMarketDataStore->GetFUTMDPrefix( Exchange )) != NULL )
		{
			for( register int i = 0; i < ProductIDList->Count; i++ )
			{
				String DisplayText = gMarketDataStore->GetChinesePrefix( Exchange, ProductIDList->Strings[i] );
				SerialComboBox->AddItem( DisplayText, (TObject*)new String( ProductIDList->Strings[i] ) );
			}
			ForceSortSymbol( Exchange );
			if( SerialIndex < 0 || SerialIndex >= SerialComboBox->Items->Count )
				SerialComboBox->ItemIndex = 0;
			else
				SerialComboBox->ItemIndex = SerialIndex;
			SerialComboBoxChange( this );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::SaveCustomList( const String& Profile )
{
	String Dir;

	for( int j = 0; j < CUSTOM_LIST_COUNT; j++ )
	{
		Dir.printf( L"ContractList\\Custom%d", j );
		g_Config.SetDesktopInteger( Dir, "SymCount", FCustomMDList[ j ].ItemCount() );
		for( int i = 0; i< FCustomMDList[ j ].ItemCount(); i ++ )
		{
			String ExchangeName = "Exchange" + IntToStr( i );
			String KeyName = "Symbol" + IntToStr( i );

			g_Config.SetDesktopString( Dir, ExchangeName, FCustomMDList[ j ].GetItem(i)->GetExchange().c_str() );
			g_Config.SetDesktopString( Dir, KeyName,  FCustomMDList[ j ].GetItem(i)->GetSymbol().c_str() );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::LoadCustomList( const String& Profile )
{
	String Dir;

	for( int j = 0; j < CUSTOM_LIST_COUNT; j++ )
	{
		FCustomMDList[ j ].Clear();
		Dir.printf( L"ContractList\\Custom%d", j );
		int Count = g_Config.GetDesktopInteger( Dir, "SymCount", 0 );
		for( int i =0; i< Count; i ++ )
		{
			String ExchangeName = "Exchange" + IntToStr( i );
			String KeyName 	= "Symbol" + IntToStr( i );
			String Exchange	= g_Config.GetDesktopString( Dir, ExchangeName, "" );
			String Symbol  	= g_Config.GetDesktopString( Dir, KeyName, "" );
			if( Exchange.Length() > 0 && Symbol.Length() > 0 )
			{
				BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false );
				if( Info != NULL )
					FCustomMDList[ j ].Add( Info );
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::SaveProperties( const String& Profile )
{
	String ColumnSettings, Dir;
	String RegKey;

	RegKey.printf( L"%s\\ContractList%d", Profile.c_str(), FID );
	///< Save columns
	MarketDataList->GetColumns( ColumnSettings );
	if( ColumnSettings.Length( ) > 0 )
		g_Config.SetDesktopString( RegKey, "Columns", ColumnSettings  );
	g_Config.SetDesktopInteger( RegKey, "FontSize", FFontSize );
	///< Save custom lists.
	SaveCustomList( Profile );
    ///< Save select list.
    g_Config.SetDesktopInteger( RegKey, "Exchange", ExchangeComboBox->ItemIndex );
    g_Config.SetDesktopInteger( RegKey, "Serial", SerialComboBox->ItemIndex );
    ///< Save windows position.
	//g_Config.SetDesktopFormPos( RegKey, "Form", RoundFormEx );
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::LoadProperties( const String& Profile )
{
	String Dir, ColumnSettings;
	String RegKey;

	RegKey.printf( L"%s\\ContractList%d", Profile.c_str(), FID );
	///< Load columns
	ColumnSettings = g_Config.GetDesktopString( RegKey, "Columns", "" );
	if( ColumnSettings.Length( ) > 0 )
	{
		MarketDataList->SetColumns( ColumnSettings );
		FColumns = ColumnSettings;
	}
	FFontSize = g_Config.GetDesktopInteger( RegKey, "FontSize", LIST_FONT_SIZE_DEF );
	MarketDataList->Font->Size = FFontSize;
	///< Load custom lists.
	LoadCustomList( Profile );
	///< Set Exchange/Serial ComboBox
	ExchangeComboBox->ItemIndex = g_Config.GetDesktopInteger( RegKey, "Exchange", 0 );
	ExchangeComboBoxChange( this );
	if( ExchangeComboBox->ItemIndex >= CUSTOM_LIST_COUNT )
	{
		SerialComboBox->ItemIndex = g_Config.GetDesktopInteger( RegKey, "Serial", 0 );
		SerialComboBoxChange( this );
	}
	///< Load windows position.
	//g_Config.GetDesktopFormPos( RegKey, "Form", RoundFormEx );
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::MarketDataListMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	if( Shift.Contains( ssRight ) && MarketDataList->Selected != NULL )
	{
		TPoint     ScrPoint( X, Y );
		TListItem* SelItem = MarketDataList->GetItemAt( X, Y );

		if( SelItem != NULL )
		{
			BasicInformation* Info = (BasicInformation*)SelItem->Data;

			if( ExchangeComboBox->ItemIndex < CUSTOM_LIST_COUNT )
				DelSymbolMenuItem->Enabled = true;
			else
				DelSymbolMenuItem->Enabled = false;
			if( Info->GetTradeFlag() == true )
				OpenDepthMenuItem->Enabled = true;
			else
				OpenDepthMenuItem->Enabled = false;
			ScrPoint = ClientToScreen( ScrPoint );
			PopupMenu->Popup( ScrPoint.x, ScrPoint.y );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::OpenDepthMenuItemClick(TObject *Sender)
{
	for( int i = MarketDataList->ItemCount()-1; i>=0; i -- )
	{
		TListItem* OpenItem = MarketDataList->Items->Item[ i ];
		if( OpenItem != NULL )
		{
			BasicInformation* Info = (BasicInformation*)OpenItem->Data;
			if( OpenItem->Selected == true )
				ContractViewerForm->OpenOrderBookForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::DelSymbolMenuItemClick(TObject *Sender)
{
	for( int i = MarketDataList->ItemCount()-1; i>=0; i -- )
	{
		TListItem* DelItem = MarketDataList->Items->Item[ i ];
		if( DelItem->Selected == true )
		{
			BasicInformation* Info = (BasicInformation*)DelItem->Data;
			RemoveSymbol( ExchangeComboBox->ItemIndex, Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
			MarketDataList->RemoveSymbol( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		}
	}
	SaveCustomList( "SpeedyUnify" );
	ContractViewerForm->LoadCustomList( this, ExchangeComboBox->ItemIndex );
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::LargeButtonClick(TObject *Sender)
{
	FFontSize++;
	if( FFontSize > LIST_FONT_SIZE_MAX )
	{
		FFontSize = LIST_FONT_SIZE_MAX;
		return;
	}
	MarketDataList->Font->Size = FFontSize;
	MarketDataList->AdjuestCol();
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::SmallButtonClick(TObject *Sender)
{
	FFontSize--;
	if( FFontSize < LIST_FONT_SIZE_MIN )
	{
		FFontSize = LIST_FONT_SIZE_MIN;
		return;
	}
	MarketDataList->Font->Size = FFontSize;
	MarketDataList->AdjuestCol();
}
//------------------------------------------------------------------------------
void __fastcall TContractListForm::UpdateContract( const String& SDExchange )
{
	int OrigIndex  = ExchangeComboBox->ItemIndex;

	if( OrigIndex >= 0 )
	{
		if( OrigIndex < CUSTOM_LIST_COUNT )
		{
			BasicInformation* Info;

			for( register int i = 0; i < FCustomMDList[ ExchangeComboBox->ItemIndex ].ItemCount(); i++ )
			{
				if( (Info = FCustomMDList[ ExchangeComboBox->ItemIndex ].GetItem(i)) != NULL  )
				{
					String Exchange( Info->GetExchange().c_str() );

					if( Exchange == SDExchange )
					{
						ChangeToPage( OrigIndex, 0 );
						return;
					}
				}
			}
		}
		else
		{
			int    ExchangeIndex = ExchangeComboBox->ItemIndex - CUSTOM_LIST_COUNT;
			int    OrigSerial = SerialComboBox->ItemIndex;
			String Exchange;

			if( ExchangeIndex >= 0 )
				Exchange = FFutExchanges->Strings[ ExchangeIndex ];
			else
				Exchange = FFutExchanges->Strings[ 0 ];
			if( Exchange == SDExchange )
				ChangeToPage( OrigIndex, OrigSerial );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::Release( void )
{
	MarketDataList->BeginUpdate();
	MarketDataList->RemoveAllSymbol();
	MarketDataList->EndUpdate();
	MarketDataList->Store = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::MarketDataListItemPosChanged(TObject *Sender)
{
	if( ExchangeComboBox->ItemIndex < CUSTOM_LIST_COUNT )
	{
		FCustomMDList[ ExchangeComboBox->ItemIndex ].Clear();
		for( int i = 0; i < MarketDataList->ItemCount(); i ++ )
		{
			BasicInformation* Info = MarketDataList->GetSymbol( i );
			FCustomMDList[ ExchangeComboBox->ItemIndex ].Add( Info );
		}
		SaveCustomList( "SpeedyUnify" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::SortButtonClick(TObject *Sender)
{
  if (ToolSV->Opened)
	  ToolSV->Close();
  else
	  ToolSV->Open();
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::FitTimerTimer(TObject *Sender)
{
	FitTimer->Enabled = false;
	MarketDataList->Font->Size = FFontSize;
	MarketDataList->AdjuestCol();
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::DefaultColor( void )
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	MarketDataList->TextColor     = CLTextColor[DorL];
	MarketDataList->OddRowColor   = CLOddRowColor[DorL];
	MarketDataList->EvenRowColor  = CLEvenRowColor[DorL];
	MarketDataList->RiseBKColor   = CLRiseBKColor[DorL];
	MarketDataList->FallBKColor   = CLFallBKColor[DorL];
	MarketDataList->SymbolColor   = CLSymbolColor[DorL];

	MarketDataList->Invalidate();
	TextColorBox->Selected    = MarketDataList->TextColor;
	OddBKColorBox->Selected   = MarketDataList->OddRowColor;
	EvenBKColorBox->Selected  = MarketDataList->EvenRowColor;
	RiseBKColorBox->Selected  = MarketDataList->RiseBKColor;
	FallBKColorBox->Selected  = MarketDataList->FallBKColor;
	SymbolColorBox->Selected  = MarketDataList->SymbolColor;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::LoadColor( void )
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	MarketDataList->TextColor    = (TColor)g_Config.GetIntegerProperty("Setting","CLTextColor",    CLTextColor[DorL] );
	MarketDataList->OddRowColor  = (TColor)g_Config.GetIntegerProperty("Setting","CLOddRowColor",  CLOddRowColor[DorL] );
	MarketDataList->EvenRowColor = (TColor)g_Config.GetIntegerProperty("Setting","CLEvenRowColor", CLEvenRowColor[DorL] );
	MarketDataList->RiseBKColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLRiseBKColor", CLRiseBKColor[DorL] );
	MarketDataList->FallBKColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLFallBKColor", CLFallBKColor[DorL] );
	MarketDataList->SymbolColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLSymbolColor", CLSymbolColor[DorL] );
	MarketDataList->Invalidate();
	TextColorBox->Selected    = MarketDataList->TextColor;
	OddBKColorBox->Selected   = MarketDataList->OddRowColor;
	EvenBKColorBox->Selected  = MarketDataList->EvenRowColor;
	RiseBKColorBox->Selected  = MarketDataList->RiseBKColor;
	FallBKColorBox->Selected  = MarketDataList->FallBKColor;
	SymbolColorBox->Selected  = MarketDataList->SymbolColor;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::SaveColor( void )
{
	g_Config.SetIntegerProperty( "Setting","CLTextColor",    TextColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLOddRowColor",  OddBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLEvenRowColor", EvenBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLRiseBKColor",  RiseBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLFallBKColor",  FallBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLSymbolColor",  SymbolColorBox->Selected );
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::ColorBoxChange(TObject *Sender)
{
	MarketDataList->TextColor     = TextColorBox->Selected;
	MarketDataList->OddRowColor   = OddBKColorBox->Selected;
	MarketDataList->EvenRowColor  = EvenBKColorBox->Selected;
	MarketDataList->RiseBKColor   = RiseBKColorBox->Selected;
	MarketDataList->FallBKColor   = FallBKColorBox->Selected;
	MarketDataList->SymbolColor   = SymbolColorBox->Selected;
	MarketDataList->Invalidate();
	SaveColor();
	ContractViewerForm->LoadContractListColor( this );
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::DefColorButtonClick(TObject *Sender)
{
	DefaultColor();
	SaveColor();
	ContractViewerForm->LoadContractListColor( this );
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::LoadCustomList( int Index )
{
	LoadCustomList( "SpeedyUnify" );
	if( ExchangeComboBox->ItemIndex == Index )
		ExchangeComboBoxChange( this );
}
//---------------------------------------------------------------------------

void __fastcall TContractListForm::OpenKMenuItemClick(TObject *Sender)
{
	TListItem* OpenItem = MarketDataList->Selected;
	if( OpenItem != NULL )
	{
		BasicInformation* Info = (BasicInformation*)OpenItem->Data;
		if( OpenItem->Selected == true )
			ContractViewerForm->OpenKBarForm( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::CustomNameMenuItemClick(TObject *Sender)
{
	int SelItem = ExchangeComboBox->ItemIndex;
	String NewName = ExchangeComboBox->Items->Strings[ SelItem ];


	if( TUnifyDlgs::AskStringDialog( L"重新命名", NewName ) == true && NewName.Length() > 1 )
	{
		String KeyName;

		KeyName.printf( L"Name%d", SelItem + 1 );
		ExchangeComboBox->Items->Strings[ SelItem ] = NewName;
		ExchangeComboBox->ItemIndex = SelItem;
		g_Config.SetStringProperty( "CustomNames",KeyName, NewName );
		ContractViewerForm->LoadCustomName( this );
	}

}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::LoadCustomNames( void )
{
	int SelItem = ExchangeComboBox->ItemIndex;

	for( int i = 0; i < CUSTOM_LIST_COUNT; i++ )
	{
		String KeyName,DefName, CustomName;

		DefName.printf( L"自選商品-%d", i + 1 );
		KeyName.printf( L"Name%d", i + 1 );
		CustomName = g_Config.GetStringProperty( "CustomNames",KeyName, DefName );
		ExchangeComboBox->Items->Strings[ i ] = CustomName;
		if( SelItem != -1 )
			ExchangeComboBox->ItemIndex = SelItem;
	}
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::CustomPopupMenuPopup(TObject *Sender)
{
	int SelItem = ExchangeComboBox->ItemIndex;

	if( SelItem >= CUSTOM_LIST_COUNT )
		CustomNameMenuItem->Enabled = false;
	else
        CustomNameMenuItem->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::SetVisible( bool IsVisible )
{
	if( IsVisible == false )
		Visible = false;
	else
		Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TContractListForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	String	Name = L"SpeedyUnify\\List" + IntToStr( FID );

	g_Config.SetDesktopInteger( Name, L"Page", GetGroup() );
	g_Config.SetDesktopFormPos( Name, L"Form", RoundFormEx );
	SaveProperties( Name );
}
//---------------------------------------------------------------------------

