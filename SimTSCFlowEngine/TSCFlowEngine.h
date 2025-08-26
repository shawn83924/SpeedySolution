#ifndef SPEEDY_FLOW_ENGINE_H
#define SPEEDY_FLOW_ENGINE_H
//------------------------------------------------------------------------------
const Int32 MAX_SUPPORT_PVCS = 256;
//------------------------------------------------------------------------------
class SpeedyInfo
{
public:
    UFC::AnsiString Host;
    Int32      FUTPVCCount;
    Int32      OPTPVCCount;
    BOOL       FUTAlive;
    BOOL       OPTAlive;
public:
    SpeedyInfo( const UFC::AnsiString& Hostname, Int32 FUTPVCs, Int32 OPTPVCs )
    :Host(Hostname),
     FUTPVCCount(FUTPVCs),
     OPTPVCCount(OPTPVCs),
     FUTAlive(FALSE),
     OPTAlive(FALSE){}
};
//------------------------------------------------------------------------------
class OrderObj
{
public:
    UFC::AnsiString Order; ///<
    UFC::AnsiString EDS;   ///<4+3+8 bytes
    Int32           NID;   ///<4 bytes
public:
    OrderObj( Int32 nid, UFC::AnsiString& order, UFC::AnsiString& eds )
    :Order( order ),EDS( eds ),NID( nid ){}
};
//----------------------------------------------------------------------------------------------------------------------
class AdminCommandListener : public MessageListener
{
private:
    BOOL CheckSpeedyStat( MTree* Data );
    BOOL OnSpeedyStatChange( BOOL IsOnline, MTree* Data );
public:
    virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//------------------------------------------------------------------------------
class WakeupListener : public MessageListener
{
public:
    void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//------------------------------------------------------------------------------
class OrderListener : public MessageListener
{
private:
    void GenerateOrderID( UFC::AnsiString & OrderID,BOOL Quote = FALSE );
    void ConvertOrder( UFC::AnsiString & Order,BOOL IsOpt );
public:
    void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class OPTConfirmListener : public MessageListener
{
public:
    void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class FUTConfirmListener : public MessageListener
{
public:
    void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//------------------------------------------------------------------------------
class FillListener : public MessageListener
{
private:
    BOOL FIsOption;
public:
    FillListener( BOOL IsOption ):FIsOption( IsOption ){}
    void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
