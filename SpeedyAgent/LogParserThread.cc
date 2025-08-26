#include "SpeedyAgent.h"
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString  CurrentDir;
//----------------------------------------------------------------------------------------------------------------------
//
// Class ParseLogThread
// Thread to parse log for a specific connection.
//
//----------------------------------------------------------------------------------------------------------------------
ParseLogThread::ParseLogThread( MTree* Data, const UFC::AnsiString& ReplyKey )
:UFC::PThread( NULL )
,FReplyKey( ReplyKey )
,FEnoughInfo( FALSE )
,FTIME_STR_LEN( 12 )
,FLINE_PREFIX_LEN( 16 )
{
    if( UFC::GLog_us == TRUE )
        FTIME_STR_LEN = 15;    
    if( Data->get( "LINK", FLink ) && 
        Data->get( "PVC", FPVC ) && 
        Data->get( "PVCID",FPVCID ) &&
        Data->get( "CMPSTR",FCmpStr ) &&
        Data->get( "SYS", FSYS) &&  
        Data->get( "AP", FAP))    
    {
        UFC::AnsiString PreFix;        
        
        if( FSYS == 0 )
            PreFix.Printf( "%s/../log/FUT.x", CurrentDir.c_str() );
        else if( FSYS == 1 )
            PreFix.Printf( "%s/../log/OPT.x", CurrentDir.c_str() );
        else if( FSYS == 2 )
            PreFix.Printf( "%s/../log/TSE.x", CurrentDir.c_str() );
        else
            PreFix.Printf( "%s/../log/OTC.x", CurrentDir.c_str() );
        if( FSYS == 0 || FSYS == 1 ) ///< TAIFEX
        {
            switch( FAP )
            {
                case 1: FLogFile = PreFix + UFC::GetDateString()+".1.2"; break;            	
                case 2: FLogFile = PreFix + UFC::GetDateString()+".1.2";break;
                case 3:
                case 8:
                case 9: FLogFile = PreFix + UFC::GetDateString()+".3.8.9";break;	            
                case 4: FLogFile = PreFix + UFC::GetDateString()+".4";break;
                case 10:FLogFile = PreFix + UFC::GetDateString()+".10"; break;
                default:FLogFile = PreFix + UFC::GetDateString()+".4";break;
            }
        }
        else ///< TWSE/OTC
        {
            switch( FAP )
            {                       
                case 1:  
                case 2: FLogFile = PreFix + UFC::GetDateString()+".1.2"; break;	          
                case 3: FLogFile = PreFix + UFC::GetDateString()+".3"; break;
                case 0:	
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9: 
                case 10: FLogFile = PreFix + UFC::GetDateString()+".4";break;
                default: FLogFile = PreFix + UFC::GetDateString()+".4";break;
            }	    	
        }
        FEnoughInfo = TRUE;
    }
    Start();
}
//----------------------------------------------------------------------------------------------------------------------
void ParseLogThread::Execute()
{
    MTree           ReplyData;
    FILE*           FFileHandle;        
    
    if( FEnoughInfo == FALSE )
        return;        
    UFC::BufferedLog::Printf( " Begin parse log for [%s]",FCmpStr.c_str()  );
    if( (FFileHandle = fopen( FLogFile.c_str(), "r" )) != NULL  )///< Open log file
    {
        UFC::MemoryStream LogStream;
        char              Buffer[ MAX_LINE_BUFFER ];
        long              FileSize;        
        Int32             LineCount = 0;
    
        fseek( FFileHandle, 0, SEEK_END );
        FileSize = ftell( FFileHandle ); ///< Get the size of the log file .
        if( FileSize > MAX_LOG_SIZE )  
        {
            fseek( FFileHandle, FileSize - MAX_LOG_SIZE, SEEK_SET );
            fgets( Buffer, MAX_LINE_BUFFER, FFileHandle );
        }
        else
            fseek( FFileHandle, 0, SEEK_SET );
        ///< Get all log lines of this connection and save to the memory stream.
        while( fgets( Buffer, MAX_LINE_BUFFER, FFileHandle ) != NULL )
        {
            if( memcmp( Buffer + FTIME_STR_LEN , FCmpStr.c_str(), FLINE_PREFIX_LEN ) == 0) ///< Match this Line
            {
                strcpy( Buffer + FTIME_STR_LEN, Buffer + FTIME_STR_LEN + FLINE_PREFIX_LEN );
                LogStream.Write( Buffer, (Int32)strlen(Buffer));
                LineCount++;
            }
        }
        fclose( FFileHandle );
        UFC::BufferedLog::Printf( " Parse log for [%s] finished,%d lines.",FCmpStr.c_str(), LineCount );
        ReplyData.append( "COMMAND", SPEEDY_REPLY_PVC_LOG );
        if( LogStream.GetSize() > 0 )
            ReplyData.append( "LOGDATA", (unsigned char*)LogStream.GetBuffer(), LogStream.GetSize());                  
        else
            ReplyData.append( "LOGDATA", (unsigned char*)"No log for this PVC", strlen("No log for this PVC") + 1 );                  
        MessageObj->Send( "SPEEDY.AGENT", FReplyKey, ReplyData, FALSE );
        return;
    }
    UFC::BufferedLog::Printf( " Open log file [%s] failed.",FLogFile.c_str()  );
}
//----------------------------------------------------------------------------------------------------------------------
