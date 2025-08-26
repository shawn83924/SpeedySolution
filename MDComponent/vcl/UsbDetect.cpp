//---------------------------------------------------------------------------
#include <vcl.h>
//#include <stdio.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <winioctl.h>

#pragma hdrstop
#include "UsbDetect.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TUsbDetect *)
{
    new TUsbDetect(NULL);
}
//---------------------------------------------------------------------------
namespace Usbdetect
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TUsbDetect)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
bool IsDeviceDetected = false;
USHORT GlobalTargetVID = 0;
USHORT GlobalTargetPID = 0;
TUsbDevice* GlobalTargetDevice = NULL;
//---------------------------------------------------------------------------
//
//  TUsb
//
//---------------------------------------------------------------------------
__fastcall TUsb::TUsb(tUsbType Type)
:FType(Type)
,FHandle(INVALID_HANDLE_VALUE)
,FParent(NULL)
,FSymbolicName("")
{
}
//---------------------------------------------------------------------------
__fastcall TUsb::~TUsb(void)
{
    Close();
}
//---------------------------------------------------------------------------
HANDLE __fastcall TUsb::Open(void)
{
    if( FHandle != INVALID_HANDLE_VALUE || FSymbolicName.Length() <= 4 )
        return FHandle;
    
    SECURITY_ATTRIBUTES SA;  // needed for windows 2000
	SA.nLength = sizeof(SECURITY_ATTRIBUTES);
	SA.lpSecurityDescriptor = NULL;
	SA.bInheritHandle = false;

    FHandle = CreateFile( FSymbolicName.c_str(),
                          GENERIC_WRITE,
                          FILE_SHARE_WRITE,
                          &SA,
                          OPEN_EXISTING,
                          0,
                          NULL);
    return FHandle;
}
//---------------------------------------------------------------------------
void __fastcall TUsb::Close(void)
{
    if( FHandle != INVALID_HANDLE_VALUE )
    {
        CloseHandle( FHandle );
        FHandle = INVALID_HANDLE_VALUE;
    }
}
//---------------------------------------------------------------------------
//
//  TUsbDevice
//
//---------------------------------------------------------------------------
__fastcall TUsbDevice::TUsbDevice(PUSB_NODE_CONNECTION_INFORMATION Info, TUsb* Parent)
:TUsb(tUsbDevice)
,FPortIndex(Info->ConnectionIndex)
{
    FParent = Parent;
    Update( Info );
}
//---------------------------------------------------------------------------
__fastcall TUsbDevice::~TUsbDevice(void)
{

}
//---------------------------------------------------------------------------
void __fastcall TUsbDevice::Update(PUSB_NODE_CONNECTION_INFORMATION Info)
{
    if( Info != NULL )
        memcpy( &FDiviceDesc, &(Info->DeviceDescriptor), sizeof(USB_DEVICE_DESCRIPTOR) );
    FProduct = GetDescString( FDiviceDesc.iProduct );

    if( GlobalTargetVID != 0 && GlobalTargetPID != 0 )
    {
        if( GlobalTargetVID == FDiviceDesc.idVendor && GlobalTargetPID == FDiviceDesc.idProduct )
        {
            IsDeviceDetected = true;
            GlobalTargetDevice = this;
        }
    }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TUsbDevice::GetDescString(UCHAR DescCode)
{
	USB_DESCRIPTOR_REQUEST Packet;
	DWORD BytesReturned;

	memset(&Packet, 0, sizeof(Packet));
	Packet.ConnectionIndex = FPortIndex;
	Packet.SetupPacket.bmRequest = 0x80;
	Packet.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
	Packet.SetupPacket.wValue[0] = DescCode;
	Packet.SetupPacket.wValue[1] = USB_STRING_DESCRIPTOR_TYPE;
	Packet.SetupPacket.wIndex[0] = LANG_CHINESE;
	Packet.SetupPacket.wIndex[1] = LANG_CHINESE;
	Packet.SetupPacket.wLength[0] = 255;
	bool Success = DeviceIoControl( FParent->Open(),
                                    IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                                    &Packet,
                                    sizeof(Packet),
                                    &Packet,
                                    sizeof(Packet),
                                    &BytesReturned,
                                    NULL );
    GetSymbolicName();
    Close();
    if( Success == true )
        return AnsiString( (WCHAR*)&(Packet.Data[2]) );
    else
        return AnsiString("");
}
//---------------------------------------------------------------------------
//
//  TUsbPort
//
//---------------------------------------------------------------------------
__fastcall TUsbPort::TUsbPort(UCHAR PortIndex, TUsb* Parent)
:TUsb(tUsbPort)
,FPortIndex(PortIndex)
,FConnectedDeviceType(tUsbNone)
,FConnectedDevice(NULL)
{
    FParent = Parent;
}
//---------------------------------------------------------------------------
__fastcall TUsbPort::~TUsbPort(void)
{
    if( FConnectedDevice != NULL )
        delete FConnectedDevice;
}
//---------------------------------------------------------------------------
char __fastcall TUsbPort::Update(PUSB_NODE_CONNECTION_INFORMATION PortInfo)
{
    memset( PortInfo, 0, sizeof(USB_NODE_CONNECTION_INFORMATION) );
    PortInfo->ConnectionIndex = FPortIndex;
    GetNodeInformation(PortInfo);

	if( PortInfo->ConnectionStatus != DeviceConnected )
		FConnectedDeviceType = tUsbNone;
	else
	{
		if( !PortInfo->DeviceIsHub )
            FConnectedDeviceType = tUsbDevice;
		else
            FConnectedDeviceType = tUsbHub;
	}
    return FConnectedDeviceType;
}
//---------------------------------------------------------------------------
void __fastcall TUsbPort::GetNodeInformation(PUSB_NODE_CONNECTION_INFORMATION PortInfo)
{
	DWORD BytesReturned;
    if( DeviceIoControl( FParent->Open(),
                         IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,
                         PortInfo,
                         sizeof(USB_NODE_CONNECTION_INFORMATION),
                         PortInfo,
                         sizeof(USB_NODE_CONNECTION_INFORMATION),
                         &BytesReturned,
                         NULL ) == false )
    {
        throw( TUsbException("error: failed in getting information of the port") );
    }
}
//---------------------------------------------------------------------------
void __fastcall TUsbPort::SetConnectedDevice(TUsb* Device)
{
    if( FConnectedDevice != NULL )
        delete FConnectedDevice;
    FConnectedDevice = Device;
}
//---------------------------------------------------------------------------
//
//  TUsbHub
//
//---------------------------------------------------------------------------
__fastcall TUsbHub::TUsbHub(TUsb* Parent)
:TUsb(tUsbHub)
,FTotalPorts(0)
,FPortList(new TList)
,FPortIndex(0)
{
    if( (FParent = Parent) != NULL )
    {
        if( FParent->GetDeviceType() == tUsbHostController )
            GetRootHubName();
        LookUpPorts();
    }
}
//---------------------------------------------------------------------------
__fastcall TUsbHub::TUsbHub(DWORD PortIndex, TUsb* Parent)
:TUsb(tUsbHub)
,FTotalPorts(0)
,FPortList(new TList)
,FPortIndex(PortIndex)
{
    if( (FParent = Parent) != NULL )
    {
        if( FParent->GetDeviceType() == tUsbHub )
            GetHubName();
        LookUpPorts();
    }
}
//---------------------------------------------------------------------------
__fastcall TUsbHub::~TUsbHub(void)
{
    ClearPortList();
    delete FPortList;
}
//---------------------------------------------------------------------------
void __fastcall TUsbHub::ClearPortList(void)
{
    try
    {
        TUsbPort* UsbPort = NULL;
        for( int i = FPortList->Count - 1; i >= 0; --i )
        {
            if( (UsbPort = (TUsbPort*)FPortList->Items[i]) != NULL )
            {
                delete UsbPort;
                UsbPort = NULL;
            }
        }
        FPortList->Clear();
    }
    catch(...)
    {
        throw( TUsbException("error: failed in clearing port list of the hub") );
    }
}
//---------------------------------------------------------------------------
void __fastcall TUsbHub::GetRootHubName(void)
{
    USB_ROOT_HUB_NAME HubName;
    DWORD ByteReturn;
    if( DeviceIoControl( FParent->Open(),
                         IOCTL_USB_GET_ROOT_HUB_NAME,
                         &HubName,
                         sizeof(USB_HUB_NAME),
                         &HubName,
                         sizeof(USB_HUB_NAME),
                         &ByteReturn,
                         NULL ) == true )
    {
        FSymbolicName = "\\\\.\\" + AnsiString( (WCHAR*)&(HubName.RootHubName[0]) );
    }
    FParent->Close();
}
//---------------------------------------------------------------------------
void __fastcall TUsbHub::GetHubName(void)
{
    USB_NODE_CONNECTION_NAME HubName;
    DWORD BytesReturned;
    HubName.ConnectionIndex = FPortIndex;
	if( DeviceIoControl( FParent->Open(),
  		    		     IOCTL_USB_GET_NODE_CONNECTION_NAME,
			    	     &HubName,
				         sizeof(HubName),
				         &HubName,
                         sizeof(HubName),
				         &BytesReturned,
                         NULL ) == true )
    {
        FSymbolicName = "\\\\.\\" + AnsiString( (WCHAR*)&(HubName.NodeName[0]) );
    }
    FParent->Close();
}
//---------------------------------------------------------------------------
void __fastcall TUsbHub::LookUpPorts(void)
{
    if( Open() == INVALID_HANDLE_VALUE )
        throw( TUsbException("error: failed in opening handle of the hub") );

    USB_NODE_INFORMATION NodeInformation;
    DWORD BytesReturned = 0;
    bool Success = DeviceIoControl( FHandle,
	               		            IOCTL_USB_GET_NODE_INFORMATION,
	                    		    &NodeInformation,
	                           		sizeof(NodeInformation),
	                           		&NodeInformation,
	                           		sizeof(NodeInformation),
	                           		&BytesReturned,
 	                           		NULL );
    Close();
	if( Success == false )
        throw( TUsbException("error: failed in getting information of the hub") );

    FTotalPorts = NodeInformation.u.HubInformation.HubDescriptor.bNumberOfPorts;
}
//---------------------------------------------------------------------------
void __fastcall TUsbHub::Update(void)
{
    ClearPortList();
    TUsbPort* UsbPort = NULL;
	for( UCHAR i = 1; i <= FTotalPorts; ++i )// index of the first port is 1
	{
        if( (UsbPort = new TUsbPort(i, this)) != NULL )
        {
            FPortList->Add( (void*)UsbPort );
            USB_NODE_CONNECTION_INFORMATION PortInfo;
            tUsbType ConnectedDevice = UsbPort->Update(&PortInfo);
            if( ConnectedDevice == tUsbDevice )
            {
                TUsbDevice* ConnectedDevice = new TUsbDevice( &PortInfo, this );
                UsbPort->SetConnectedDevice( ConnectedDevice );
            }
            else if( ConnectedDevice == tUsbHub )
            {
                TUsbHub* ConnectedDevice = new TUsbHub( i, this );
                ConnectedDevice->Update();
                UsbPort->SetConnectedDevice( ConnectedDevice );

            }

            UsbPort = NULL;
        }
	}
    Close();
}
//---------------------------------------------------------------------------
//
//  TUsbHostController
//
//---------------------------------------------------------------------------
__fastcall TUsbHostController::TUsbHostController(const AnsiString& SymbolicName)
:TUsb(tUsbHostController)
,FRootHub(NULL)
,FDriverKey("")
,FDriverName("")
{
    FSymbolicName = SymbolicName;
    Open();
    GetHostControllerInfo();
    Update();
    Close();
}
//---------------------------------------------------------------------------
__fastcall TUsbHostController::~TUsbHostController(void)
{
    if( FRootHub != NULL )
        delete FRootHub;
}
//---------------------------------------------------------------------------
void __fastcall TUsbHostController::GetHostControllerInfo(void)
{
    BOOL                    success;
    ULONG                   nBytes;
    USB_HCD_DRIVERKEY_NAME  driverKeyName;
    FDriverKey = "";
    FDriverName = "";
    memset( &driverKeyName, 0, MAX_PATH );
    driverKeyName.ActualLength = sizeof( USB_HCD_DRIVERKEY_NAME );
    success = DeviceIoControl( FHandle, IOCTL_GET_HCD_DRIVERKEY_NAME, &driverKeyName,
    sizeof(driverKeyName), &driverKeyName, sizeof(driverKeyName), &nBytes, NULL);
	if( success )
	{
    	FDriverKey = AnsiString( (WCHAR*)&(driverKeyName.DriverKeyName[0]) );
	}

	if( FDriverKey.Length() > 0 )
		FDriverName = DriverNameToDeviceDesc();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TUsbHostController::DriverNameToDeviceDesc(void)
{
	CHAR buf[2048];
	
	DEVINST     devInst;
	DEVINST     devInstNext;
	CONFIGRET   cr;
	ULONG       walkDone = 0;
	ULONG       len;
	
	// Get Root DevNode
	cr = CM_Locate_DevNode(&devInst, NULL, 0);
	
	if( cr != CR_SUCCESS )
		return AnsiString( "" );
	
	// Do a depth first search for the DevNode with a matching
	// DriverName value
	while( !walkDone )
	{
		// Get the DriverName value
		len = sizeof(buf);
		cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DRIVER, NULL, buf, &len, 0);
		
		// If the DriverName value matches, return the DeviceDescription
		if( cr == CR_SUCCESS && strcmp(FDriverKey.c_str(), buf) == 0 )
		{
			len = sizeof(buf);
			cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DEVICEDESC, NULL, buf, &len, 0);
			
			if (cr == CR_SUCCESS)
				return AnsiString( buf );
			else
			    return AnsiString( "" );
		}
	
		// This DevNode didn't match, go down a level to the first child.
		cr = CM_Get_Child(&devInstNext, devInst, 0);
		
		if( cr == CR_SUCCESS )
		{
			devInst = devInstNext;
			continue;
		}
	
		// Can't go down any further, go across to the next sibling.  If
		// there are no more siblings, go back up until there is a sibling.
		// If we can't go up any further, we're back at the root and we're
		// done.
		for(;;)
		{
			cr = CM_Get_Sibling(&devInstNext, devInst, 0);
			
			if (cr == CR_SUCCESS)
			{
				devInst = devInstNext;
				break;
			}
	
			cr = CM_Get_Parent(&devInstNext, devInst, 0);
	
	
			if (cr == CR_SUCCESS)
				devInst = devInstNext;
			else
			{
				walkDone = 1;
				break;
			}
		}
	}
	return "";
}
//---------------------------------------------------------------------------
bool __fastcall TUsbHostController::Update(void)
{
    try
    {
        if( FRootHub == NULL )
            FRootHub = new TUsbHub(this);
        FRootHub->Update();
        return true;
    }
    catch(...)
    {
        delete FRootHub;
        FRootHub = NULL;
    }
    return false;
}
//---------------------------------------------------------------------------
//
//  TUsbDetect
//
//---------------------------------------------------------------------------
__fastcall TUsbDetect::TUsbDetect(TComponent* Owner)
:TComponent(Owner)
,FMaxHostController(10)
,FHostControllerList(new TList)
,FTargetVID(0)
,FTargetPID(0)
,FTargetConnected(false)
{
    DetectHostController();
}
//---------------------------------------------------------------------------
__fastcall TUsbDetect::~TUsbDetect(void)
{
    ClearHostControllerList();
    delete FHostControllerList;
}
//---------------------------------------------------------------------------
void __fastcall TUsbDetect::DetectHostController(void)
{
    AnsiString HostControllerName;
    TUsbHostController* HostController = NULL;
    ClearHostControllerList();
	for( int i = 0; i < 10; ++i)
    {
        HostControllerName.printf( "\\\\.\\HCD%d", i );
        if( (HostController = new TUsbHostController(HostControllerName)) != NULL )
        {
            if( HostController->GetRootHub() != NULL )
                FHostControllerList->Add( (void*)HostController );
            else
                delete HostController;
            HostController = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TUsbDetect::ClearHostControllerList(void)
{
    TUsbHostController* HostController = NULL;
    for( int i = FHostControllerList->Count - 1; i >= 0; --i )
    {
        if( (HostController = (TUsbHostController*)FHostControllerList->Items[i]) != NULL )
        {
            delete HostController;
            HostController = NULL;
        }
    }
    FHostControllerList->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TUsbDetect::Update(void)
{
    IsDeviceDetected = false;
    GlobalTargetDevice = NULL;

    TUsbHostController* UsbHostController = NULL;
    for( int i = FHostControllerList->Count - 1; i >= 0; --i )
    {
        UsbHostController = (TUsbHostController*)FHostControllerList->Items[i];
        if( UsbHostController != NULL )
        {
            UsbHostController->Update();
            UsbHostController = NULL;
        }
    }

    if( FTargetConnected == false )
    {
        if( IsDeviceDetected == true && GlobalTargetDevice != NULL )
        {
            FTargetConnected = true;
            if( FOnConnected != NULL )
                FOnConnected( GlobalTargetDevice );
        }
    }
    else
    {
        if( IsDeviceDetected == false || GlobalTargetDevice == NULL )
        {
            FTargetConnected = false;
            if( FOnDisconnected != NULL )
                FOnDisconnected(this);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TUsbDetect::SetTargetVID( USHORT VID )
{
    GlobalTargetVID = VID;
    FTargetVID = VID;
}
//---------------------------------------------------------------------------
void __fastcall TUsbDetect::SetTargetPID( USHORT PID )
{
    GlobalTargetPID = PID;
    FTargetPID = PID;
}
//---------------------------------------------------------------------------

