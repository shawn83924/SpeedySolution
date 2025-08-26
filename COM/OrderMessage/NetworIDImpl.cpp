// NETWORIDIMPL : Implementation of TNetworIDImpl (CoClass: NetworID, Interface: INetworID)

#include <vcl.h>
#pragma hdrstop

#include "NETWORIDIMPL.H"
#include <TNetworkID.h>

/////////////////////////////////////////////////////////////////////////////
// TNetworIDImpl

STDMETHODIMP TNetworkIDImpl::Init(long CID)
{
    TNetworkID::Init( CID );
	return S_OK;
}

