/**@file QueryFCM.cp
*  @brief Query all FCM information from DB
*  @author Simon Chaang
*
*/
#include "QueryPVC.h"
#include "../TAIFEXLib/TMPMessage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*--------------------------------------------------------------------------------------------------------------------*/
TFXSessions::TFXSessions( BOOL IsOption,BOOL Testing  )
:IsOptions( IsOption )
,IsTesting( Testing )
{
    char  Buffer[ 512 ];
    FILE* FFileHandle;
    UFC::PStringList NewConfigStrings;
    
    #ifdef _WIN32
    if( IsOptions )
        strcpy( FFileName, "..\\cfg\\SpeedyPVCOpt.cfg" );
    else
        strcpy( FFileName, "..\\cfg\\SpeedyPVCFut.cfg" );
    #else
    if( IsOptions )
        strcpy( FFileName, "../cfg/SpeedyPVCOpt.cfg" );
    else
        strcpy( FFileName, "../cfg/SpeedyPVCFut.cfg" );
    #endif    
    if( (FFileHandle = fopen( FFileName, "r" )) == NULL ) 
    {
        UFC::BufferedLog::Printf( " Can't open PVC configuration file \"%s\".", FFileName );
        exit( 0 );
    }
    while( fgets( Buffer, 512, FFileHandle ) != NULL ) 
    {
        UFC::AnsiString ValidSetting;
        
        if( CheckURL( Buffer, ValidSetting  ) == TRUE )
            NewConfigStrings.Add( ValidSetting );
    }
    fclose( FFileHandle );
    if( (FFileHandle = fopen( FFileName, "w+" )) != NULL ) 
    {
        for( int i = 0; i < NewConfigStrings.ItemCount(); i ++ )
            fputs( NewConfigStrings.GetItem(i).c_str(), FFileHandle );
        fclose( FFileHandle );    
    }
    else
    {
        UFC::BufferedLog::Printf( " Write PVC configuration file \"%s\".", FFileName );
        exit( 0 );
    }
}        
//--------------------------------------------------------------------------------------------------------------------
BOOL TFXSessions::LOOKUpURL( const UFC::AnsiString& BrokerID, int SessionID, UFC::AnsiString& NewURL, BOOL IsCM )
{
    UFC::AnsiString FCM( BrokerID );
    UFC::AnsiString BID( BrokerID );    
    UFC::AnsiString TFXURL;    
    UInt16          TFXPort;
            
    FCM.PadThis( 4,'0');
    BID.LowerThis();
    if( FFCMSet.Exists(FCM ) == FALSE )
    {
         FFCMSet.Add( FCM );
         TMP::TMPMessage::LoadURLTable( IsOptions, FCM );
    }
    if( TMP::TMPMessage::GetURLPort( IsOptions, BID,(UInt16) SessionID, TFXURL, TFXPort, IsTesting, IsCM ) == TRUE )
    {
        NewURL.Printf("%s:%d",TFXURL.c_str(), TFXPort );
        NewURL.LowerThis();
        ///NewURL.Printf("127.0.0.1:%d", TFXPort );
        return TRUE;
    }
    NewURL = TFXURL;
    return FALSE;            
}   
//--------------------------------------------------------------------------------------------------------------------
BOOL TFXSessions::CheckURL( char* Line, UFC::AnsiString& NewLine )
{
    UFC::PStringList Fields;
    
    Fields.SetStrings( Line, ",\n" );    
    if( strstr( Line, "#") != NULL ) ///< It's a mark line.
    {
        NewLine = Line;
        return TRUE;
    }    
    if( Fields.ItemCount() < 5 ) ///< At least need 5 fields.
        return FALSE;      
    if( Fields.ItemCount() == 5 ) ///< without URL and CM
    {
        NewLine = Line;
        return TRUE;
    }
    else if( Fields.ItemCount() == 6 ) ///< With CM or URL
    {
        if( Fields[5].Length() == 4 )///< Length = 4, It's CM ID.
        {
            NewLine = Line;
            return TRUE;
        }
        else ///< It's URL
        {
            UFC::AnsiString BrokerID( Fields[3]);
            UFC::AnsiString NewURL;
            int SessionID = Fields[1].ToInt();
            BOOL IsCM;

            if( Fields[2] == "4" )
                IsCM = FALSE;
            else
                IsCM = TRUE;
            if( LOOKUpURL( BrokerID, SessionID, NewURL, IsCM )== TRUE )
            {
                NewLine.Printf("%s,%s,%s,%s,%s,%s\n",Fields[0].c_str(),Fields[1].c_str(),Fields[2].c_str(),Fields[3].c_str(),Fields[4].c_str(),NewURL.c_str());
                return TRUE;
            }
            else
            {
                UFC::BufferedLog::Printf( " Can't find FCM[%s] SessionID[%d] in P07", BrokerID.c_str(), SessionID);
                UFC::BufferedLog::Printf( " URL[%s]", NewURL.c_str() );
                NewLine = Line;
                return TRUE;
            }
        }
    }
    else if( Fields.ItemCount() == 7 ) ///< Fields[6] It's URL
    {
        UFC::AnsiString BrokerID( Fields[3]);
        UFC::AnsiString NewURL;
        int SessionID = Fields[1].ToInt();
        BOOL IsCM;
        
        if( Fields[2] == "4" )
            IsCM = FALSE;
        else
            IsCM = TRUE;
        if( LOOKUpURL( BrokerID, SessionID, NewURL, IsCM )== TRUE )
        {
            NewLine.Printf("%s,%s,%s,%s,%s,%s,%s\n",Fields[0].c_str(),Fields[1].c_str(),Fields[2].c_str(),Fields[3].c_str(),Fields[4].c_str(),Fields[5].c_str(),NewURL.c_str());
            return TRUE;
        }
        else
        {
            UFC::BufferedLog::Printf( " Can't find FCM[%s] SessionID[%d] in P07", BrokerID.c_str(), SessionID);
            NewLine = Line;
            return TRUE;
        }
    }    
    return FALSE;
}
/*--------------------------------------------------------------------------------------------------------------------
 * X.25 Format:
 * Link,PVC,APCode,BrokerID,Password,CM( CM not required )
 *
 * TMP Format:
 * Port,SessionID,APCode,BrokerID,Password,CM( CM not required ),URL
 *
 *--------------------------------------------------------------------------------------------------------------------*/
