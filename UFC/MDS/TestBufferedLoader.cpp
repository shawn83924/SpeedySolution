//------------------------------------------------------------------------------------------------------------------------
#include "BufferedLoader.h"
#include "../UFC/Utility.h"
//------------------------------------------------------------------------------------------------------------------------
using namespace MDS;
//------------------------------------------------------------------------------------------------------------------------
void Task_Fgets( int argc, char * argv[] )
{
	FILE* FHnd;
	if ( ( FHnd = fopen( argv[2], "r" ) ) == NULL )
	{
		printf( "Open %s failed!!\n", argv[2] );
		exit( 0 );
	}
	BOOL IsTurnOnDataMsg = atoi( argv[4] );
	BOOL IsTurnOnMsg     = atoi( argv[5] );

	char Buffer[512+1];
	int BeginMs = time_ms();
	while( fgets( Buffer, 512, FHnd ) != NULL )
	{
		UFC::AnsiString BufferStr( Buffer );
		BufferStr.TrimRight();
		if ( IsTurnOnDataMsg )
			printf( "%s", Buffer );
	}
	if ( IsTurnOnMsg )
        printf( "\nRead %s EOF!!\n", argv[2] );
	if ( IsTurnOnMsg )
        printf( "Use %d ms.\n", time_ms() - BeginMs );
}
//------------------------------------------------------------------------------------------------------------------------
void Task_BL_Read( int argc, char * argv[] )
{
	BufferedLoader* BLoad = new BufferedLoader( atoi(argv[3]) );
	if ( BLoad->Open( argv[2] ) == FALSE )
	{
		printf( "Open %s failed!!\n", argv[2] );
		exit( 0 );
	}
	BOOL IsTurnOnDataMsg = atoi( argv[4] );
	BOOL IsTurnOnMsg     = atoi( argv[5] );

	char Buffer[512+1];
	int n;
	int BeginMs = time_ms();
	while( TRUE )
	{
		n = BLoad->Read( Buffer, 512 );
		if ( n < 0 )
		{
			if ( IsTurnOnMsg )
                printf( "\nRead %s failed!!\n", argv[2] );
			exit( 0 );
		}
		if ( n == 0 )
		{
			if ( IsTurnOnMsg )
                printf( "\nRead %s EOF!!\n", argv[2] );
			break;
		}
		if ( IsTurnOnDataMsg )
		{
			Buffer[n] = 0;
			printf( "%s", Buffer );
		}
	}
	if ( IsTurnOnMsg )
        printf( "Use %d ms.\n", time_ms() - BeginMs );
}
//------------------------------------------------------------------------------------------------------------------------
void Task_BL_ReadLine( int argc, char * argv[] )
{
	BufferedLoader* BLoad = new BufferedLoader( atoi(argv[3]) );
	if ( BLoad->Open( argv[2] ) == FALSE )
	{
		printf( "Open %s failed!!\n", argv[2] );
		exit( 0 );
	}
	BOOL IsTurnOnDataMsg = atoi( argv[4] );
	BOOL IsTurnOnMsg     = atoi( argv[5] );

	char Buffer[512+1];
	int n;
	int BeginMs = time_ms();
	while( TRUE )
	{
		n = BLoad->ReadLine( Buffer, 512 );
		if ( n < 0 )
		{
			if ( IsTurnOnMsg )
                printf( "\nRead %s failed!!\n", argv[2] );
			exit( 0 );
		}
		if ( n == 0 )
		{
			if ( IsTurnOnMsg )
                printf( "\nRead %s EOF!!\n", argv[2] );
			break;
		}
		if ( IsTurnOnDataMsg )
		{
			Buffer[n] = 0;
			printf( "%s", Buffer );
		}
	}
	if ( IsTurnOnMsg )
        printf( "Use %d ms.\n", time_ms() - BeginMs );
}
//------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
	if ( argc < 4 )
	{
		printf( "Usage:%s <Type> <FileName> <BufferSiezeBytes> <IsTurnOnDataMsg> <IsTurnOnMsg>\n", argv[0] );
		printf( "  Type -> 1 : fgets()\n" );
		printf( "          2 : BufferedLoader.Read()\n" );
		printf( "          3 : BufferedLoader.ReadLine\n" );
		exit( 0 );
	}
	switch( atoi(argv[1]) )
	{
	case 1:
		Task_Fgets( argc, argv );
		break;
	case 2:
		Task_BL_Read( argc, argv );
		break;
	case 3:
		Task_BL_ReadLine( argc, argv );
		break;
	default :
		printf( "Type error!!\n" );
	}
}
//------------------------------------------------------------------------------------------------------------------------
