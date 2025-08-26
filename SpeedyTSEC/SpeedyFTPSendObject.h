//---------------------------------------------------------------------------
// SpeedyFTPSendObject
//---------------------------------------------------------------------------
#ifndef SpeedyFTPSendObject_H
#define SpeedyFTPSendObject_H
//---------------------------------------------------------------------------
#include "SpeedyFTPConnectionObject.h"
//---------------------------------------------------------------------------
class TFtpJobObject
{
private:
    UFC::AnsiString FFileCode;
    int FStartSeq;
    int FEndSeq;
public:
    TFtpJobObject( const UFC::AnsiString& FileCode, int StartSeq = 0, int EndSeq = 0 ) : FFileCode( FileCode ), FStartSeq( StartSeq ), FEndSeq( EndSeq ) {}
    UFC::AnsiString& GetFileCode( void ) { return FFileCode; }
    int 			 GetStartSeq(void )  { return FStartSeq; }
    int 			 GetEndSeq( void )   { return FEndSeq; }

};
//---------------------------------------------------------------------------
class TSpeedyFTPSendObject : public TSpeedyFTPConnectionObject
{
private:
    UFC::PCriticalSection FCS;
    UFC::PList FJobs;
public:
    TSpeedyFTPSendObject( const UFC::AnsiString& SourceID, const UFC::AnsiString& DestinationID, ConnectionParameter& info, UFC::ThreadListener* TListener, ConnectionListener* CListener );
protected:
    virtual void CustomMessageLoopFunction( void );
    virtual void OnIdle( void );
private:
    void ViewJobList( void );
    BOOL Work( TFtpJobObject* pJob );
    BOOL SendF010( TFtpJobObject* pJob );
    BOOL SendF050( TFtpJobObject* pJob );
    void SendF070( void );
public:
    void AddNewJob( TFtpJobObject* pJob );
};
//---------------------------------------------------------------------------
#endif // SpeedyFTPSendObject_H