int TFXSessions::ParseLine( char* Line, int APCode, int Index  )
{
    if( strlen( Line ) < 18 )
        return 0;
    if( strstr( Line, "#") != NULL )
        return 0;
    char  Seps[] = ",\n";
    char* Link   = strtok( Line, Seps );
    char* PVC    = strtok( NULL, Seps );
    char* APC    = strtok( NULL, Seps );
    char* Broker = strtok( NULL, Seps );
    char* Pwd    = strtok( NULL, Seps );
    char* CM     = strtok( NULL, Seps );
    char* URL    = strtok( NULL, Seps );

    if( Link!= NULL && PVC != NULL && APC != NULL && Broker != NULL && Pwd != NULL && APCode == atoi(APC) )
    {
        strncpy( XTCS_BROKER_IDs[ Index ], Broker, 7 );
        strncpy( XTCS_PASSWORDs[ Index ], Pwd, 4 );
        XTCS_PVC_NOs[ Index ] = atoi( PVC );
        XTCS_LINK_NOs[ Index ] = atoi( Link );
        if( CM != NULL )///< Assign CM ID
        {
            if( strlen( CM ) == 4 ) ///< Length = 4, It's CM ID.
            {
                strncpy( XTCS_CMs[ Index ], CM, 4 );
                if( URL != NULL )
                    strcpy( XTCS_URLs[ Index ], URL );                    
                else
                    strcpy( XTCS_URLs[ Index ], "NULL" );
            }
            else ///< No, It's URL.(Assign URL, but not CMID )
            {
                XTCS_CMs[ Index ][0] = 0;
                strcpy( XTCS_URLs[ Index ], CM );                
            }
        }
        else ///< Not assign CM and not TMP session.
        {            
            XTCS_CMs[ Index ][0] = 0;
            strcpy( XTCS_URLs[ Index ], "NULL" );
        }
        return 1;
    }
    return 0;
}
/*--------------------------------------------------------------------------------------------------------------------*/
int TFXSessions::GetLines( int AP )
{
    int   FetchCount = 0;
    char  Buffer[ 512 ];
    FILE* FFileHandle;
            
    if( (FFileHandle = fopen( FFileName, "r" )) == NULL ) 
    {
        UFC::BufferedLog::Printf( " Can't open PVC configuration file \"%s\".", FFileName );
        exit( 0 );
    }
    while( fgets( Buffer, 512, FFileHandle ) != NULL ) 
    {
        int Len = strlen( Buffer );
        
        for( int i = Len - 1; i >= 0; i -- )
        {
            if( *(Buffer + i) == 10 || *(Buffer + i) == 13 )
                *(Buffer + i) = 0;
            else
                break;
        }
        if( ParseLine( Buffer, AP, FetchCount ) == 1 )
            FetchCount++;
    }
    fclose( FFileHandle );
    return FetchCount;
}
/*--------------------------------------------------------------------------------------------------------------------*/
char* TFXSessions::GetCM( int Index )
{
    return XTCS_CMs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/
char* TFXSessions::GetBrokerID( int Index )
{
    return XTCS_BROKER_IDs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/
char* TFXSessions::GetLinePassword( int Index )
{
    return XTCS_PASSWORDs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/
char* TFXSessions::GetURL( int Index )
{
    return XTCS_URLs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/
int TFXSessions::GetPVC( int Index )
{
    return XTCS_PVC_NOs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/
int TFXSessions::GetLink( int Index )
{
    return XTCS_LINK_NOs[Index];
}
/*--------------------------------------------------------------------------------------------------------------------*/




