/******************************************************************************
 * Module   : UMemoryMap.cpp
 * Author   : Allen Lee
 * Modified : 2005/5/3
 * Purpose  : Unix map files or devices into memory.
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include "UMemoryMap.h"
//------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------
UMemoryMap::UMemoryMap(SyncMode sync_mode)
:FSyncMode( sync_mode ),
 FMemoryPtr( NULL ),
 FWorkPtr( NULL ),
 FMemoryLen( 0 ),
 FIsMapped( false )
{
}
//------------------------------------------------------------------------------------------------------------------------
UMemoryMap::~UMemoryMap()
{
	if ( FWorkPtr != NULL )
	{
		delete [] FWorkPtr;
		FWorkPtr = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void UMemoryMap::CheckFileSizeAndExtendIt(int fd, int len)
{
	struct stat statbuf;

	if (fstat(fd, &statbuf) < 0)
		throw UException("fstat()", errno);

	if (len > statbuf.st_size)
	{
		if (lseek(fd, len-1, SEEK_SET) < 0)
			throw UException("lseek()", errno);

		if (write(fd, "", 1) != 1)
			throw UException("write()", errno);
	}
}
//------------------------------------------------------------------------------------------------------------------------
char* UMemoryMap::Map(AccessMode access_mode, int fd, UInt32 len)
{
	int flag;
	char* rtn_ptr;

	if ( FIsMapped )
		throw UException("Memory map has mapped, you should unmap first.");

	// Check file size if is not enough then extend it
	CheckFileSizeAndExtendIt(fd, len);

	FAccessMode = access_mode;
	FMemoryLen = len;

	// Modification mode have MAP_SHARED and MAP_PRIVATE, we use MAP_SHARED only.
	// Interpret mode have MAP_FIXED and MAP_VARIABLE, we use MAP_VARIABLE only.
	// Target mode have MAP_FILE and MAP_ANON, we use MAP_FILE only.
	flag = MAP_SHARED | MAP_VARIABLE | MAP_FILE;

	// char *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
	rtn_ptr = (char*)mmap(0, len, FAccessMode, flag, fd, 0);
	if (rtn_ptr == (void*)-1)
	{
		throw UException("mmap()", errno);
	}
	else
	{
        FMemoryPtr = rtn_ptr;
	}

	// if ManualSync then reallocate FWorkPtr and return FWorkPtr
	if ( FSyncMode == ManualSync )
	{
		if ( FWorkPtr != NULL )
		{
			delete [] FWorkPtr;
			FWorkPtr = NULL;
		}
		FWorkPtr = new char[ len ];

		// keep states & return value
		FIsMapped = true;
		rtn_ptr = FWorkPtr;
	}
	else
	{
		// keep states & return value
		FIsMapped = true;
		rtn_ptr = FMemoryPtr;
	}

	return rtn_ptr;
}
//------------------------------------------------------------------------------------------------------------------------
void UMemoryMap::UnMap()
{
	if (!FIsMapped)
		throw UException("Memory map has not mapped, cann't UnMap.");

	// keep states
	FIsMapped = false;

	// if ManualSync delete FWorkPtr
	if ( (FSyncMode == ManualSync) && (FWorkPtr != NULL) )
	{
		delete [] FWorkPtr;
		FWorkPtr = NULL;
	}

	if ( munmap(FMemoryPtr, FMemoryLen) < 0 )
		throw UException("munmap()", errno);
}
//------------------------------------------------------------------------------------------------------------------------
void UMemoryMap::SyncFromFile(void)
{
	if (!FIsMapped)
		throw UException("Memory map has not mapped, you should map first.");

	if (FSyncMode == ManualSync)
		memcpy(FWorkPtr, FMemoryPtr, FMemoryLen);

	printf("FWorkPtr : %s, FMemoryPtr : %s, FMemoryLen:%d\n", FWorkPtr, FMemoryPtr, FMemoryLen);
}
//------------------------------------------------------------------------------------------------------------------------
void UMemoryMap::SyncToFile(void)
{
	if (!FIsMapped)
		throw UException("Memory map has not mapped, you should map first.");

	if (FSyncMode == ManualSync)
		memcpy(FMemoryPtr, FWorkPtr, FMemoryLen);

	printf("FWorkPtr : %s, FMemoryPtr : %s, FMemoryLen:%d\n", FWorkPtr, FMemoryPtr, FMemoryLen);
}
//------------------------------------------------------------------------------------------------------------------------
bool UMemoryMap::IsMapped(void)
{
	return FIsMapped;
}
//------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
