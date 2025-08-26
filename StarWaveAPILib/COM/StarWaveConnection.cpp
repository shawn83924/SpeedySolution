// 1.1
// UNIT1 : Implementation of TStarWaveConnectionImpl (CoClass: StarWaveConnection, Interface: IStarWaveConnection)
#include <vcl.h>
#pragma hdrstop
#include "StarWaveConnection.h"
/////////////////////////////////////////////////////////////////////////////
// TStarWaveConnectionImpl
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Connect(BSTR IP, long Port, long Timeout)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString IPStr( IP );

			FIP   = IPStr.c_str();
			FPort = Port;
			FConnection->Connect( IPStr.c_str(), Port, Timeout );
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Disconnect()
{
	try
	{
		if( FConnection != NULL )
			FConnection->Disconnect();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::DownloadContract()
{
	try
	{
		FConnection->Clear();
		if( FConnection != NULL )
			FConnection->DownloadContract();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_AppName(BSTR* Value)
{
	try
	{
		WideString AppStr( FAppName.c_str() );
		*Value = AppStr.Copy();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_IP(BSTR* Value)
{
	try
	{
		WideString IPStr( FIP.c_str() );
		*Value = IPStr.Copy();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_Port(long* Value)
{
	*Value = FPort;
	 return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetExchange(long Index, BSTR* Exchange)
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
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSecurityDefinition(BSTR Exchange, BSTR Symbol,
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
				TSecurityDefinitionImpl* DefImpl = dynamic_cast<TSecurityDefinitionImpl*>(SecurityDefinition);
				DefImpl->FromBasicInfo(Info);
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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Init()
{
  try
  {
	if( FConnection == NULL )
	{
		FAppName.Printf( "%s.%d", UFC::Hostname, UFC::GetTickCountMS() );
		FConnection = new MDConnection( this, FAppName );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType)

















{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		AnsiString SymStr( Symbol );

		FConnection->Subscribe( ExStr.c_str(), SymStr.c_str(),SubscribeType );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Unsubscribe(BSTR Exchange, BSTR Symbol)
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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Destroy()
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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SymbolCount(BSTR Exchange, long* Count)
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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSymbol(BSTR Exchange, long Index, BSTR* Symbol)

















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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeAll()
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
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSecurityDefinitionByIndex(BSTR Exchange,
          long Index, ISecurityDefinition* SecurityDefinition, VARIANT_BOOL* IsExist)

















{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			BasicInformation* Info = FConnection->GetBasicInformation( ExStr.c_str(), Index );

			if( Info != NULL )
			{
				TSecurityDefinitionImpl* DefImpl = dynamic_cast<TSecurityDefinitionImpl*>(SecurityDefinition);
				DefImpl->FromBasicInfo(Info);
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
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->SubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->UnsubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SendNews(BSTR Group, BSTR Message)
{
	AnsiString GroupStr( Group );
	AnsiString MsgStr( Message );

	if( FConnection != NULL )
		FConnection->SendNews( GroupStr.c_str(), MsgStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->SubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->UnsubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
/*
STDMETHODIMP TStarWaveConnectionImpl::Subscribe2(BSTR Exchange)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			FConnection->Subscribe2( ExStr.c_str() );
		}
	}
	catch(Exception &e)
	{
		AnsiString ExMsg( e.Message.c_str() );
		UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
*/
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Recover(BSTR Exchange)
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
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth,
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
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}

