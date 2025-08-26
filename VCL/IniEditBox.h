//---------------------------------------------------------------------------

#ifndef IniEditBoxH
#define IniEditBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
//---------------------------------------------------------------------------
class TiniObject : public TObject
{
protected:
    AnsiString FName;
	bool       FDirty;
	bool       FIsSection;
public:
	__fastcall TiniObject( const AnsiString& Name, bool IsSec )
	:FName(Name)
	,FDirty( false )
	,FIsSection( IsSec ){}
	const AnsiString& __fastcall GetName( void )  { return FName; }
	bool              __fastcall IsDirty( void )  { return FDirty; }
	bool              __fastcall IsSection( void ){ return FIsSection; }
};
//---------------------------------------------------------------------------
class TSectionObject : public TiniObject
{
public:
	__fastcall TSectionObject( const AnsiString& Name )
	:TiniObject( Name, true ){}
	void __fastcall SetName( const AnsiString& Name ) { FName = Name; FDirty = true; }
};
//---------------------------------------------------------------------------
class TNameValueObject : public TiniObject
{
private:
	AnsiString FValue;
public:
	__fastcall TNameValueObject( const AnsiString& Name, const AnsiString& Value )
	:TiniObject(Name, false )
	,FValue( Value ){}
    const AnsiString& __fastcall GetValue( void ) { return FValue; }
    void              __fastcall SetValue( const AnsiString& Value ) { FValue = Value; FDirty = true; }
};
//---------------------------------------------------------------------------
class PACKAGE TIniEditBox : public TCustomListBox
{
private:
    UFC::AnsiString FFileName;
    TEdit*          FEdit;
	bool            FEditable;
	TPopupMenu*     FMenu;
	int             FSelectIndex;
	int             FInitItemCount;
protected:
	void __fastcall DrawItem(int Index, const Types::TRect &Rect, TOwnerDrawState State);
	void __fastcall UpdateValue( int Index, AnsiString Value );
	void __fastcall SetEditable( bool Editable );
	void __fastcall ClearItems( void );
	void __fastcall EditSection( int Index, const Types::TRect &Rect, const AnsiString& Name );
	int __fastcall SectionList( TStringList* List );
protected:
	DYNAMIC void __fastcall Resize( void );
	DYNAMIC void __fastcall MouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y);
	void __fastcall OnNewSection( TObject* Sender );
	void __fastcall OnNewNameValue( TObject* Sender );
	void __fastcall OnDelete( TObject* Sender );
public:
    __fastcall TIniEditBox( TComponent* Owner );
    __fastcall ~TIniEditBox( void ); 
	void __fastcall Load( const AnsiString& FileName );
	void __fastcall Load( UFC::UiniFile& IniFile );
	void __fastcall Save( const AnsiString& FileName );
	void __fastcall Save( UFC::UiniFile& IniFile );
	void __fastcall Save( void );
	bool __fastcall Modified( void );
__published:
    __property bool SectionEditable = {read = FEditable, write = SetEditable };
    __property Align;
    __property Font;        
};
//---------------------------------------------------------------------------
#endif
