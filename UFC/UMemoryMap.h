/******************************************************************************
 * Module   : UMemoryMap.h
 * Author   : Allen Lee
 * Modified : 2005/5/3
 * Purpose  : Unix map files or devices into memory.
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef __UMemoryMap_H
#define __UMemoryMap_H
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "UException.h"
//------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------
// class UMessageQueue
//
//  Exceptions :
//    -UException
//
//  Description :
//    Unix map files or devices into memory.
//    Be careful below-
//    1. The AccessMode for a memory region has to match the open mode of the file.
//       For example, we can't specify Write if the file was opened read-only.
//    2. Two signals are normally used with mapped regions. SIGSEGV is the signal normally
//       used to indicate that we have tried to access memory that is not available to us.
//       It can also be generated if we try to store into a mapped region that we specified to mmap
//       as read-only. The SIGBUS signal can be generated if we access a portion of the mapped
//       region that does not make sense at the time of the access. For example, assume we map
//       a file using the file's size, but before we reference the mapped region the file's size is
//       truncated by some other process. If we then try to access the memory mapped region
//       corresponding to the end portion of the file that was truncated, we'll receive SIGBUS.
//    3. Since the starting offset of the mapped file is tied to the system's virtual memory
//       page size, what happens if the length of the mapped region isn't a multiple of the page
//       size? Assume the file size is 12 bytes and the system's page size is 512 bytes. In this
//       case the system normally provides a mapped region of 512 bytes and the final 500 bytes
//       of this region are set to 0. We can modify the final 500 bytes, but any changes we make
//       to them are not reflected in the file.
//
//	Refernece :
//    MAP_SHARED--
//       Modifications are shared between all processes mapping the same range of the same file, 
//       and changes to the memory region are reflected in the mapped file.
//    MAP_PRIVATE--
//       Modifications are private to the process, and changes to the memory region are not reflected 
//       in the mapped file.
//    MAP_FIXED--
//       Interpret addr exactly.
//    MAP_VARIABLE--
//       Place memory mapped region at an system-computed address.
//    MAP_FILE--
//       Map a regular file, or character special device file /dev/zero.
//    MAP_ANON--
//       Map anonymous memory not associated with any specific file. The fildes parameter should be 
//       specified as -1 with this flag.
//    SIGSEGV--
//    SIGBUS--
//
//------------------------------------------------------------------------------------------------------------------------
class UMemoryMap
{
public:
	typedef enum {
		AutoSync, ManualSync
	} SyncMode;
	typedef enum {
		NonAccessed	= PROT_NONE,			// Page can not be accessed.
		Read = PROT_READ,					// Page can be read.
		Write = PROT_WRITE,					// Page can be written.
		Execute = PROT_EXEC,				// Page can be executed.
		ReadWrite = PROT_READ | PROT_WRITE	// Page can be read & write
	} AccessMode;
private:
	SyncMode	FSyncMode;
	int			FAccessMode;
	char*		FMemoryPtr;
	char*		FWorkPtr;
	UInt32		FMemoryLen;
	bool		FIsMapped;
protected:
	void	CheckFileSizeAndExtendIt(int fd, int len);
public:
	char*	Map(AccessMode access_mode, int fd, UInt32 len);
	void	UnMap(void);
	void	SyncFromFile(void);
	void	SyncToFile(void);
	bool	IsMapped(void);
public:
	UMemoryMap(SyncMode sync_mode);
	~UMemoryMap();
};
//------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
