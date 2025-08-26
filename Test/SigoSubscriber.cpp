#ifndef _WIN32
    #ident "@(#) $Id: SigoSubscriber.cpp,v 1.6 2004/05/21 10:34:18 ap27 Exp $"
    #include <signal.h>
#endif
#include "../Sigo/Sigo.h"

//----------------------------------------------------------------------------------------------------------------------
class TestListener : public MessageListener
{
public:
	virtual void OnMigoMessage( AnsiString Subject, AnsiString Key, MTree* Data )
	{
		MTreeEnumerator* pTreeEnum = Data->newEnumerator();
		MNode* pNode;

		printf("--------------------------------------------------------------------\n");
		printf("Subject:%s\n",Subject.c_str());
		printf("Key:%s\n",Key.c_str());
		pTreeEnum->reset();
        void* pItem = NULL;
		while( (pNode = (MTree*)pTreeEnum->next()) != NULL )
		{
            MString   mName = pNode->getName() ;
		    MDataType mType = pNode->getType() ;
		    MNode   mNode ;
            if ( Data->get( mName, mNode ) == TRUE )
    		{
                pItem = (void*)mNode.getData();
                switch( mType )
			    {
					case M_STRING:
						  printf( "STRING\t[%s]\n", (char*)pItem ) ;
						  break ;
					case M_BOOLEAN:
						  printf( "BOOLEAN\t[%s]\n", (*((int*)pItem)==0?"False":"True") ) ;
						  break ;
					case M_INT:
						  printf( "INT\t[%d]\n", *((int*)pItem) ) ;
						  break ;
					case M_UINT:
                          printf( "UINT\t[%d]\n", *((int*)pItem) ) ;
                          break ;
					case M_SHORT:
                          printf( "SHORT[%d]\n", *((short*)pItem) ) ;
                          break ;
					case M_USHORT:
                          printf( "USHORT[%d]\n", *((short*)pItem) ) ;
                          break ;
					case M_LONG:
                          printf( "LONG\t[%ld]\n", *((long*)pItem) ) ;
                          break ;
					case M_ULONG:
                          printf( "ULONG\t[%ld]\n", *((long*)pItem) ) ;
                          break ;
					case M_FLOAT:
                          printf( "FLOAT\t[%f]\n", *((float*)pItem) ) ;
                          break ;
					case M_DOUBLE:
                          printf( "DOUBLE\t[%f]\n", *((double*)pItem) ) ;
                          break ;
			        default:
                          printf( "Unknown Data Type.\n" ) ;
                          break ;
			    } // switch
                pItem = NULL;
			}
		}
	    printf("--------------------------------------------------------------------\n");
	}

};
//--------------------------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{

      if( argc != 3 && argc != 1 )
      {
              printf( "Usage: SigoSubscriber <Subject> <Key>\n" );
              printf( "       to listen message from the geven subject and key.\n" );
              printf( "Usage: SigoSubscriber\n" );
              printf( "       to listen all messages.\n" );
              return 0;
      }
      else
      {
            MessageObject *Sigo;

            MApp::SetMAppPort( 12345 );
            Sigo = new MessageObject("SigoSubscriber","1.0","Copyright MDBS");
            if( argc == 1 )
                Sigo->AddListener( "all", "all", new TestListener() );
            else
                Sigo->AddListener( argv[1], argv[2], new TestListener() );
            Sigo->Start();
            while( TRUE )
            {
                sleep( 1 );
                fflush( stdout );
            }
      }
}
//--------------------------------------------------------------------------------------------------------------------

