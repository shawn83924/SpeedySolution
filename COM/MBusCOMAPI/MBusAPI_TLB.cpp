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

// $Rev: 52393 $
// File generated on 2016/6/4 ¤W¤È 12:23:58 from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\src\COM\MBusCOMAPI\MBusAPI (1)
// LIBID: {80657B39-789C-44B3-B6FD-7B1E0EEA8DCE}
// LCID: 0
// Helpfile:
// HelpString: MBusAPI COM API
// DepndLst:
//   (1) v2.0 stdole, (C:\Windows\SysWOW64\stdole2.tlb)
// SYS_KIND: SYS_WIN32
// ************************************************************************ //

#include <vcl.h>
#pragma hdrstop

#include "MBusAPI_TLB.h"

#if !defined(__PRAGMA_PACKAGE_SMART_INIT)
#define      __PRAGMA_PACKAGE_SMART_INIT
#pragma package(smart_init)
#endif

namespace Mbusapi_tlb
{


// *********************************************************************//
// GUIDS declared in the TypeLibrary
// *********************************************************************//
const GUID LIBID_MBusAPI = {0x80657B39, 0x789C, 0x44B3,{ 0xB6, 0xFD, 0x7B,0x1E, 0x0E, 0xEA,0x8D, 0xCE} };
const GUID IID_IMdTransport = {0x5CA7595B, 0xB4F6, 0x49D7,{ 0x89, 0x1A, 0x97,0x08, 0x1A, 0xF2,0x4A, 0x84} };
const GUID DIID_IMdTransportEvents = {0x6690CD53, 0x07CE, 0x46C2,{ 0xBA, 0xD4, 0x9B,0x61, 0x41, 0x8E,0x9A, 0xC6} };
const GUID CLSID_MdTransport = {0xE370FC1A, 0x9E50, 0x4B37,{ 0x93, 0xB8, 0x2B,0x0F, 0x82, 0xF9,0xA3, 0x4F} };
const GUID IID_IMdMessage = {0x8B002554, 0xD036, 0x4782,{ 0xBA, 0x8A, 0x95,0xF4, 0xA7, 0xA0,0x3A, 0xDE} };
const GUID CLSID_MdMessage = {0x446A5F7C, 0xC0CD, 0x4AF0,{ 0x83, 0xAD, 0x3E,0x00, 0x97, 0xA7,0xDF, 0xE8} };
const GUID IID_IMdListener = {0x05D6B349, 0x66FB, 0x4FFF,{ 0x9D, 0xDD, 0x33,0x86, 0x70, 0xAC,0x44, 0x3B} };
const GUID DIID_IMdListenerEvents = {0x73696846, 0xD471, 0x4A49,{ 0x9D, 0x07, 0xFB,0x79, 0x94, 0x06,0xC3, 0xB3} };
const GUID CLSID_MdListener = {0x653882B9, 0x631A, 0x43F0,{ 0xB1, 0x07, 0xBF,0xDB, 0xA6, 0x1B,0x44, 0xEA} };

};     // namespace Mbusapi_tlb

