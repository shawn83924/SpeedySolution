#define __OSFUNC_CPP__

///#pragma warning (disable:4530 4786)

//#include "tchars.h"
#include "osfunc.h"
#include <list>
#include <string>
#include <algorithm>
#include <cctype>

#include <time.h>
#include <sys/timeb.h>

#if defined(_WIN32) && !defined(_XBOX)
#include <direct.h>
#include "cslock.h"
#endif

//=========================================
// overrided functions for XBOX
//=========================================

#ifdef _XBOX

#include <xtl.h>
#include <stdio.h>

extern "C"
{

static std::string XRootDir;

// Get current working directory
char *getcwd(char *buf, unsigned int buf_size)
{
	strncpy(buf, XRootDir.c_str(), buf_size);
	return buf;
}

// Set current working directory
int chdir(const char *dirname)
{
    size_t pos = 0;
    std::string dir = dirname;

    while( (pos=dir.find("\\", pos))!=dir.npos)  // correct the '\' char
	    dir.replace(pos, 1, "/");
		 
    // determine which type this dirname is
    if(dir[0]=='/' || dir.find(':')!=dir.npos)
	{
		// absolute dir
		XRootDir = dir;
	}
	else if(dir.size()!=0)
	{
        // relative dir
		XRootDir += ("/" + dir);
	}

	return 0;
}

// Our file open routine for XBOX
void *Xfopen(const char* name, const char* access)
{
	std::string path = name;
	size_t pos = 0;

	while( (pos=path.find("\\", pos))!=path.npos)  // correct the '\' char
	    path.replace(pos, 1, "/");

	pos = 0;
	while( (pos=path.find("./", pos))!=path.npos)
		path.erase(pos, 2);

    return fopen((XRootDir+"/"+path).c_str(), access);
}

}

//=========================================
// overrided functions for PS2
//=========================================

#elif SN_TARGET_PS2     

#include <stdio.h>
#include <unistd.h>
#include <eeregs.h>
#include "skyfs.h"

extern "C"
{

// Get current working directory
char *getcwd(char *buf, unsigned int buf_size)
{
	strncpy(buf, SkyGetRootDir(), buf_size);
	return buf;
}

// Set current working directory
int chdir(const char *dirname)
{
    unsigned int pos=0;
    std::string dir = dirname;
		 
    // determine which type this dirname is
    if(dir[0]=='/' || dir[0]=='\\' || dir.find(':')!=dir.npos)
	{
		 // absolute dir
		SkySetRootDir(dir.c_str());
	}
	else if(dir.size()!=0)
	{
		std::string old_dir = SkyGetRootDir();

		if( old_dir.size()!=0 && (old_dir[old_dir.size()-1]=='/'
		   || old_dir[old_dir.size()-1]=='\\') )
        {
		    SkySetRootDir((old_dir + dir).c_str()); // relative dir, not append '/'
		}
		else
		{
		    SkySetRootDir((old_dir + "/" + dir).c_str()); // relative dir, add '/'
		}
	}

	return 0;
}

// Variables for PS2 timer
static volatile unsigned long ps2TimerHighCount = 0;
static int                    ps2TimerHandlerHid = -1;

// Timer handler
static int ps2TimerHandler(int ca)
{
    if ((ca == INTC_TIM0) && (*T0_MODE & 0x800))
    {
        *T0_MODE |= 0x800;
        ps2TimerHighCount += 0x10000;
    }

    ExitHandler(); // This is a TRC requirement at the moment 
    return 0;
}

// Our PS2 timer
unsigned int get_system_tick()
{
    if(ps2TimerHandlerHid==-1) // not init yet, so set it up
	{
       ps2TimerHandlerHid = AddIntcHandler(INTC_TIM0, ps2TimerHandler, 0);
    
       ps2TimerHighCount = 0;
       *T0_COUNT = 0;
       *T0_COMP = 0;
       *T0_HOLD = 0;
       *T0_MODE = 0x281;
       EnableIntc(INTC_TIM0);
	}

    unsigned long  high0, low0, high1, low1;

    high0 = ps2TimerHighCount;
    low0 = *T0_COUNT;
    high1 = ps2TimerHighCount;
    low1 = *T0_COUNT;

    if (high0 == high1)
    {
        return ((unsigned int) ((high0 | (low0 & 0xffff)) / 9216));
    }
    else
    {
        return ((unsigned int) ((high1 | (low1 & 0xffff)) / 9216));
    }
}


}  // end of extern "C"

