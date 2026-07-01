//---------------------------------------------------------------------------

#include <vcl.h>
#include "FMTConfig.h"
#include "main.h"
#include "TBarForm.h"
#include "ContractViewer.h"
#pragma hdrstop
#include "UnifyUtility.h"
#include "MarketDataStore.h"
//#include "PMFNextStrings.hpp"
//#include "TBarSettingForm.h"
//#include "StrikePxSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphPanel"
#pragma link "OptionsStrikePriceView"
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "cgauges"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
extern TCMarketDataStore   *gMarketDataStore;
//---------------------------------------------------------------------------
const int FONT_SIZE_MIN = 7;
const int FONT_SIZE_DEF = 11;
const int FONT_SIZE_MAX = 28;
//---------------------------------------------------------------------------
TOptionsTBarForm *OptionsTBarForm;
const String STR_TBAR_OPTION( L"選擇權報價" );
const String STR_TBAR_ALL_MONTH( L"所有" );
//---------------------------------------------------------------------------
TColor HeaderCommodColor[2]      = {RGB(252,255,23),RGB(1,234,237)};
TColor HeaderCallColor[2]        = {RGB(252,255,23),RGB(1,234,237)};
TColor HeaderPutColor[2]         = {RGB(252,255,23),RGB(1,234,237)};
TColor HeaderCommodBKColor[2]    = {RGB(40,90,159),RGB(45,45,45)};
TColor HeaderCallBKColor[2]      = {RGB(40,90,159),RGB(45,45,45)};
TColor HeaderPutBKColor[2]       = {RGB(40,90,159),RGB(45,45,45)};
TColor FieldNameCommodColor[2]   = {RGB(34,34,34),RGB(238,238,238)};
TColor FieldNameCallColor[2]     = {RGB(34,34,34),RGB(238,238,238)};
TColor FieldNamePutColor[2]      = {RGB(34,34,34),RGB(238,238,238)};
TColor FieldNameCallBKColor[2]   = {clWhite,RGB(104,104,104)};
TColor FieldNamePutBKColor[2]    = {clWhite,RGB(104,104,104)};
TColor FieldNameCommodBKColor[2] = {clWhite,RGB(104,104,104)};
TColor StkPxColor[2]             = {clWhite,clWhite};
TColor StkPxBKColor[2]           = {RGB(40,90,159),RGB(53,53,53)};
TColor CallBKColor[2]            = {clWhite,RGB(53,53,53)};
TColor PutBKColor[2]             = {clWhite,RGB(53,53,53)};
TColor RisePxColor[2]            = {RGB(230,0,0),RGB(230,0,0)};
TColor FallPxColor[2]            = {RGB(33,140,4),RGB(33,140,4)};
TColor EqualPxColor[2]           = {RGB(85,85,85),RGB(153,153,153)};
TColor HGridColor[2]             = {RGB(205,205,205),RGB(73,73,73)};
TColor VGridColor[2]             = {clWhite,RGB(53,53,53)};
TColor BKBarColor[2]             = {RGB(255,246,231),RGB(8,16,27)};
//---------------------------------------------------------------------------
__fastcall TOptionsTBarForm::TOptionsTBarForm(TComponent* Owner, int ID )
:TForm(Owner)
,FFontSize( FONT_SIZE_DEF )
,FDepthFormIndex( -1 )///< -1 means open new.
,FIsInit( false )
,FOpeningTBarOrderBook( false )
{
	FOptExchanges = new	TStringList();
	FOptSerial    = new TStringList();
	SetTBarFormID( ID );
	ToolSV->UseAnimation = false;
	ToolSV->Opened = false;
	ToolSV->UseAnimation = true;

	FOrigWindowProc    = FPList->WindowProc;
	FPList->WindowProc = ListWndProc;
	FEarseBKCanvas     = new TCanvas();
	DefaultMonitor = dmMainForm;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::ListWndProc( TMessage &Msg )
{
	NMHDR* pnmh;
	POINT Point;
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			if(  FPList->ViewStyle == vsReport )
			{
				EraseBK( (HDC) Msg.WParam );
				Msg.Result = 1;
			}
			else
				FOrigWindowProc( Msg );
			break;
		default:
			FOrigWindowProc( Msg );
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			Msg.Result = 1;
			return;
		case WM_MDIACTIVATE:
			 Msg.Result = 1;
			 return;
		case WM_MOUSEACTIVATE:
			 Msg.Result = MA_ACTIVATE;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::EraseBK( HDC DC )
{
	HWND HeaderHandle = ListView_GetHeader( FPList->Handle );
	TRect HeaderItemRect, HeaderRect;

	FEarseBKCanvas->Handle = DC;
	Header_GetItemRect( HeaderHandle, 0, &HeaderItemRect );
	HeaderRect = Rect( 0, 0, FPList->Width, HeaderItemRect.Height() );
	TRect RepaintRect = Rect( 0, 0, FPList->Width, FPList->Height );
	FEarseBKCanvas->Brush->Color = clSilver;//FPList->Color;
	HRGN MainRgn = CreateRectRgnIndirect( &RepaintRect ); ///< TList rect
	HRGN HeaderRgn = CreateRectRgnIndirect( &HeaderRect );///< Header rect
	CombineRgn( MainRgn, MainRgn, HeaderRgn, RGN_DIFF );  ///< Repaint rect = TList rect - Header rect
	if( FPList->Items->Count > 0 )
	{
		TRect ItemRect;
		int width, height;

		///< Get the last item rect.
		ListView_GetItemRect( FPList->Handle, FPList->Items->Count - 1, &ItemRect, LVIR_BOUNDS );
		if( ItemRect.Bottom > FPList->Height )
			height = FPList->Height;
		else
			height = ItemRect.Bottom;
		width = ItemRect.Width();
		TRect TmpRect = Rect( 0, HeaderRect.Height(), width, height );
		HRGN ItemRgn = CreateRectRgnIndirect( &TmpRect );     ///< Items rect
		CombineRgn( MainRgn, MainRgn, ItemRgn, RGN_DIFF );    ///< Repaint rgn -= Items rect
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle );///< Fill Repaint rgn
		DeleteObject( (HGDIOBJ)ItemRgn );
	}
	else
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle ); ///< No items, fill Repaint rect
	DeleteObject( (HGDIOBJ)MainRgn );
	DeleteObject( (HGDIOBJ)HeaderRgn );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::InitExchange( int ExIndex )
{
	///< Add Exchange list.
	FOptExchanges->Clear();
	FOptExchanges->AddStrings( ContractInfoForm->OptExchanges() );
	///< Add Exchange tabs.
	OptMarketTabSet->Tabs->Clear();
	OptMarketTabSet->Tabs->AddStrings( ContractInfoForm->OptMarketTabSet->Tabs );
	OptMarketTabSet->TabIndex = ExIndex;
	if( OptMarketTabSet->Tabs->Count <=1 )
		OptMarketTabSet->Height = 0;
	else
		OptMarketTabSet->Height = 27;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::InitSerial( int ExIndex, int SerialIndex )
{
	String Ex( FOptExchanges->Strings[ ExIndex ] );
	///< Add Serial list.
	FOptSerial->Clear();
	FOptSerial->AddStrings( ContractInfoForm->OptSerils( ExIndex ) );
	///< Add Serial Chinese name into combobox.
	OptionsSerialComboBox->Items->Clear();
	for( int i = 0; i < FOptSerial->Count; i ++ )
		OptionsSerialComboBox->Items->Add( gMarketDataStore->GetChinesePrefix( Ex,FOptSerial->Strings[i] ));
	OptionsSerialComboBox->ItemIndex = SerialIndex;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::InitTBar( void )
{
	if( FIsInit == false )
	{
		InitExchange( 0 ); ///< First exchange.
		InitSerial( 0, 0 );///< First exchange and first serial.
		OptionsSerialComboBoxChange( this );
		SelectPinsButtonClick(NULL);
		FIsInit = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SetTBarFormID( int ID )
{
	String FormName;

	FID = ID;

	if( FID == 0 )
		FormName.printf( L"%s", STR_TBAR_OPTION ); ///< 選擇權報價
	else
		FormName.printf( L"%s-%d", STR_TBAR_OPTION, ID ); ///< 選擇權報價
	Caption = FormName;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SetMaturityComboBox( const String& Exchange, const String& SerialName )
{
	TStringList* MYlist  = (TStringList*)gMarketDataStore->GetOPTMonthYear( Exchange, SerialName );

	MaturityComboBox->Sorted = true;
	MaturityComboBox->Items->Clear();
	MaturityComboBox->Items->AddStrings( MYlist );
	if( MaturityComboBox->Items->Strings[0] == L"(null)" )
        MaturityComboBox->Items->Delete( 0 );
	//MaturityComboBox->Items->Add( STR_TBAR_ALL_MONTH );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::OptionsSerialComboBoxChange(TObject *Sender)
{
	String       Exchange   = FOptExchanges->Strings[ OptMarketTabSet->TabIndex ];
	String       SerialName = FOptSerial->Strings[ OptionsSerialComboBox->ItemIndex ];

	SetMaturityComboBox( Exchange, SerialName );
	OptionsStrikePriceView->DisplayName = SerialName;
	if( Sender != NULL )
	{
		MaturityComboBox->ItemIndex = 0;
		MaturityComboBoxChange( this );
	}
}
//---------------------------------------------------------------------------
String __fastcall TOptionsTBarForm::GetMaturityString( void )
{
	return MaturityComboBox->Items->Strings[ MaturityComboBox->ItemIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::MaturityComboBoxChange(TObject *Sender)
{
	SavePinsSetting( );
	///< setup TBar control.
	OptionsStrikePriceView->Exchange  = FOptExchanges->Strings[ OptMarketTabSet->TabIndex ];;
	OptionsStrikePriceView->Symbol    = FOptSerial->Strings[ OptionsSerialComboBox->ItemIndex ];
	OptionsStrikePriceView->YearMonth = GetMaturityString();
	OptionsStrikePriceView->IsReverse =	InvertCheckBox->Checked;
	UpdateOptionsStrikePriceViewPins( );
	LoadPinsSetting();
	if( this->Visible == true )
		OptionsStrikePriceView->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::FormCreate(TObject *Sender)
{
	OptionsStrikePriceView->Store = gMarketDataStore;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::InvertCheckBoxClick(TObject *Sender)
{
	OptionsStrikePriceView->IsReverse =	InvertCheckBox->Checked;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::FormShow(TObject *Sender)
{
	InitTBar();
	AdjuestFont();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::LargeButtonClick(TObject *Sender)
{
	FFontSize++;
	AdjuestFont();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SmallButtonClick(TObject *Sender)
{
	FFontSize--;
	AdjuestFont();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::AdjuestFont( void )
{
	TFont* UseFont = new TFont();
	if( FFontSize < FONT_SIZE_MIN )
		FFontSize = FONT_SIZE_MIN;
	if( FFontSize > FONT_SIZE_MAX )
		FFontSize = FONT_SIZE_MAX;
	UseFont->Size = FFontSize;
    UseFont->Name = Font->Name;
	OptionsStrikePriceView->Font->Assign( UseFont );
	delete UseFont;
	AdjuestWidth( );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::AdjuestWidth( void )
{
	int TBarWidth;

	OptionsStrikePriceView->CalSize();
	TBarWidth = OptionsStrikePriceView->BestFitWidth + RoundFormEx->LeftGraph->Width/2 +  RoundFormEx->RightGraph->Width/2 + 18;
	Constraints->MaxWidth = TBarWidth;
	Constraints->MinWidth = TBarWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::GenStrikePxSet( UFC::PHashedSet<double>* StrikePxSet, const String& StrikePxString )
{
	TStringList*  ExcludeStrikelist = new TStringList();

	ExcludeStrikelist->Delimiter = ',';
	ExcludeStrikelist->DelimitedText = StrikePxString;
	try
	{
		for( int i = 0 ;i < ExcludeStrikelist->Count; i++ )
		{
			double StkPx = ExcludeStrikelist->Strings[ i ].ToDouble();
			StrikePxSet->Add( StkPx );
		}
	}
	catch(...)
	{
	}
	delete ExcludeStrikelist;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::ApplyToColorBox( void )
{
	HeaderTextColorBox->Selected = OptionsStrikePriceView->HeaderCommodColor;
	HeaderBKColorBox->Selected = OptionsStrikePriceView->HeaderCommodBKColor;
	TextColorBox->Selected    = OptionsStrikePriceView->FieldNameCommodColor;
	BKColorBox->Selected      = OptionsStrikePriceView->FieldNameCommodBKColor;
	CallBKColorBox->Selected  = OptionsStrikePriceView->CallBKColor;
	PutBKColorBox->Selected   = OptionsStrikePriceView->PutBKColor;
	RisePxColorBox->Selected  = OptionsStrikePriceView->RisePxColor;
	FallColorBox->Selected    = OptionsStrikePriceView->FallPxColor;
	EqualPxColorBox->Selected = OptionsStrikePriceView->EqualPxColor;
	HGridColorBox->Selected   = OptionsStrikePriceView->HGridColor;
	VGridColorBox->Selected   = OptionsStrikePriceView->VGridColor;
	BKBarColorBox->Selected   = OptionsStrikePriceView->BKBarColor;
	StkPxColorBox->Selected   = OptionsStrikePriceView->StkPxColor;
	StkPxBKColorBox->Selected = OptionsStrikePriceView->StkPxBKColor;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::LoadProperties( const String& Profile )
{
	String ColumnSettings,RegKey,Fields;
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	InitTBar();
	RegKey.printf( L"%s\\TBarForm%d", Profile.c_str(), FID );
	InvertCheckBox->Checked           = g_Config.GetDesktopBool( RegKey, "Reverse", true );
	OptionsStrikePriceView->IsReverse = InvertCheckBox->Checked;
	Fields = g_Config.GetDesktopString( RegKey, "Fields", L"BuyPx^SellPx^TradePx^DiffPx^TradeQty^TotalQty^HighPx^LowPx^Delta^ImpVar^TheoryPX^OpenInterest" );
	OptionsStrikePriceView->SetFieldsString( Fields );

	OptionsStrikePriceView->HeaderCommodColor      = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderCommodColor", HeaderCommodColor[ DorL ]);
	OptionsStrikePriceView->HeaderCommodBKColor    = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderCommodBKColor", HeaderCommodBKColor[ DorL ] );
	OptionsStrikePriceView->HeaderCallColor        = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderCallColor", HeaderCallColor[ DorL ] );
	OptionsStrikePriceView->HeaderCallBKColor      = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderCallBKColor", HeaderCallBKColor[ DorL ] );
	OptionsStrikePriceView->HeaderPutColor         = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderPutColor", HeaderPutColor[ DorL ] );
	OptionsStrikePriceView->HeaderPutBKColor       = (TColor)g_Config.GetDesktopInteger( RegKey, "HeaderPutBKColor", HeaderPutBKColor[ DorL ] );
	OptionsStrikePriceView->FieldNameCallColor     = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNameCallColor", FieldNameCallColor[ DorL ] );
	OptionsStrikePriceView->FieldNameCallBKColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNameCallBKColor", FieldNameCallBKColor[ DorL ] );
	OptionsStrikePriceView->FieldNamePutColor      = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNamePutColor", FieldNamePutColor[ DorL ] );
	OptionsStrikePriceView->FieldNamePutBKColor    = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNamePutBKColor", FieldNamePutBKColor[ DorL ] );
	OptionsStrikePriceView->FieldNameCommodColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNameCommodColor", FieldNameCommodColor[ DorL ] );
	OptionsStrikePriceView->FieldNameCommodBKColor = (TColor)g_Config.GetDesktopInteger( RegKey, "FieldNameCommodBKColor", FieldNameCommodBKColor[ DorL ] );
	OptionsStrikePriceView->CallBKColor  = (TColor)g_Config.GetDesktopInteger( RegKey, "CallBKColor", CallBKColor[ DorL ] );
	OptionsStrikePriceView->PutBKColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "PutBKColor", PutBKColor[ DorL ] );
	OptionsStrikePriceView->RisePxColor  = (TColor)g_Config.GetDesktopInteger( RegKey, "RisePxColor", RisePxColor[ DorL ] );
	OptionsStrikePriceView->FallPxColor  = (TColor)g_Config.GetDesktopInteger( RegKey, "FallPxColor", FallPxColor[ DorL ] );
	OptionsStrikePriceView->EqualPxColor = (TColor)g_Config.GetDesktopInteger( RegKey, "EqualPxColor", EqualPxColor[ DorL ] );
	OptionsStrikePriceView->HGridColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "HGridColor", HGridColor[ DorL ] );
	OptionsStrikePriceView->VGridColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "VGridColor", VGridColor[ DorL ] );
	OptionsStrikePriceView->BKBarColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "BKBarColor", BKBarColor[ DorL ] );
	OptionsStrikePriceView->StkPxColor   = (TColor)g_Config.GetDesktopInteger( RegKey, "StkPxColor", StkPxColor[ DorL ] );
	OptionsStrikePriceView->StkPxBKColor = (TColor)g_Config.GetDesktopInteger( RegKey, "StkPxBKColor", StkPxBKColor[ DorL ] );

	ApplyToColorBox( );
	FFontSize                         = g_Config.GetDesktopInteger( RegKey, "FontSize", FONT_SIZE_DEF );
	OptionsSerialComboBox->ItemIndex  = g_Config.GetDesktopInteger( RegKey, "Serial", 0 );
	BKComboBox->ItemIndex             = g_Config.GetDesktopInteger( RegKey, "BKBar", 0 );
	BKComboBoxChange( NULL );
	if( OptionsSerialComboBox->ItemIndex == -1 )
		OptionsSerialComboBox->ItemIndex = 0;
	OptionsSerialComboBoxChange( NULL );
	MaturityComboBox->ItemIndex       = g_Config.GetDesktopInteger( RegKey, "Mature", 0 );
	if( MaturityComboBox->ItemIndex == -1 )
		MaturityComboBox->ItemIndex = 0;
	MaturityComboBoxChange( NULL );
	AdjuestFont();
	g_Config.GetDesktopFormPos( RegKey, "Form", RoundFormEx );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SaveProperties( const String& Profile )
{
	String RegKey;

	RegKey.printf( L"%s\\TBarForm%d", Profile.c_str(), FID );
	g_Config.SetDesktopBool(    RegKey, "Reverse", InvertCheckBox->Checked );
	g_Config.SetDesktopString( RegKey, "Fields", OptionsStrikePriceView->GetFieldsString() );
	g_Config.SetDesktopInteger( RegKey, "FontSize", FFontSize );
	g_Config.SetDesktopInteger( RegKey, "Serial", OptionsSerialComboBox->ItemIndex );
	g_Config.SetDesktopInteger( RegKey, "BKBar", BKComboBox->ItemIndex );
	g_Config.SetDesktopInteger( RegKey, "Mature", MaturityComboBox->ItemIndex );
	g_Config.SetDesktopFormPos( RegKey, "Form", RoundFormEx );

	g_Config.SetDesktopInteger( RegKey, "HeaderCommodColor", OptionsStrikePriceView->HeaderCommodColor);
	g_Config.SetDesktopInteger( RegKey, "HeaderCommodBKColor", OptionsStrikePriceView->HeaderCommodBKColor);
	g_Config.SetDesktopInteger( RegKey, "HeaderCallColor", OptionsStrikePriceView->HeaderCallColor);
	g_Config.SetDesktopInteger( RegKey, "HeaderCallBKColor", OptionsStrikePriceView->HeaderCallBKColor);
	g_Config.SetDesktopInteger( RegKey, "HeaderPutColor", OptionsStrikePriceView->HeaderPutColor);
	g_Config.SetDesktopInteger( RegKey, "HeaderPutBKColor", OptionsStrikePriceView->HeaderPutBKColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNameCallColor", OptionsStrikePriceView->FieldNameCallColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNameCallBKColor", OptionsStrikePriceView->FieldNameCallBKColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNamePutColor", OptionsStrikePriceView->FieldNamePutColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNamePutBKColor", OptionsStrikePriceView->FieldNamePutBKColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNameCommodColor", OptionsStrikePriceView->FieldNameCommodColor);
	g_Config.SetDesktopInteger( RegKey, "FieldNameCommodBKColor", OptionsStrikePriceView->FieldNameCommodBKColor);

	g_Config.SetDesktopInteger( RegKey, "CallBKColor", OptionsStrikePriceView->CallBKColor);
	g_Config.SetDesktopInteger( RegKey, "PutBKColor", OptionsStrikePriceView->PutBKColor);
	g_Config.SetDesktopInteger( RegKey, "RisePxColor", OptionsStrikePriceView->RisePxColor);
	g_Config.SetDesktopInteger( RegKey, "FallPxColor", OptionsStrikePriceView->FallPxColor);
	g_Config.SetDesktopInteger( RegKey, "EqualPxColor", OptionsStrikePriceView->EqualPxColor);
	g_Config.SetDesktopInteger( RegKey, "HGridColor", OptionsStrikePriceView->HGridColor);
	g_Config.SetDesktopInteger( RegKey, "VGridColor", OptionsStrikePriceView->VGridColor);
	g_Config.SetDesktopInteger( RegKey, "BKBarColor", OptionsStrikePriceView->BKBarColor);
	g_Config.SetDesktopInteger( RegKey, "StkPxColor", OptionsStrikePriceView->StkPxColor);
	g_Config.SetDesktopInteger( RegKey, "StkPxBKColor", OptionsStrikePriceView->StkPxBKColor);
	g_Config.Save();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::OptionsStrikePriceViewTBarStrikePxMouseClick(TObject *Sender,
		  UnicodeString Symbol, CallPutCode CallPut)
{
	if(FOpeningTBarOrderBook == true)
		return;

	FOpeningTBarOrderBook = true;
	String Exchange = FOptExchanges->Strings[ OptMarketTabSet->TabIndex ];

	FDepthFormIndex = ContractViewerForm->OpenTBarOrderBookForm( Exchange, Symbol, FDepthFormIndex );
	SendToBack();
	FOpeningTBarOrderBook = false;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::Release( void )
{
//  RemoveAllSymbols();
	SaveProperties( "TBarDef" );
}
//---------------------------------------------------------------------------
//
//  User change Exchange tab
//
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::OptMarketTabSetChange(TObject *Sender, int NewTab,
		  bool &AllowChange)
{
	if( FIsInit == true )
	{
		String       Exchange( FOptExchanges->Strings[ NewTab ] );
		String       SerialName;

		///< setup options serial combobox.
		InitSerial( NewTab, 0 );
		SerialName = FOptSerial->Strings[0];
		///< set options maturity combobox.
		SetMaturityComboBox( Exchange, SerialName );
		OptionsStrikePriceView->DisplayName = SerialName;
		MaturityComboBox->ItemIndex = 0;
		///< setup TBar control.
		OptionsStrikePriceView->Exchange  = Exchange;
		OptionsStrikePriceView->Symbol    = SerialName;
		OptionsStrikePriceView->YearMonth = MaturityComboBox->Text;
		OptionsStrikePriceView->IsReverse =	InvertCheckBox->Checked;
		UpdateOptionsStrikePriceViewPins( );
	}
	AllowChange = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	OptionsStrikePriceView->Clear( );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::BKComboBoxChange(TObject *Sender)
{
	switch( BKComboBox->ItemIndex )
	{
		case 0: OptionsStrikePriceView->BackgroundCol = obNone; break;
		case 1: OptionsStrikePriceView->BackgroundCol = obTOTAL_QTY; break;
		case 2: OptionsStrikePriceView->BackgroundCol = obOPEN_INTEREST; break;
		case 3: OptionsStrikePriceView->BackgroundCol = obIMP_VAR; break;
	}
}
//---------------------------------------------------------------------------
//
//  User setting Options pins.
//
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SettingButtonClick(TObject *Sender)
{
  if (ToolSV->Opened)
	  ToolSV->Close();
  else
	  ToolSV->Open();
}
//---------------------------------------------------------------------------
//  Load pins from config file
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::LoadPinsSetting( void )
{
	UFC::PStringList YMs;
	UFC::PDoubleList StrikePxs,SpotPxs;
	UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*> ExcludeStkPxSet;
	int  StrikePxDigit;

	///< Gat all pins
	OptionsStrikePriceView->GetStrikePxInfo( StrikePxDigit, YMs, StrikePxs, SpotPxs );
	///< Gat excloud pins
	GenExcludeSet( ExcludeStkPxSet );
    ///< Initialize pins setting UI
	if( MaturityComboBox->ItemIndex == MaturityComboBox->Items->Count -1 ) ///< All Maturity month
		Init( true, StrikePxDigit, YMs,  StrikePxs, SpotPxs, ExcludeStkPxSet );
	else
		Init( false, StrikePxDigit, YMs,  StrikePxs, SpotPxs, ExcludeStkPxSet );
	SelectPinsButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::Init( bool IsAll, int Digi, UFC::PStringList& YMs, UFC::PDoubleList& StrikePxs, UFC::PDoubleList& SpotPxs,UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& Exclude )
{
	FIsAll = IsAll;
	FStrikePXDigi = Digi;

	FYearMonth.Clear();
	FStrikePx.Clear();
	FSpotPx.Clear();
	FPList->Clear();
	for( int i =0; i < YMs.ItemCount();i++)
	{
		TListItem* NewItem = FPList->Items->Add();
		UFC::AnsiString YearMonth = YMs[i];
		double StrikePx = StrikePxs[i];
		double SpotPx   = SpotPxs[i];
		String StrikePxStr;
		UFC::PHashedSet<double>* ExSet;

		StrikePxStr.printf( L"%.*f",FStrikePXDigi, StrikePx );
		NewItem->SubItems->Add( YearMonth.c_str() );
		NewItem->SubItems->Add( StrikePxStr  );
		if( Exclude.GetObjectByKey( YearMonth, ExSet ) == true && ExSet->Exists(StrikePx) == true )
			NewItem->Checked = false;
		else
			NewItem->Checked = true;
		FYearMonth.Add( YearMonth );
		FStrikePx.Add( StrikePx );
		FSpotPx.Add( SpotPx );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SavePinsSetting( void )
{
	UFC::PHashMap<UFC::AnsiString, TStringList*> ExcludeStkPxList;
	String RegKey;

	if( MaturityComboBox->ItemIndex == MaturityComboBox->Items->Count -1 ) ///< All Maturity month
		RegKey.printf( L"Options\\TBarForm\\Exclude\\%s_%s_all", OptionsStrikePriceView->Exchange, OptionsStrikePriceView->Symbol );
	else
		RegKey.printf( L"Options\\TBarForm\\Exclude\\%s_%s", OptionsStrikePriceView->Exchange, OptionsStrikePriceView->Symbol );
	for( int i =0; i < FYearMonth.ItemCount();i++)
	{
		TListItem*   CheckItem = FPList->Items->Item[i];

		if(	CheckItem->Checked == false )
		{
			UFC::AnsiString YearMonth = FYearMonth[i];
			TStringList*    ExcludeListMY;
			String          StrikePxStr;

			StrikePxStr.printf( L"%.*f", FStrikePXDigi, FStrikePx[i] );
			if( ExcludeStkPxList.GetObjectByKey( YearMonth, ExcludeListMY ) == false)
			{
				ExcludeListMY = new TStringList();
				ExcludeStkPxList.Add( YearMonth, ExcludeListMY );
			}
			ExcludeListMY->Add( StrikePxStr );
		}
	}
	for( int i = 0; i<  ExcludeStkPxList.ItemCount(); i++ )
	{
		UFC::AnsiString MY;
		TStringList*    ExcludeListMY;

		if( ExcludeStkPxList.GetItem( i, MY, ExcludeListMY ) == true )
		{
			ExcludeListMY->Delimiter = ',';
			g_Config.SetDesktopString( RegKey, MY.c_str(), ExcludeListMY->DelimitedText );
			delete ExcludeListMY;
		}
	}
	ExcludeStkPxList.Clear();
}
//---------------------------------------------------------------------------
//  Set the exclude pins set.
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::GenExcludeSet( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxList )
{
	AnsiString AnsiYearMonth;
	String RegKey,Fields;

	UFC::PHashedSet<double>* StrikePxSet  = new UFC::PHashedSet<double>();

	RegKey.printf( L"Options\\TBarForm\\Exclude\\%s_%s", OptionsStrikePriceView->Exchange, OptionsStrikePriceView->Symbol );
	AnsiYearMonth = OptionsStrikePriceView->YearMonth;
	Fields = g_Config.GetDesktopString( RegKey, OptionsStrikePriceView->YearMonth, L"" );
	GenStrikePxSet( StrikePxSet, Fields );

	ExcludeStkPxList.Add( AnsiYearMonth.c_str(), StrikePxSet );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::ClearExcludeSet( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxList )
{
	UFC::PHashedSet<double>* DelSet = ExcludeStkPxList.First();

	while( DelSet != NULL )
	{
		delete DelSet;
		DelSet = ExcludeStkPxList.Next();
	};
}
//---------------------------------------------------------------------------
//  Update selected pins to OptionsStrikePriceView
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::UpdateOptionsStrikePriceViewPins( void )
{
	int TBarWidth;
	UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*> ExcludeStkPxSet;

	GenExcludeSet( ExcludeStkPxSet );
	OptionsStrikePriceView->Locked = true;
	try
	{
		OptionsStrikePriceView->UpdateTable( ExcludeStkPxSet );
	}
	__finally
	{
		OptionsStrikePriceView->Locked = false;
	}
	ClearExcludeSet( ExcludeStkPxSet );

	TBarWidth = OptionsStrikePriceView->BestFitWidth + RoundFormEx->LeftGraph->Width/2 +  RoundFormEx->RightGraph->Width/2 + 18;
	Constraints->MaxWidth = TBarWidth;
	Constraints->MinWidth = TBarWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SelectPinsButtonClick(TObject *Sender)
{
	double Percent = PercentComboBox->Text.ToDouble() / 100.0;

	for( int i =0; i < FYearMonth.ItemCount();i++)
	{
		TListItem* CheckItem = FPList->Items->Item[i];
		double StrikePx = FStrikePx[i];
		double SpotPx   = FSpotPx[i];

		double Range = SpotPx* Percent;
		if( StrikePx <= (SpotPx + Range )&& StrikePx >= (SpotPx - Range ) )
			CheckItem->Checked = true;
		else
			CheckItem->Checked = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::ApplyButtonClick(TObject *Sender)
{
	MaturityComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::CancelButtonClick(TObject *Sender)
{
	LoadPinsSetting();
}
//---------------------------------------------------------------------------

void __fastcall TOptionsTBarForm::FPListDrawItem(TCustomListView *Sender, TListItem *Item,
          TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw = true;
	TRect            DrawRect;

	CDState << cdsSelected;
	Sender->Canvas->Brush->Style = bsClear;
	if( Item->Selected == true )
	{
		DrawGradientBar( Sender->Canvas , Rect, clGray, clBlack );
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		int Mon =  Item->SubItems->Strings[ 0 ].ToInt();
		if( Mon % 2 == 0 )
			DrawGradientBar( Sender->Canvas , Rect, clWhite, clWhite );
		else
			DrawGradientBar( Sender->Canvas , Rect, clGray, clGray );
		//SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
	}
	if( Item->Checked == false  )
		ImageList->Draw( Sender->Canvas, Rect.left, Rect.top, 0, true );
	else
		ImageList->Draw( Sender->Canvas, Rect.left, Rect.top, 1, true );
	Sender->Canvas->Pen->Color   = clSilver;
	Sender->Canvas->MoveTo( Rect.left + 29 , Rect.top );
	Sender->Canvas->LineTo( Rect.left + 29 , Rect.bottom );
	Rect.Left += 31;
	for( int i = 1; i <= Item->SubItems->Count; i ++ )
		 FPListCustomDrawSubItem( Sender, Item ,i, CDState, DefaultDraw );
}
//---------------------------------------------------------------------------

void __fastcall TOptionsTBarForm::FPListCustomDrawSubItem(TCustomListView *Sender,
          TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)

{
	TRect       ItemRect;
	TTextFormat Formats;
	String      DrawStr = Item->SubItems->Strings[ SubItem -1 ];

	if( ListView_GetSubItemRect( Sender->Handle, Item->Index, SubItem, LVIR_BOUNDS, &ItemRect ) )
	{
		Sender->Canvas->Brush->Style = bsClear;
		Sender->Canvas->Pen->Color   = clGray;//Silver;

		if( Item->Selected == true )
		{
			DrawGradientBar( Sender->Canvas , ItemRect, clGray, clBlack );
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
		}
		else
		{
			int Mon =  Item->SubItems->Strings[ 0 ].ToInt();
			if( Mon % 2 == 0 )
			{
				DrawGradientBar( Sender->Canvas , ItemRect, clWhite, clWhite );
				SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
			}
			else
			{
				DrawGradientBar( Sender->Canvas , ItemRect, clGray, clGray );
				SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
			}
		}
		Sender->Canvas->MoveTo( ItemRect.left, ItemRect.top );
		Sender->Canvas->LineTo( ItemRect.left, ItemRect.bottom );
		ItemRect.right -= 2;
		if( SubItem == 1 )
			Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
		else
			Formats <<tfSingleLine<<tfRight<<tfVerticalCenter;
		Sender->Canvas->TextRect( ItemRect, DrawStr, Formats );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::FPListMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TListItem* ClickItem = FPList->GetItemAt( X, Y );
	if( ClickItem != NULL )
	{
		if( X <= 30 )
			ClickItem->Checked = !ClickItem->Checked;
	}
}
//---------------------------------------------------------------------------

void __fastcall TOptionsTBarForm::HeaderTextColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->HeaderCommodColor = HeaderTextColorBox->Selected;
	OptionsStrikePriceView->HeaderCallColor   = HeaderTextColorBox->Selected;
	OptionsStrikePriceView->HeaderPutColor    = HeaderTextColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TOptionsTBarForm::HeaderBKColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->HeaderCommodBKColor = HeaderBKColorBox->Selected;
	OptionsStrikePriceView->HeaderCallBKColor   = HeaderBKColorBox->Selected;
	OptionsStrikePriceView->HeaderPutBKColor    = HeaderBKColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::TextColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->FieldNameCommodColor = TextColorBox->Selected;
	OptionsStrikePriceView->FieldNameCallColor   = TextColorBox->Selected;
	OptionsStrikePriceView->FieldNamePutColor    = TextColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::BKColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->FieldNameCommodBKColor = BKColorBox->Selected;
	OptionsStrikePriceView->FieldNameCallBKColor   = BKColorBox->Selected;
	OptionsStrikePriceView->FieldNamePutBKColor    = BKColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::CallBKColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->CallBKColor = CallBKColorBox->Selected;
	OptionsStrikePriceView->CalSize();
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::PutBKColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->PutBKColor = PutBKColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::RisePxColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->RisePxColor = RisePxColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::FallColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->FallPxColor = FallColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::EqualPxColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->EqualPxColor = EqualPxColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::HGridColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->HGridColor = HGridColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::VGridColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->VGridColor = VGridColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::BKBarColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->BKBarColor = BKBarColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::StkPxColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->StkPxColor = StkPxColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::StkPxBKColorBoxChange(TObject *Sender)
{
	OptionsStrikePriceView->StkPxBKColor = StkPxBKColorBox->Selected;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::DefColorButtonClick(TObject *Sender)
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	OptionsStrikePriceView->HeaderCommodColor      = HeaderCommodColor[DorL];
	OptionsStrikePriceView->HeaderCallColor        = HeaderCallColor[DorL];
	OptionsStrikePriceView->HeaderPutColor         = HeaderPutColor[DorL];
	OptionsStrikePriceView->HeaderCommodBKColor    = HeaderCommodBKColor[DorL];
	OptionsStrikePriceView->HeaderCallBKColor      = HeaderCallBKColor[DorL];
	OptionsStrikePriceView->HeaderPutBKColor       = HeaderPutBKColor[DorL];
	OptionsStrikePriceView->FieldNameCommodColor   = FieldNameCommodColor[DorL];
	OptionsStrikePriceView->FieldNameCallColor     = FieldNameCallColor[DorL];
	OptionsStrikePriceView->FieldNamePutColor      = FieldNamePutColor[DorL];
	OptionsStrikePriceView->FieldNameCallBKColor   = FieldNameCallBKColor[DorL];
	OptionsStrikePriceView->FieldNamePutBKColor    = FieldNamePutBKColor[DorL];
	OptionsStrikePriceView->FieldNameCommodBKColor = FieldNameCommodBKColor[DorL];
	OptionsStrikePriceView->StkPxColor   =  StkPxColor[DorL];
	OptionsStrikePriceView->StkPxBKColor =  StkPxBKColor[DorL];
	OptionsStrikePriceView->CallBKColor  =  CallBKColor[DorL];
	OptionsStrikePriceView->PutBKColor   =  PutBKColor[DorL];
	OptionsStrikePriceView->RisePxColor  =  RisePxColor[DorL];
	OptionsStrikePriceView->FallPxColor  =  FallPxColor[DorL];
	OptionsStrikePriceView->EqualPxColor =  EqualPxColor[DorL];
	OptionsStrikePriceView->HGridColor   =  HGridColor[DorL];
	OptionsStrikePriceView->VGridColor   =  VGridColor[DorL];
	OptionsStrikePriceView->BKBarColor   =  BKBarColor[DorL];
	ApplyToColorBox();
	OptionsStrikePriceView->CalSize();
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::OptionsStrikePriceViewTBarFieldsChanged(TObject *Sender)
{
	int TBarWidth;

	TBarWidth = OptionsStrikePriceView->BestFitWidth + RoundFormEx->LeftGraph->Width/2 +  RoundFormEx->RightGraph->Width/2 + 18;
	Constraints->MaxWidth = TBarWidth;
	Constraints->MinWidth = TBarWidth;
}
//---------------------------------------------------------------------------

void __fastcall TOptionsTBarForm::InvertLabelClick(TObject *Sender)
{
	InvertCheckBox->Checked = !InvertCheckBox->Checked;
	OptionsStrikePriceView->IsReverse =	InvertCheckBox->Checked;
	OptionsStrikePriceView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsTBarForm::SetVisible( bool IsVisible )
{
	this->Visible = IsVisible;
}

//---------------------------------------------------------------------------


