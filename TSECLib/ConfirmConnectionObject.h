#ifndef __CONFIRM_CONNECTION_OBJECT_H
#define __CONFIRM_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "Messages.h"
#include "../UFC/UFC.h"
#include "../TSECLib/ExchangeConst.h"
//-----------------------------------------------------------------------------------------------------------------------
//   TAIFEX Side: Connection Object for AP code 3 ( Matched Confirm report )
//--------------------------------------------------------------------------------------------------------------------
extern UFC::SInt TotalCount;
extern UFC::SInt TotalProcessingTime;
extern UFC::SInt MaxProcessingTime;
extern UFC::PStringList Executions;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
//-----------------------------------------------------------------------------------------------------------------------
class ConfirmReader
{
public:
    int Read( UFC::PStringList& ExecList, int ReadPosition )
    { 
        int Begin = ReadPosition-1;///< to Zero base        
        int Count = 0;        
        
        for( int i = Begin; i < Executions.ItemCount(); i++ )
        {
            ExecList.Add( Executions.GetItem( i ) );
            Count++;
        }
        return Count; 
    }
};
//-----------------------------------------------------------------------------------------------------------------------
class TSECConfirmConnectionObject : public TWSE::ConnectionObjectBase, public UFC::PThread
{
private:
    BOOL                     FHandshakeReady;
    BOOL                     FNeedLock;
    BOOL                     FNeedReset;
    ConnectionListener*      FConnectionListener;
    int                      FPreviousTime;
    LinkSubSystemTAIFEX*     FConnection;
    UFC::PtrList<MessageR3>  FMessages;
    UFC::PCriticalSection    FMsgCriticalSection;
    int                      FSendPosition;
    int                      FReadPosition;
    int                      FEndPosition;
    int                      FTotalCount;
    char*                    FReportBuffer;
    BOOL                     FConnected;	
    ConfirmReader*           FConfirmReader;
    UFC::BufferedLog*              FLog;
private:
    void IdleHandshake( void );
    void ConfirmsubSystemHandshake( BOOL ReceivedL010 );
    void MessageLoop( void );
    void ClearMessageList();
    BOOL ReadMessage();
    BOOL MessageOut();
    MessageHeader* CheckR1( MessageR1* C010 );
    void AddReportMessage( UFC::PStringList& ExecList );
    BOOL SendConfirmMessage( void );
    void LockLoop( void );
    virtual void Execute( void );
public:
    TSECConfirmConnectionObject( ConnectionParameter& info,
              		         ConnectionListener*  Listener,
               		         UFC::ThreadListener* TListener );
    virtual ~TSECConfirmConnectionObject();
    void SetLogManager( UFC::BufferedLog* LogObj );
    void WaitForConnected( void );
    void AddReportMessage( MessageR3* R3 );
public:
    virtual void Active( void ) { UFC::PThread::Start(); }
    virtual BOOL Accept( int FD );
    virtual void Close( void );
    virtual void OnCommand( int Command, void* Data );
    virtual void UpdateConnectionParameter( ConnectionParameter& Info );
    virtual BOOL IsEnable( void ) { return TRUE; }      
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
