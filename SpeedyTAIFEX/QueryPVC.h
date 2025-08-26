/**@file QueryFCM.cp
*  @brief Header file for QueryFCM.cp
*  @author Simon Chaang
*
*/
/*-----------------------------------------------------------------------------------------*/
#ifndef _QUERYPVC_H
#define _QUERYPVC_H
/*-----------------------------------------------------------------------------------------*/
#include "../UFC/UFC.h"
/*-----------------------------------------------------------------------------------------*/
#define MAX_FCM     120
#define MAX_PRODUCT 50
#define MAX_STRLEN  30
#define BROKERID_FIELD_LEN 8
#define PVCNO_FIELD_LEN    5
#define PASSWORD_FIELD_LEN 5
#define URL_FIELD_LEN      128
/*-----------------------------------------------------------------------------------------*/
class TFXSessions
{
private:
    char XTCS_BROKER_IDs[MAX_FCM][BROKERID_FIELD_LEN];
    char XTCS_PASSWORDs[MAX_FCM][PASSWORD_FIELD_LEN];
    char XTCS_URLs[MAX_FCM][URL_FIELD_LEN];
    int  XTCS_PVC_NOs[MAX_FCM];
    int  XTCS_LINK_NOs[MAX_FCM];
    char XTCS_CMs[MAX_FCM][5];
    BOOL IsOptions;
    BOOL IsTesting;
    char FFileName[ 128 ];
    UFC::PHashedSet<UFC::AnsiString> FFCMSet;
private:    
    int  ParseLine( char* Line, int APCode, int Index  );
    BOOL CheckURL( char* Line, UFC::AnsiString& NewLine );
    BOOL LOOKUpURL( const UFC::AnsiString& BrokerID, int SessionID, UFC::AnsiString& NewURL, BOOL IsCM );
public:
    TFXSessions( BOOL IsOption, BOOL Testing );
public:
    int   GetLines( int ApCode );
    char* GetCM( int Index );
    char* GetBrokerID( int Index );
    char* GetLinePassword( int Index );
    char* GetURL( int Index );
    int   GetPVC( int Index );
    int   GetLink( int Index );
};    
/*-----------------------------------------------------------------------------------------*/
#endif
/*-----------------------------------------------------------------------------------------*/
