#include "OrcTFXMapper.h"
#include <stdio.h>
//----------------------------------------------------------------------------------------------------
const char* ERROR_MESSAGE_FILE = "../cfg/TSCTFXErrorMessage.csv";
//----------------------------------------------------------------------------------------------------
OrcTFXMapper::OrcTFXMapper( const UFC::AnsiString& MapperName, OrderFactory* Factory,NIDRule* Rule, BOOL Recover )
:FIXMapper( MapperName, Factory, Rule, Recover )
,ErrorMessage( ERROR_MESSAGE_FILE )
{
    ReadAccountList();
}
//----------------------------------------------------------------------------------------------------
void OrcTFXMapper::ReadAccountList( void )
{
    UFC::AnsiString    AccountFile ( "../cfg/ORCTFXAccount.txt");
    UFC::AnsiString    Account, BrokerID, TWebID;
    char               Buffer[ 512 ] = "";
    char               Seps[] = ",";
    FILE*              FileHandle;

    if( (FileHandle = fopen( AccountFile.c_str() , "r" )) == NULL )
    {
        UFC::BufferedLog::Printf( " Can not open file %s", AccountFile.c_str());
        exit( 0 );
    }

    try
    {
        while( fgets( Buffer, 512, FileHandle ) != NULL )
        {
            Account = strtok( Buffer, Seps );
            BrokerID = strtok( NULL, Seps );
            TWebID = strtok( NULL, Seps );
            TWebID.TrimRight();
            UFC::BufferedLog::DebugPrintf(" Account:[%s] ==> BrokerID:[%s], WebID:[%s].  ",  Account.c_str(), BrokerID.c_str(), TWebID.c_str());
            AccountAttribute* AccountAttr = new AccountAttribute( Account, BrokerID, TWebID );
            FAccountList.Add( Account, AccountAttr );
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" Pleass check the account file.");
        exit(0);
    }
    fclose( FileHandle );
}
//----------------------------------------------------------------------------------------------------
AccountAttribute::AccountAttribute( UFC::AnsiString TAccount, UFC::AnsiString TBrokerID, UFC::AnsiString TWebID ):
Account( TAccount ),
FBrokerID( TBrokerID ),
FWebID( TWebID )
{}
//----------------------------------------------------------------------------------------------------
