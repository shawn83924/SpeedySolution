//---------------------------------------------------------------------------
#ifndef UsbDetectH
#define UsbDetectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <windows.h>
#include <usbioctl.h>
//---------------------------------------------------------------------------
typedef char tUsbType;
tUsbType tUsbNone           = 0;
tUsbType tUsbHostController = 1;
tUsbType tUsbHub            = 2;
tUsbType tUsbPort           = 3;
tUsbType tUsbDevice         = 4;
//---------------------------------------------------------------------------
const char ConnectionStatus[] [30] =
{
    "No device connected", "Device connected", "Device FAILED enumeration",
    "Device general FAILURE", "Device caused overcurrent", "Not enough power for device"
};
//---------------------------------------------------------------------------
//
//  TUsbException
//
//---------------------------------------------------------------------------
class TUsbException : public Exception
{
public:
    __fastcall  TUsbException(void) : Exception("Unknow USB Exception") {}
    __fastcall  TUsbException(const AnsiString &ErrStr) : Exception( ErrStr ) {}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  TUsb
//
//---------------------------------------------------------------------------
class TUsb
{
protected:
    tUsbType    FType;
    AnsiString  FSymbolicName;
    HANDLE      FHandle;
    TUsb*       FParent;
public:
               __fastcall  TUsb(tUsbType Type);
    virtual    __fastcall ~TUsb(void);
    HANDLE     __fastcall  Open(void);
    void       __fastcall  Close(void);
public:
    char         __fastcall GetDeviceType(void) { return FType; }
    AnsiString   __fastcall GetSymbolicName(void) { return FSymbolicName; }
    TUsb*        __fastcall GetParentDevice(void) { return FParent; }
};
//---------------------------------------------------------------------------
//
//  TUsbDevice
//
//---------------------------------------------------------------------------
class TUsbDevice : public TUsb
{
private:
    UCHAR                 FPortIndex;
    USB_DEVICE_DESCRIPTOR FDiviceDesc;
    AnsiString            FProduct;
private:
    AnsiString __fastcall GetDescString(UCHAR DescCode);
    void       __fastcall GetDriverKey(void);
public:
               __fastcall  TUsbDevice(PUSB_NODE_CONNECTION_INFORMATION Info, TUsb* Parent);
    virtual    __fastcall ~TUsbDevice(void);
    void       __fastcall  Update(PUSB_NODE_CONNECTION_INFORMATION Info);
    AnsiString __fastcall  GetProductName(void) { return FProduct; }
    USHORT     __fastcall  GetVendorID(void) { return FDiviceDesc.idVendor; }
    USHORT     __fastcall  GetProductID(void) { return FDiviceDesc.idProduct; }
    UCHAR      __fastcall  GetPortIndex(void) { return FPortIndex; }
};
//---------------------------------------------------------------------------
//
//  TUsbPort
//
//---------------------------------------------------------------------------
class TUsbPort : public TUsb
{
private:
    UCHAR    FPortIndex;
    tUsbType FConnectedDeviceType;
    TUsb*    FConnectedDevice;
private:
    void     __fastcall  GetNodeInformation(PUSB_NODE_CONNECTION_INFORMATION PortInfo);
public:
             __fastcall  TUsbPort(UCHAR PortIndex, TUsb* Parent);
             __fastcall ~TUsbPort(void);
    tUsbType __fastcall  Update(PUSB_NODE_CONNECTION_INFORMATION PortInfo);
public:
    void     __fastcall  SetConnectedDevice(TUsb* Device);
    TUsb*    __fastcall  GetConnectedDevice(void) { return FConnectedDevice; }
    tUsbType __fastcall  GetConnectedDeviceType(void) { return FConnectedDeviceType; }
};
//---------------------------------------------------------------------------
//
//  TUsbRootHub
//
//---------------------------------------------------------------------------
class TUsbHub : public TUsb
{
private:
    UCHAR  FTotalPorts;
    DWORD  FPortIndex;
private:
    void __fastcall LookUpPorts(void);
    void __fastcall ClearPortList(void);
    void __fastcall GetRootHubName(void);
    void __fastcall GetHubName(void);
public:
                 __fastcall  TUsbHub(TUsb* Parent);
                 __fastcall  TUsbHub(DWORD PortIndex, TUsb* Parent);
    virtual      __fastcall ~TUsbHub(void);
    virtual void __fastcall  Update(void);
    UCHAR        __fastcall  GetPortCount(void) { return FTotalPorts; }
    TList* FPortList;
};
//---------------------------------------------------------------------------
//
//  TUsbHostController
//
//---------------------------------------------------------------------------
class TUsbHostController : public TUsb
{
private:
    AnsiString  FDriverKey;
    AnsiString  FDriverName;
public:
    TUsbHub*    FRootHub;
private:
    AnsiString __fastcall  DriverNameToDeviceDesc(void);
    void       __fastcall  GetHostControllerInfo(void);
public:
               __fastcall  TUsbHostController(const AnsiString& DeviceName);
    virtual    __fastcall ~TUsbHostController(void);
    TUsbHub*   __fastcall  GetRootHub(void) { return FRootHub; }
    bool       __fastcall  Update(void);
    AnsiString __fastcall  GetDriverKey(void) { return FDriverKey; }
    AnsiString __fastcall  GetDriverName(void) { return FDriverName; }
};
//---------------------------------------------------------------------------
//
//  TUsbDetect
//
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *ConnectedEvent) (TUsbDevice* UsbDevice);
//---------------------------------------------------------------------------
class PACKAGE TUsbDetect : public TComponent
{
private:
    int              FMaxHostController;
    USHORT           FTargetVID;
    USHORT           FTargetPID;
    bool             FTargetConnected;
    ConnectedEvent   FOnConnected;
    TNotifyEvent     FOnDisconnected;
public:
    TList*           FHostControllerList;
private:
    void    __fastcall ClearHostControllerList(void);
    void    __fastcall SetTargetVID( USHORT VID );
    void    __fastcall SetTargetPID( USHORT PID );
    void    __fastcall DetectHostController(void);
public:
            __fastcall  TUsbDetect(TComponent* Owner);
            __fastcall ~TUsbDetect(void);
    void    __fastcall  Update(void);
__published:
    __property USHORT           TargetVID     = { read = FTargetVID, write = SetTargetVID };
    __property USHORT           TargetPID     = { read = FTargetPID, write = SetTargetPID };
    __property ConnectedEvent   OnConnected   = { read = FOnConnected, write = FOnConnected };
    __property TNotifyEvent     OnDisonnected = { read = FOnDisconnected, write = FOnDisconnected };
};
//---------------------------------------------------------------------------
#endif