#elif defined(__linux__) || defined(__APPLE__)

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#if defined(__APPLE__)

#include <stdint.h> 
#include <mach/mach_time.h>  

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
unsigned int get_system_tick()
{ 
	static double conversion = 0.0;
	
	if( conversion == 0.0 )
	{ 
		mach_timebase_info_data_t info; 
		if(mach_timebase_info( &info ) == 0) 
		{
			conversion= 1e-6 * (double) info.numer / (double) info.denom; 
		}
	} 
	
	return mach_absolute_time() * conversion; 
}

#else

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
unsigned int get_system_tick()
{
	//timespec time;
	//clock_gettime(CLOCK_MONOTONIC, &time);
    //return (time.tv_sec * 1000) + (time.tv_nsec / 1000000);
    
    struct timeb  t;

    ftime( &t );
    return t.time*1000 + t.millitm;
    
}

#endif

/**
 * @fn unsigned int GetCurrentThreadId()
 * The system timer which can retrieve the current thread id.
 */
unsigned int GetCurrentThreadId()
{
	pthread_t id = pthread_self();
    return (size_t)id;
}

#endif

/////////////////////////////////////////////////////////////////////////
// Interface for default file system
////////////////////////////////////////////////////////////////////////
void *IFileSystem::Fopen(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

long IFileSystem::Ftell(void *stream)
{
	return ftell((FILE *)stream);
}

int IFileSystem::Fseek(void *stream, long offset, int origin)
{
	return fseek((FILE *)stream, offset, origin);
}

size_t IFileSystem::Fread(void *buffer, size_t size, size_t count, void *stream)
{
	return fread(buffer, size, count, (FILE *)stream);
}

size_t IFileSystem::Fwrite(const void *buffer, size_t size, size_t count, void *stream)
{
	return fwrite(buffer, size, count, (FILE *)stream);
}

int IFileSystem::Fgetc(void *stream)
{
	return fgetc((FILE *)stream);
}

char *IFileSystem::Fgets(char *string, int n, void *stream)
{
	return fgets(string, n, (FILE *)stream);
}

int IFileSystem::Fputs(const char *string, void *stream)
{
	return fputs(string, (FILE *)stream);
}

int IFileSystem::Feof(void *stream)
{
	return feof((FILE *)stream);
}

int IFileSystem::Fclose(void *stream)
{
	return fclose((FILE *)stream);
}

int IFileSystem::Chdir(const char *dirname)
{
#if defined(_MSC_VER)
	return _chdir(dirname);
#else
	return chdir(dirname);
#endif
}

char *IFileSystem::Getcwd(char *buffer, int maxlen)
{
#if defined(_MSC_VER)
	return _getcwd(buffer, maxlen);
#else
	return getcwd(buffer, maxlen);
#endif
}

// Check if work directory supports multi-thread
bool IFileSystem::IsDirMT() const
{
	return false;
}

// global IFileSystem instance
static IFileSystem g_DefaultFileSystem;
static IFileSystem *g_pFileSystem = &g_DefaultFileSystem;

// Get the current file system
IFileSystem *IFileSystem::GetInstance()
{
	return g_pFileSystem;
}

// Install custom file system
void IFileSystem::Install(IFileSystem *pNewFS)
{
	g_pFileSystem = pNewFS;
}

// Uninstall custom file system, i.e. restore to default FS
void IFileSystem::Uninstall()
{
	g_pFileSystem = &g_DefaultFileSystem;
}

//////////////////////////////////////////////////////////////////////////////////////
// Directory functions
//////////////////////////////////////////////////////////////////////////////////////
// Global directory stack
#include <map>
#include "cslock.h"

typedef std::list<std::string> DIRSTACK;
static std::map<unsigned int, DIRSTACK> g_dirstack; // thread_id + DIRSTACK
static CS_Lock g_dirstack_lock;

// Extract path into pathname part and filename part
void extract_path(const std::string &path, std::string &pathname, std::string &filename)
{
   size_t i = path.rfind('\\');
   size_t j = path.rfind('/');

   if(i==path.npos && j==path.npos)
   {
      pathname = "";
      filename = path;
      return;
   }
   else if(i==path.npos) i=j;
   else if(j==path.npos) j=i;
   else if(i<j) i = j;

   filename.assign(path, i+1, path.size());
   pathname.assign(path, 0, i);
}

// Make path to be relative to another path
std::string relative_path(const std::string &to_path, const std::string &from_path)
{
	size_t i, pos;
	std::string p1 = to_path;
	std::string p2 = from_path;
	std::string output;

	// convert to lower case
    std::transform(p1.begin(), p1.end(), p1.begin(), tolower);
	std::transform(p2.begin(), p2.end(), p2.begin(), tolower);

    // correct the '\' char
	pos = 0;
	while((pos = p1.find("\\", pos))!= p1.npos) p1.replace(pos, 1, "/");
	pos = 0;
	while((pos = p2.find("\\", pos))!= p2.npos) p2.replace(pos, 1, "/");
	
	// force to append '/' at the end of path
	if(p1.size() > 0 && p1[p1.size()-1] != '\\' && p1[p1.size()-1] != '/') p1 += '/';
	if(p2.size() > 0 && p2[p2.size()-1] != '\\' && p2[p2.size()-1] != '/') p2 += '/';

	// compare the p1 & p2 until not match
	for(i=0; i< p2.size(); i++)
	{
		if(p2[i]!=p1[i])
		{
			// find the head of directory's name
			while(i > 0)
			{
				if(p2[i-1] == '/') break;
				i--;
			}
			
			break;
		}
	}

	// check if need to pop
	if(i< p2.size())
	{
		pos = i;
		while((pos = p2.find("/", pos))!= p2.npos)
		{
			output += "../";
			pos ++;
		}
	}

	// check if need to push
	if(i< p1.size())
	{
		while((pos = p1.find("/", i))!= p1.npos)
		{
			int count = pos - i;
			if(count > 0) output.append(p1, i, count + 1);
			i = pos + 1;
		}
	}

	return output;
}

// Push current working directory into stack
void push_cwd()
{
	g_dirstack_lock.Lock();
	DIRSTACK &stack = g_dirstack[GetCurrentThreadId()];
	
	if(IFileSystem::GetInstance()->IsDirMT())
	{
		g_dirstack_lock.Unlock();
	}

	char dir[1024];
	IFileSystem::GetInstance()->Getcwd(dir, sizeof(dir));
	stack.push_back(std::string(dir));
}

// Pop directory as current working directory
void pop_cwd()
{
	if(IFileSystem::GetInstance()->IsDirMT())
	{
		g_dirstack_lock.Lock();
	}

	DIRSTACK &stack = g_dirstack[GetCurrentThreadId()];
	g_dirstack_lock.Unlock();

	if(stack.size()!=0)
	{
		IFileSystem::GetInstance()->Chdir(stack.back().c_str());
		stack.pop_back();
	}
}

// Set current working directory by path. It return filename part if the path includes
std::string set_cwd(const std::string &path)
{
    std::string pathname, filename;
	extract_path(path, pathname, filename);
	IFileSystem::GetInstance()->Chdir(pathname.c_str()); 
	return filename;
}

// Get current working directory
std::string get_cwd()
{
	char dir[1024];

	IFileSystem::GetInstance()->Getcwd(dir, sizeof(dir));
	int slen = (int)strlen(dir);

	// force to append '/' at the end of path
	if(slen > 0 && dir[slen-1] != '\\' && dir[slen-1] != '/')
	{
		dir[slen] = '/';
		dir[slen+1] = 0;
	}

	return dir;
}


// Make relative full pathname from current working directory
std::string relative_fullpath(const std::string &fullpath)
{
	std::string pathname, filename;
	extract_path(fullpath, pathname, filename);

    if(pathname=="")
        return filename;

	return relative_path(pathname, get_cwd()) + filename;
}

