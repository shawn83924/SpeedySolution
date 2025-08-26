#ifndef _WIN32
    #include <signal.h>
#endif
#pragma link "\\MBus\\lib\\UFC.lib"
#pragma link "\\MBus\\lib\\Migo.lib"
#include "../Migo/Sigo.h"
    
//----------------------------------------------------------------------------------------------------------------------
BOOL STDIO_OFF = FALSE;
//----------------------------------------------------------------------------------------------------------------------
class TestListener : public MessageListener
{
public:
	virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data )
	{
            if( STDIO_OFF == FALSE )
            {
                MTreeEnumerator* pTreeEnum = Data->newEnumerator();
                MNode* pNode;
                void*  pItem;
                UFC::AnsiString NodeName;

                pTreeEnum->reset();
                printf("--------------------------------------------------------------------\n");
                printf("Subject:%s\n",Subject.c_str());
                printf("Key:%s\n",Key.c_str());
                while( (pNode = (MTree*)pTreeEnum->next()) != NULL )
                {
                    MDataType mType = pNode->getType() ;
                    pItem = (void*)pNode->getData();
                    NodeName = pNode->getName();
                    
                    switch( mType )
                    {
                        case M_STRING: printf( "STRING\t[%s=%s]\n", NodeName.c_str(), (char*)pItem ) ;
                                       break;
                        case M_BOOLEAN:printf( "BOOLEAN\t[%s=%s]\n", NodeName.c_str(), (*((int*)pItem)==0?"False":"True") ) ;
                                       break;
                        case M_INT:    printf( "INT\t[%s=%d]\n", NodeName.c_str(), *((int*)pItem) ) ;
                                       break;
                        case M_UINT:   printf( "UINT\t[%s=%d]\n", NodeName.c_str(), *((unsigned int*)pItem) ) ;
                                       break;
                        case M_SHORT:  printf( "SHORT[%s=%d]\n", NodeName.c_str(), *((short*)pItem) ) ;
                                       break;
                        case M_USHORT: printf( "USHORT[%s=%d]\n", NodeName.c_str(), *((unsigned short*)pItem) ) ;
                                       break;
                        case M_LONG:   printf( "LONG\t[%s=%ld]\n", NodeName.c_str(), *((long*)pItem) ) ;
                                       break;
                        case M_ULONG:  printf( "ULONG\t[%s=%ld]\n", NodeName.c_str(), *((unsigned long*)pItem) ) ;
                                       break;
                        case M_FLOAT:  printf( "FLOAT\t[%s=%f]\n", NodeName.c_str(), *((float*)pItem) ) ;
                                       break;
                        case M_DOUBLE: printf( "DOUBLE\t[%s=%lf]\n", NodeName.c_str(), *((double*)pItem) ) ;
                                       break;
                        default:       printf( "Unknown Data Type:%d.\n", mType ) ;
                                       break;
                    } // switch
                }
                printf("--------------------------------------------------------------------\n");
            }
	}

};
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" )
        {
            printf( "Subscriber Copyright (c) 2003,2005 MDBS\n");
            printf( "Usage: Subscriber <Subject> <Key> [-options]\n" );
            printf( "   -H  Show this page.\n" );
            printf( "   -N  Trun of the stdout.\n" );
            exit( 1 );
        }
        else if( Param == "-N" || Param == "-n" )
            STDIO_OFF = TRUE;
    }
}
//--------------------------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    MessageObject *Sigo;

    CheckArgs( argc, argv );
    Sigo = new MessageObject("Subscriber","1.0","Copyright MDBS");
    if( argc < 3 )
        Sigo->AddListener( "all", "all", new TestListener() );
    else
        Sigo->AddListener( argv[1], argv[2], new TestListener() );
    Sigo->Start();
    sleep( 5 );
    while( TRUE )
    {
        sleep( 1 );
    }
}
//--------------------------------------------------------------------------------------------------------------------

