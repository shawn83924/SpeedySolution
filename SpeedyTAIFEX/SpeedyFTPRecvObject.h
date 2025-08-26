//---------------------------------------------------------------------------
// SpeedyFTPRecvObject
//---------------------------------------------------------------------------
#ifndef SpeedyFTPRecvObject_H
#define SpeedyFTPRecvObject_H
//---------------------------------------------------------------------------
#include "SpeedyFTPConnectionObject.h"
//---------------------------------------------------------------------------
class ResendExecutionThread : public UFC::PThread
{
private:
    void Execute( void );
    long FMType;
    UFC::AnsiString FBrokerID;
public:
    ResendExecutionThread( long Mtype, const UFC::AnsiString& Broker )
    :UFC::PThread( NULL, TRUE ),
     FMType( Mtype ),
     FBrokerID( Broker )
    { Start(); }
};
//---------------------------------------------------------------------------
class TSpeedyFTPRecvObject : public TSpeedyFTPConnectionObject
{
public:
    TSpeedyFTPRecvObject( SpeedyObjectsManager* ObjManager,
                          const UFC::AnsiString& SourceID,
                          const UFC::AnsiString& DestinationID,
                          ConnectionParameter& info,
                          UFC::ThreadListener* TListener,
                          ConnectionListener* CListener,
                          BOOL IsOpt );

protected:
    virtual void CustomMessageLoopFunction( void );
    virtual void OnIdle( void );
private:
    long FmType;
    BOOL ReplyConfirmMessage( FMessageHeader* FMsg );
    BOOL Download( MessageF010* F010 );
    UFC::FileStream* OpenFile( const UFC::AnsiString& FileCode );
    MessageF030* ReceiveF030( const UFC::AnsiString& FileCode );
    BOOL SendF040( const UFC::AnsiString& FileCode, char IsEOF );
    BOOL Finished( void );
    void ToBackend( const UFC::AnsiString& FileCode );
};
//---------------------------------------------------------------------------
#endif // SpeedyFTPRecvObject_H
