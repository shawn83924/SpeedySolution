// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// C++ TLBWRTR : $Revision:   1.151.1.0.1.27  $
// File generated on 2004/4/2 ¤U¤È 04:52:05 from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\WINNT\system32\msnetobj.dll (1)
// LIBID: {8AA720BF-7468-4DA1-97DA-66D2E41B3DDA}
// LCID: 0
// Helpfile: 
// HelpString: msnetobj 1.0 Type Library
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINNT\system32\stdole2.tlb)
// Errors:
//   Error creating palette bitmap of (TRMGetLicense) : Server C:\WINNT\System32\msnetobj.dll contains no icons
// ************************************************************************ //

#include <vcl.h>
#pragma hdrstop

#include <olectrls.hpp>
#include <oleserver.hpp>
#if defined(USING_ATL)
#include <atl\atlvcl.h>
#endif

#include "MSNETOBJLib_OCX.h"

#if !defined(__PRAGMA_PACKAGE_SMART_INIT)
#define      __PRAGMA_PACKAGE_SMART_INIT
#pragma package(smart_init)
#endif

namespace Msnetobjlib_tlb
{

IRMGetLicensePtr& TRMGetLicense::GetDefaultInterface()
{
  if (!m_DefaultIntf)
    Connect();
  return m_DefaultIntf;
}

_di_IUnknown __fastcall TRMGetLicense::GetDunk()
{
  _di_IUnknown diUnk;
  if (m_DefaultIntf) {
    IUnknownPtr punk = m_DefaultIntf;
    diUnk = LPUNKNOWN(punk);
  }
  return diUnk;
}

void __fastcall TRMGetLicense::Connect()
{
  if (!m_DefaultIntf) {
    _di_IUnknown punk = GetServer();
    m_DefaultIntf = punk;
    if (ServerData->EventIID != GUID_NULL)
      ConnectEvents(GetDunk());
  }
}

void __fastcall TRMGetLicense::Disconnect()
{
  if (m_DefaultIntf) {
    
    if (ServerData->EventIID != GUID_NULL)
      DisconnectEvents(GetDunk());
    m_DefaultIntf.Reset();
  }
}

void __fastcall TRMGetLicense::BeforeDestruction()
{
  Disconnect();
}

void __fastcall TRMGetLicense::ConnectTo(IRMGetLicensePtr intf)
{
  Disconnect();
  m_DefaultIntf = intf;
  if (ServerData->EventIID != GUID_NULL)
    ConnectEvents(GetDunk());
}

void __fastcall TRMGetLicense::InitServerData()
{
  static Oleserver::TServerData sd;
  sd.ClassID = CLSID_RMGetLicense;
  sd.IntfIID = __uuidof(IRMGetLicense);
  sd.EventIID= GUID_NULL;
  ServerData = &sd;
}


};     // namespace Msnetobjlib_tlb


// *********************************************************************//
// The Register function is invoked by the IDE when this module is 
// installed in a Package. It provides the list of Components (including
// OCXes) implemented by this module. The following implementation
// informs the IDE of the OCX proxy classes implemented here.
// *********************************************************************//
namespace Msnetobjlib_ocx
{

void __fastcall PACKAGE Register()
{
  // [1]
  TComponentClass cls_svr[] = {
                              __classid(Msnetobjlib_tlb::TRMGetLicense)
                           };
  RegisterComponents("Chio", cls_svr,
                     sizeof(cls_svr)/sizeof(cls_svr[0])-1);
}

};     // namespace Msnetobjlib_ocx
