// -------------------------------------------------------------------------
// Implementation of TStarWaveActiveXImpl (CoClass: StarWaveActiveX, Interface: IStarWaveActiveX)
// -------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <ComObj.hpp>
#include <ActiveX.hpp>
#include <axbase.h>
#include "StarWaveActiveXImpl.h"
#pragma package(smart_init)
// ---------------------------------------------------------------------------------
// TStarWaveActiveX - Class Factory
// ---------------------------------------------------------------------------------
static void createFactory()
{
  new TCppActiveXControlFactory<TStarWaveActiveXImpl, TCustomControl>
							   (Comserv::GetComServer(),
								__classid(TStarWaveActiveXImpl),
								__classid(TCustomControl),
								CLSID_StarWaveActiveX,
								0,
								"",
								0,
								tmApartment);
}
#pragma startup createFactory 32
// ---------------------------------------------------------------------------------
//
// Class TStarWaveActiveXImpl
//
// ---------------------------------------------------------------------------------
__fastcall TStarWaveActiveXImpl::TStarWaveActiveXImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller)
:_COM_CLASS(Factory, Controller)
,FConnection( NULL )
,FAppName( "" )
{
	FLogonTimer = new TTimer( NULL );
	FLogonTimer->Interval = 5000;
	FLogonTimer->OnTimer = OnLogonTimer;
	FLogonTimer->Enabled = false;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::DisposeOf()
{
  try
  {
	m_VclCtl->DisposeOf();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::DrawTextBiDiModeFlagsReadingOnly(long* Value)
{
  try
  {
    *Value = m_VclCtl->DrawTextBiDiModeFlagsReadingOnly();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_AlignDisabled(VARIANT_BOOL* Value)
{
  try
  {
    *Value = m_VclCtl->AlignDisabled;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_AlignWithMargins(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->AlignWithMargins;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_DockSite(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->DockSite;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_DoubleBuffered(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->DoubleBuffered;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_Enabled(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->Enabled;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------

STDMETHODIMP TStarWaveActiveXImpl::get_ExplicitHeight(long* Value)
{
  try
  {
	*Value = (long)(m_VclCtl->ExplicitHeight);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_ExplicitLeft(long* Value)
{
  try
  {
	*Value = (long)(m_VclCtl->ExplicitLeft);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_ExplicitTop(long* Value)
{
  try
  {
	*Value = (long)(m_VclCtl->ExplicitTop);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_ExplicitWidth(long* Value)
{
  try
  {
	*Value = (long)(m_VclCtl->ExplicitWidth);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_MouseInClient(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->MouseInClient;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------

STDMETHODIMP TStarWaveActiveXImpl::get_ParentCustomHint(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->ParentCustomHint;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------

STDMETHODIMP TStarWaveActiveXImpl::get_ParentDoubleBuffered(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->ParentDoubleBuffered;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_UseDockManager(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->UseDockManager;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_Visible(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->Visible;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_VisibleDockClientCount(long* Value)
{
  try
  {
	*Value = (long)(m_VclCtl->VisibleDockClientCount);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::InitiateAction()
{
  try
  {
	m_VclCtl->InitiateAction();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::IsRightToLeft(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->IsRightToLeft();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::QualifiedClassName(BSTR* Value)
{
  try
  {
	*Value = System::WideString(m_VclCtl->QualifiedClassName()).Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_AlignWithMargins(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 219;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->AlignWithMargins = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_DockSite(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 201;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->DockSite = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_DoubleBuffered(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 202;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->DoubleBuffered = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_Enabled(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = -514;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->Enabled = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_ParentCustomHint(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 220;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->ParentCustomHint = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_ParentDoubleBuffered(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 207;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->ParentDoubleBuffered = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_UseDockManager(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 208;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->UseDockManager = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_Visible(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 218;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->Visible = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SetDesignVisible(VARIANT_BOOL Value)
{
  try
  {
	m_VclCtl->SetDesignVisible(Value);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SetSubComponent(VARIANT_BOOL IsSubComponent)
{
  try
  {
	m_VclCtl->SetSubComponent(IsSubComponent);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UnitScope(BSTR* Value)
{
  try
  {
	*Value = System::WideString(m_VclCtl->UnitScope()).Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UseRightToLeftReading(VARIANT_BOOL* Value)
{
  try
  {
	*Value = m_VclCtl->UseRightToLeftReading();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UseRightToLeftScrollBar(VARIANT_BOOL* Value)
{
  try
  {
    *Value = m_VclCtl->UseRightToLeftScrollBar();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Connect(BSTR IP, long Port, long Timeout)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString IPStr( IP );

			FConnection->SetIP( IPStr.c_str() );
			FConnection->SetPort( Port );
			FConnection->Connect( IPStr.c_str(), Port, Timeout );
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Disconnect()
{
	try
	{
		if( FConnection != NULL )
			FConnection->Disconnect();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::DownloadContract()
{
	try
	{
		FConnection->Clear();
		if( FConnection != NULL )
			FConnection->DownloadContract();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_AppName(BSTR* Value)
{
	try
	{
		WideString AppStr( FAppName.c_str() );
		*Value = AppStr.Copy();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_IP(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString IP( Value );

		FConnection->SetIP( IP.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::set_Port(long Value)
{
	FConnection->SetPort( Value );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_IP(BSTR* Value)
{
	try
	{
		if( FConnection != NULL )
		{
			WideString IPStr( FConnection->GetIP().c_str() );
			*Value = IPStr.Copy();
		}
		else
		{
			WideString IPStr( "127.0.0.1" );
			*Value = IPStr.Copy();
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_Port(long* Value)
{
	if( FConnection != NULL )
		*Value = FConnection->GetPort();
	else
		*Value = DEFAULT_PORT;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::GetExchange(long Index, BSTR* Exchange)
{
	try
	{
		if( FConnection != NULL )
		{
			if( Index >= 0 && Index < FConnection->ExchangeCount() )
			{
				WideString ExStr( FConnection->GetExchange( Index ).c_str() );
				*Exchange = ExStr.Copy();
			}
			else
				*Exchange = NULL;
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::GetSecurityDefinition(BSTR Exchange, BSTR Symbol,
          ISecurityDefinition* SecurityDefinition, VARIANT_BOOL* IsExist)
{
  try
  {
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			AnsiString SymStr( Symbol );
			BasicInformation* Info = FConnection->GetBasicInformation( ExStr.c_str(), SymStr.c_str() );

			if( Info != NULL )
			{
				TSecurityDefinitionImpl* DefImpl = new TSecurityDefinitionImpl( );

				DefImpl->FromBasicInfo(Info);
				SecurityDefinition->Copy( interface_cast<ISecurityDefinition>(DefImpl) );
				*IsExist = VARIANT_TRUE;
			}
			else
			{
				*IsExist = VARIANT_FALSE;
			}
		}
  }
  catch(Exception &e)
  {
	AnsiString ExMsg( e.Message.c_str() );
	UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Init()
{
  try
  {
	if( FConnection == NULL )
	{

		FAppName.Printf( "%s.%d", UFC::Hostname, UFC::GetTickCountMS() );
		UFC::BufferedLog::Printf( " Init App[%s]",FAppName.c_str() );
		FConnection = new MDAXConnection( this, FAppName );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType,
          VARIANT_BOOL* Value)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		AnsiString SymStr( Symbol );

		if( FConnection->Subscribe( ExStr.c_str(), SymStr.c_str(),SubscribeType ) == true )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Unsubscribe(BSTR Exchange, BSTR Symbol)
{
  try
  {
	  if( FConnection != NULL )
	  {
		AnsiString ExStr( Exchange );
		AnsiString SymStr( Symbol );
		FConnection->Unsubscribe( ExStr.c_str(), SymStr.c_str() );
	  }
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Destroy()
{
  try
  {
	if( FConnection != NULL )
	{
		FConnection->UnsubscribeAll();
		FConnection->Clear();
		delete FConnection;
		FConnection = NULL;
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SymbolCount(BSTR Exchange, long* Count)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		*Count = FConnection->SymbolCount( ExStr.c_str() );
	}
	else
		*Count = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::GetSymbol(BSTR Exchange, long Index, BSTR* Symbol)















{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		UFC::AnsiString SymStr = FConnection->GetSymbol( ExStr.c_str(), Index );
		WideString Result( SymStr.c_str() );
		*Symbol = Result.Copy();
	}
	else
		*Symbol = NULL;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UnsubscribeAll()
{
	try
	{
		if( FConnection != NULL )
		{
			FConnection->UnsubscribeAll();
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::GetSecurityDefinitionByIndex(BSTR Exchange, long Index,
          ISecurityDefinition* SecurityDefinition, VARIANT_BOOL* IsExist)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			BasicInformation* Info = FConnection->GetBasicInformation( ExStr.c_str(), Index );

			if( Info != NULL )
			{
				TSecurityDefinitionImpl* DefImpl = new TSecurityDefinitionImpl( );

				DefImpl->FromBasicInfo(Info);
				SecurityDefinition->Copy( interface_cast<ISecurityDefinition>(DefImpl) );
				*IsExist = VARIANT_TRUE;
			}
			else
			{
				*IsExist = VARIANT_FALSE;
			}
		}
	}
	catch(Exception &e)
	{
		AnsiString ExMsg( e.Message.c_str() );
		UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->SubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UnsubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->UnsubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SendNews(BSTR Group, BSTR Message)
{
	AnsiString GroupStr( Group );
	AnsiString MsgStr( Message );

	if( FConnection != NULL )
		FConnection->SendNews( GroupStr.c_str(), MsgStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->SubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UnsubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->UnsubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Recover(BSTR Exchange)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			FConnection->Recover( ExStr.c_str() );
		}
	}
	catch(Exception &e)
	{
		AnsiString ExMsg( e.Message.c_str() );
		UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
		return Error(e.Message.c_str(), IID_IStarWaveActiveX);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth,
          EnumCallPut CallPutCode, double StrikePx, EnumSubscribeType SubscribeType)

{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		AnsiString ProductStr( ProductID );
		AnsiString SettleMonthStr( SettlementMonth );

		FConnection->SubscribeX( ExStr.c_str(), ProductStr.c_str(), SettleMonthStr.c_str(),
			CallPutCode, StrikePx, SubscribeType );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
int __fastcall TStarWaveActiveXImpl::GetVersion( void )
{
	int   COMVersion = 20000;
	char  Binary[MAX_PATH];
	DWORD n;

	if( GetModuleFileNameA( HInstance, Binary, MAX_PATH ) > 0 )
	{
		if((n = GetFileVersionInfoSizeA( Binary, NULL ) ) > 0 )
		{
			VS_FIXEDFILEINFO* vsinfo;
			char*             pBuf = new char[ n ];
			UINT              Len;

			GetFileVersionInfoA( Binary, 0, n, pBuf );
			if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
			{
				AnsiString  VersionString;
				int Version  = HIWORD(vsinfo->dwFileVersionMS);
				int MVersion = LOWORD(vsinfo->dwFileVersionMS);
				int Release  = HIWORD(vsinfo->dwFileVersionLS);
				int Build    = LOWORD(vsinfo->dwFileVersionLS);

				COMVersion = Version*10000 + (MVersion%100)*100 + Build%100;
			}
			delete [] pBuf;
		}
	}
	return COMVersion;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SetDebugLog(BSTR FileName)
{
	if( FConnection != NULL )
	{
		AnsiString LogFileName( FileName );

		FConnection->SetLogFile( LogFileName.c_str() );
		UFC::BufferedLog::Printf( " -------------------------------------------------" );
		UFC::BufferedLog::Printf( "    StarWave API, Win%d OCX Version           ", sizeof(void*)*8 );
		UFC::BufferedLog::Printf( "    Copyright 2010-%d by MDBS Software Inc.     ", UFC::ThisYear() );
		UFC::BufferedLog::Printf( "    All right reserved.                           " );
		UFC::BufferedLog::Printf( "                                                  " );
		UFC::BufferedLog::Printf( "    StarWaveActiveX Version[%d]                   ", GetVersion( ) );
		UFC::BufferedLog::Printf( "    Build date[%s] ", __DATE__ );
		UFC::BufferedLog::Printf( " -------------------------------------------------"  );
		UFC::BufferedLog::FlushToFile();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_BuildDate(BSTR* Value)
{
	AnsiString DateStr( __DATE__ );
	WideString wStr( DateStr );

	*Value = wStr.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::get_Version(BSTR* Value)
{
	WideString VerStr;
	int        COMVer = GetVersion( );

	VerStr.printf( L"%2d.%02d.%02d %d-bit",COMVer/10000,(COMVer%10000)/100,COMVer%100,sizeof(void*)*8);
	*Value = VerStr.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::Logon(BSTR ID, BSTR Password, BSTR* Msg, VARIANT_BOOL* Value)





{
  try
  {
	 AnsiString AnsiID( ID );
	 AnsiString AnsiPWD( Password );
	 UFC::AnsiString logonMsg,Version;
	 int             COMVer = GetVersion( );

	 Version.Printf( "%2d.%02d.%02d %d-bit",COMVer/10000,(COMVer%10000)/100,COMVer%100,sizeof(void*)*8);

	 bool Result = FConnection->Logon( AnsiID.c_str(), AnsiPWD.c_str(), Version, logonMsg );
	 if( Result == true )
	 {
		 *Value = VARIANT_TRUE;
		 FLogonTimer->Enabled = true;
	 }
	 else
		 *Value = VARIANT_FALSE;

	 WideString ErrStr( logonMsg.c_str() );
	 *Msg = ErrStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveActiveX);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
void __fastcall TStarWaveActiveXImpl::OnLogonTimer(TObject*Sender )
{
	FLogonTimer->Enabled = false;
	if( FConnection->IsResponse() == false )
	{
		WideString MsgStr( L"Logon timeout!" );

		Fire_OnLogonReply( VARIANT_FALSE, MsgStr.c_bstr() );
		printf( "OnLoginReply[%d:%s]\n", 0, "Logon timeout!" );
		FConnection->Disconnect();
	}
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::SubscribeExchangeNews(BSTR Exchange)
{
	AnsiString AnsiEx( Exchange );

	FConnection->SubscribeExchangeNews( AnsiEx.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveActiveXImpl::UnsubscribeExchangeNews(BSTR Exchange)
{
	AnsiString AnsiEx( Exchange );

	FConnection->UnsubscribeExchangeNews( AnsiEx.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------


