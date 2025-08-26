// UNIT1 : Implementation of TMarketDataConnectionImpl (CoClass: MarketDataConnection, Interface: IMarketDataConnection)

#include <vcl.h>
#pragma hdrstop

#include "MarketDataConnectionImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TMarketDataConnectionImpl
//---------------------------------------------------------------------------
TMarketDataConnectionImpl::TMarketDataConnectionImpl():
FConnection( NULL )
{}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Connect(BSTR RemoteIP, long RemotePort)
{ 
    if( FConnection != NULL )
    {
        AnsiString AnsiRemoteIP( RemoteIP );
        FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort );
	}
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Create(BSTR AppName)
{
	AnsiString AnsiAppName( AppName );

	if( SysStringLen( AppName ) <= 2 )
		AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
	FConnection = new TMarketDataConnection( _Module.m_hInst, AnsiAppName.c_str(), this );
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Destroy()
{
	if( FConnection != NULL )
	{
		delete FConnection;
		FConnection = NULL;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Disconnect()
{
    if( FConnection != NULL )
	{
        FConnection->Disconnect();
    }
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Subscribe(BSTR Market, BSTR Symbol)
{
	AnsiString MarketStr( Market );
	AnsiString SecurityIDStr( Symbol );

	if( FConnection != NULL )
	{
		FConnection->Subscribe( MarketStr.c_str(), SecurityIDStr.c_str() );
	}
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMarketDataConnectionImpl::Unsubscribe(BSTR Market, BSTR Symbol)
{
	AnsiString MarketStr( Market );
	AnsiString SecurityIDStr( Symbol );

	if( FConnection != NULL )
	{
		FConnection->UnSubscribe( MarketStr.c_str(), SecurityIDStr.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnConnected( void )
{
	Fire_OnConnected();
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnDisconnected( void )
{
	Fire_OnDisconnected();
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnMarketDataMessage( TMarketDataMessage* Msg )
{
	VARIANT MessageInstance;
	TCOMIMarketDataMessage MessageToSend = CoMarketDataMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TMarketDataMessage* pMarketDataMessage = (TMarketDataMessage*)(MessageInstance.byref);
	pMarketDataMessage->Attach( Msg->GetMTree() );

	Fire_OnMarketData( MessageToSend );
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnQuoteRequest( TMDQuoteRequestMessage* Msg )
{
	VARIANT MessageInstance;
	TCOMIMDQuoteRequestMessage MessageToSend = CoMDQuoteRequestMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TMDQuoteRequestMessage* pMDQuoteRequestMessage = (TMDQuoteRequestMessage*)(MessageInstance.byref);
	pMDQuoteRequestMessage->Attach( Msg->GetMTree() );

	Fire_OnQuoteRequest( MessageToSend );
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnSecurityDefinition( TSecurityDefinitionMessage* Msg )
{
	VARIANT MessageInstance;
	TCOMISecurityDefinitionMessage MessageToSend = CoSecurityDefinitionMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TSecurityDefinitionMessage* pSecurityDefinitionMessage = (TSecurityDefinitionMessage*)(MessageInstance.byref);
	pSecurityDefinitionMessage->Attach( Msg->GetMTree() );

	Fire_OnSecurityDefinition( MessageToSend );
}
//------------------------------------------------------------------------------
void TMarketDataConnectionImpl::OnNews( TNewsMessage* Msg )
{
	VARIANT MessageInstance;
	TCOMINewsMessage MessageToSend = CoNewsMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TNewsMessage* pNewsMessage = (TNewsMessage*)(MessageInstance.byref);
	pNewsMessage->Attach( Msg->GetMTree() );

	Fire_OnNews( MessageToSend );
}
//------------------------------------------------------------------------------

STDMETHODIMP TMarketDataConnectionImpl::SecurityDefinitionRequest(
  ISecurityDefinitionRequestMessage* Msg)
{
  ISecurityDefinitionRequestMessagePtr MsgPtr(Msg, true);
  if( FConnection != NULL )
  {
	VARIANT MessageInstance;
	Msg->get_Instance( &MessageInstance );
	TSecurityDefinitionRequestMessage* pSecurityDefinitionRequestMessage = (TSecurityDefinitionRequestMessage*)(MessageInstance.byref);
	if( pSecurityDefinitionRequestMessage != NULL )
	{
		FConnection->SecurityDefinitionRequest(pSecurityDefinitionRequestMessage);
	}
  }
  return S_OK;
}
//------------------------------------------------------------------------------
