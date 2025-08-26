#ident "@(#) $Id: TransactionObject.h,v 1.35 2004/05/21 10:34:21 ap27 Exp $"
//----------------------------------------------------------------------------------------------------------------------
#ifndef __TransactionObject_h
#define __TransactionObject_h
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../SimTAIFEX/QueryFCM.h"
#include "Messages.h"
#include "XBIODefine.h"
#include "TMPMessage.h"
#include "../../inc/XBIO_msg.h"
#include "../../tap_client/TapClientCls.h"
//----------------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;
//----------------------------------------------------------------------------------------------------------------------
const int OPEN_SWITCH_CHANGED  = 1000;
//----------------------------------------------------------------------------------------------------------------------
class TransactionObject
{
public:
   virtual MessageHeader* OnMessage( ConnectionObjectBase* Sender ,MessageHeader* InMsg)=0;
   virtual TMP::TMPMessage* OnMessage( ConnectionObjectBase* Sender ,TMP::TMPMessage* InMsg) { return NULL; }
   virtual void OnEvent(int Type, void* Data)=0;
};
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
	otOrder,
	otQuery,
	otOther

} POrderType;
//----------------------------------------------------------------------------------------------------------------------
class OrderTransactionObject : public TransactionObject , public MessageListener, public PThread
{
private:
    int*              FIndex;
    int               FOpenSwitch;
    int               FSendMark;
    OrderSender*      FOrderSender;
    OrderCheck*       FOrderCheck;
    MessageR050*      FR050;
    MessageR020*      FR020;
    MessageR100*      FR100;
    MessageR030*      FR030;
    MessageR080*      FR080;
    PEvent*           FEvents;
    MessageHeaderPtr* FReturnMsg;
    PCriticalSection  FWriteCriticalSection;
    PCriticalSection  FSendingCriticalSection;
private: // For States.
    int			  FMaxResponseTimePVC;    ///<  The PVC number
    int                   FMaxResponseTime;       ///<  The mex response time in MS
    int          	  FOrderCount1;		      ///<  The count of the R010 "Insert" message
    int          	  FOrderCount2;		      ///<  The count of the R010 "Reduce" message
    int          	  FOrderCount3;		      ///<  The count of the R010 "Cancel" message
    int          	  FOrderCount5;		      ///<  The count of the R010 "Query" message
    int          	  FQueryCount1;			  ///<  The count of the R090 "Insert" message
    int          	  FQueryCount2;			  ///<  The count of the R090 "Reduce" message
    int          	  FQueryCount3;			  ///<  The count of the R090 "Cancel" message
    int          	  FQueryCount5;			  ///<  The count of the R090 "Query" message
    unsigned int* 	  FReceiveTime;           ///<  Received time for each PVC.
    unsigned int 	  FOrderTotalTime; 		  ///<  Total time for process orders.
    unsigned int	  FQueryTotalTime;        ///<  Total time for process queries.
    int                   FOrderIntervalCount;
    int                   FQueryIntervalCount;
    unsigned int 	  FOrderIntervalTime;
    unsigned int	  FQueryIntervalTime;
    int*		  FR060Count;  ///<  Broken line query times for each PVC.
    BOOL                  FConnected;
    int                   FLUT[10000];
private: // inline functions.
    inline MessageHeader* ReturnErrorMessage( int PVCZeroBase, int ErrCode )
    {
        FR030[ PVCZeroBase ].SetStatusCode( ErrCode );
        return &FR030[ PVCZeroBase ];
    }
    inline BOOL CheckFCM( const char* LineFCMNo,const char* FCMNo )
    {
        return ( strncmp( LineFCMNo,FCMNo, 4 ) == 0 );
    }
private:
    void InitQIF( void );
    int AddMessage(  char* AddMsg ); // test by James Lu on 3/16/04
    void SendR070( MessageR070* R070, int PVC );
    void ReceiveR080( MTree* Data );
    BOOL SendOrderQuery( MessageR010* R010, int PVC );
    BOOL SendQuoteQuery( MessageR090* R090, int PVC );
    void ReceiveR020R100( MTree* Data );
    void UpdateStates( int PdatVC0Base, POrderType type );
    BOOL ConnectTAPServer( void );
    MessageHeader* OnR090Arrived( MessageR090* InMsg, int PVC, AnsiString& BrokerID );
    MessageHeader* OnR070Arrived( MessageR070* InMsg, int PVC, AnsiString& BrokerID );
    MessageHeader* OnR010Arrived( MessageR010* InMsg, int PVC, AnsiString& BrokerID );
    BOOL SendMessageToTAPServer( void );
    MessageHeader* WaitMessage( int PVC0Base, POrderType ,char* OrderNo,char* Str, BOOL IsTAP = TRUE );
    MessageHeader* OnConfirmConnectionHandshake( int PVCZeroBase );
    MessageHeader* OnBrokenLineQuery( int PVCZeroBase );
public:
    OrderTransactionObject( void );
    void GetStates( XBIO4States* States );
    void Report( LogManager* Log );
    virtual ~OrderTransactionObject();
    void SetOrderCheckObj( OrderCheck* CheckObj );
    virtual MessageHeader* OnMessage( ConnectionObjectBase* Sender, MessageHeader* InMsg );
    virtual void OnEvent(int Type, void* Data );
    // MessageListener
    virtual void OnMigoMessage( AnsiString Subject, AnsiString Key,  MTree* );
    // PThread
    virtual void Execute( void );
};
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
