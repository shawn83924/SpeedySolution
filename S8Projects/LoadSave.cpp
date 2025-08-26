//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "UnifyUtility.h"

//------------------------------------------------------------------------------
bool Config::DeleteProperty( const String& Group, const String& Name )
{
	AnsiString    SectionName( Group );
	AnsiString    TagName( Name );
	UFC::Section* Sec = m_ini->GetSection( SectionName.c_str() );

	if( Sec != NULL )
	{
		Sec->DeleteValue( TagName.c_str() );
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
// Save settings
//------------------------------------------------------------------------------
int Config::GetIntegerProperty( const String& Group, const String& Name, int Default )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString Value;

	if( m_ini->GetValue( SectionName.c_str(), TagName.c_str(), Value  ) == true )
		return Value.ToInt();
	return Default;
}
//------------------------------------------------------------------------------
void Config::SetIntegerProperty( const String& Group, const String& Name, int Value  )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString IntValue( Value );

	m_ini->SetValue( SectionName.c_str(), TagName.c_str(), IntValue  );
}
//------------------------------------------------------------------------------
double Config::GetDoubleProperty( const String& Group, const String& Name, double Default )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString Value;

	if( m_ini->GetValue( SectionName.c_str(), TagName.c_str(), Value  ) == true )
		return Value.ToDouble();
	return Default;
}
//------------------------------------------------------------------------------
void Config::SetDoubleProperty( const String& Group, const String& Name, double Value  )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString DoubleValue;

	DoubleValue.Printf( "%f", Value );
	m_ini->SetValue( SectionName.c_str(), TagName.c_str(), DoubleValue  );
}
//------------------------------------------------------------------------------
bool Config::GetBoolProperty( const String& Group, const String& Name, bool Default )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString Value;

	if( m_ini->GetValue( SectionName.c_str(), TagName.c_str(), Value  ) == true )
		return (bool)Value.ToInt();
	return Default;
}
//------------------------------------------------------------------------------
void Config::SetBoolProperty( const String& Group, const String& Name, bool Value  )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString IntValue( (int)Value );

	m_ini->SetValue( SectionName.c_str(), TagName.c_str(), IntValue  );
}
//------------------------------------------------------------------------------
String Config::GetStringProperty( const String& Group, const String& Name, String Default )
{
	String     Result;
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString Value;

	if( m_ini->GetValue( SectionName.c_str(), TagName.c_str(), Value  ) == true )
	{
		Result = Value.c_str();
		return Result;
	}
	return Default;
}
//------------------------------------------------------------------------------
void Config::SetStringProperty( const String& Group, const String& Name, String Value  )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	AnsiString StrValue( Value );

	m_ini->SetValue( SectionName.c_str(), TagName.c_str(), StrValue.c_str() );
}
//------------------------------------------------------------------------------
String Config::GetBase64StringProperty( const String& Group, const String& Name, String Default )
{
	String     Result;
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	UFC::AnsiString Value;

	if( m_ini->GetValue( SectionName.c_str(), TagName.c_str(), Value  ) == true )
	{
		Result = Value.c_str();
		return Base64Decode( Result );
	}
	return Default;
}
//------------------------------------------------------------------------------
void Config::SetBase64StringProperty( const String& Group, const String& Name, String Value  )
{
	AnsiString SectionName( Group );
	AnsiString TagName( Name );
	AnsiString StrValue( Base64Encode( Value ) );

	m_ini->SetValue( SectionName.c_str(), TagName.c_str(), StrValue.c_str() );
}
//------------------------------------------------------------------------------
bool Config::GetDesktopBool( const String& Group, const String& Name, bool Default )
{
	String ActGroup = "Desktop\\" + Group;
	return GetBoolProperty( ActGroup,Name, Default );
}
//------------------------------------------------------------------------------
void Config::SetDesktopBool( const String& Group, const String& Name, bool Value  )
{
	String ActGroup = "Desktop\\" + Group;
	SetBoolProperty( ActGroup, Name, Value  );
}
//------------------------------------------------------------------------------
String Config::GetDesktopString( const String& Group, const String& Name, String Default )
{
	String ActGroup = "Desktop\\" + Group;
	return GetStringProperty( ActGroup,Name, Default );
}
//------------------------------------------------------------------------------
void Config::SetDesktopString( const String& Group, const String& Name, String Value  )
{
	String ActGroup = "Desktop\\" + Group;
    SetStringProperty( ActGroup, Name, Value  );
}
//------------------------------------------------------------------------------
int Config::GetDesktopInteger( const String& Group, const String& Name, int Default )
{
	String ActGroup = "Desktop\\" + Group;
	return GetIntegerProperty( ActGroup,Name, Default );
}
//------------------------------------------------------------------------------
void Config::SetDesktopInteger( const String& Group, const String& Name, int Value  )
{
	String ActGroup = "Desktop\\" + Group;
	SetIntegerProperty( ActGroup, Name, Value  );
}
//------------------------------------------------------------------------------
double Config::GetDesktopDouble( const String& Group, const String& Name, double Default )
{
	String ActGroup = "Desktop\\" + Group;
	return GetDoubleProperty( ActGroup,Name, Default );
}
//------------------------------------------------------------------------------
void Config::SetDesktopDouble( const String& Group, const String& Name, double Value  )
{
	String ActGroup = "Desktop\\" + Group;
	SetDoubleProperty( ActGroup, Name, Value  );
}
//------------------------------------------------------------------------------
void Config::GetDesktopFormPos( const String& Group, const String& Name, TRoundFormEx* Obj, bool SetVisbale, int X, int Y, int W, int H  )
{
   String GroupName = Group+"\\"+Name;
   bool FormLock    = GetDesktopBool( GroupName, "Lock",   false );
   bool FormVisbale = GetDesktopBool( GroupName, "Visable", true );
   int  FormLeft    = GetDesktopInteger( GroupName, "Left",   X );
   int  FormTop     = GetDesktopInteger( GroupName, "Top",    Y );
   int  FormWidth   = GetDesktopInteger( GroupName, "Width",  W );
   int  FormHeight  = GetDesktopInteger( GroupName, "Height", H );
   int  HalfH,HalfW;

   ///< Windows higher then working area.
   if( FormHeight > Screen->WorkAreaHeight )
	   FormHeight = Screen->WorkAreaHeight;
   ///< Windows Wider than working area.
   if( FormWidth > Screen->WorkAreaWidth )
	   FormWidth = Screen->WorkAreaWidth;
   ///< Makesure Windows Rect is in working area.
   HalfH = FormHeight/2;
   HalfW = FormWidth/2;
   if( FormLeft + HalfW > Screen->DesktopWidth )
	   FormLeft = Screen->DesktopWidth - HalfW;
   else if( FormLeft < -1*HalfW )
	   FormLeft = -1*HalfW;
   if( FormTop + HalfH > Screen->DesktopHeight )
	   FormTop = Screen->DesktopHeight - HalfH;
   else if( FormTop < 0 )
	   FormTop = 0;
    ///< Visible and lock
   Obj->ParentForm->Visible = false;
   Obj->Lock = FormLock;
   Obj->ParentForm->SetBounds( FormLeft, FormTop, FormWidth, FormHeight );
   if( FormLock == true )
   {
	   Obj->ParentForm->Visible = true;
	   Obj->ParentForm->SetBounds( FormLeft, FormTop, FormWidth, FormHeight );
   }
}
//------------------------------------------------------------------------------
void Config::SetDesktopFormPos( const String& Group, const String& Name, TRoundFormEx* Obj )
{
   String GroupName = Group+"\\"+Name;

   SetDesktopBool(    GroupName, "Lock",   Obj->Lock );
   SetDesktopBool(    GroupName, "Visable",Obj->ParentForm->Visible );
   SetDesktopInteger( GroupName, "Left",   Obj->ParentForm->Left );
   SetDesktopInteger( GroupName, "Top",    Obj->ParentForm->Top );
   SetDesktopInteger( GroupName, "Width",  Obj->ParentForm->Width );
   SetDesktopInteger( GroupName, "Height", Obj->ParentForm->Height );
}
//------------------------------------------------------------------------------
