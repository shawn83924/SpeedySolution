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
    UFC::AnsiString FData;
public:
    TFtpJobObject( const UFC::AnsiString& Source, const UFC::AnsiString& FileCode, int StartSeq, int EndSeq )
    :FFileCode( FileCode )
    {
        UFC::AnsiString SourceID( Source );
        
        if( FFileCode == "R30" )
        {         
            FData.PadThis( 7,'0');
            FData.Printf( "%s%08d%08d%s", FileCode.c_str(), StartSeq, EndSeq, SourceID.c_str() );
        }
        else if( FFileCode == "R40" )
        {
            FData.PadThis( 4,'0');
            FData.Printf( "%s%08d%08d%s", FileCode.c_str(), StartSeq, EndSeq, SourceID.c_str() );
        }
        else if( FFileCode == "R50" )
        {
            FData.PadThis( 4,'0');
            FData.Printf( "%s%08d%08d%s", FileCode.c_str(), StartSeq, EndSeq, SourceID.c_str() );
        }
    }
    TFtpJobObject( const UFC::AnsiString& FileCode, const UFC::AnsiString& Data )
    :FFileCode( FileCode )
    ,FData( Data )
    {
    }
    const UFC::AnsiString& GetFileCode( void ) { return FFileCode; }
    const UFC::AnsiString& GetData( void )     { return FData; }
};
//---------------------------------------------------------------------------
class TSpeedyFTPSendObject : public TSpeedyFTPConnectionObject
{
private:
    UFC::PCriticalSection FCS;
    UFC::PList FJobs;
public:
    TSpeedyFTPSendObject( SpeedyObjectsManager* ObjManager,
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
