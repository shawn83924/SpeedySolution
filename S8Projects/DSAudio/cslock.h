///////////////////////////////////////////////////////////////
// File Name: CSLock.h
//
// Author : Little Chio, Tsaio, CakeChen
// Last Modified : 2003/8/11
// Last Modified : 2007/10/31
//
// Copyright (c) 2003-2007, Gamania Inc. All Rights Reserved.
///////////////////////////////////////////////////////////////

#ifndef __CSLOCK_H__
#define __CSLOCK_H__

#if defined(_WIN32)
#include <windows.h>

/**
//////////////////////////////////////////////////////////////////////////////////
// CS_Lock : A critical-session object for thread-safe.
//////////////////////////////////////////////////////////////////////////////////
*/
class CS_Lock
{
public:

	/// Constructor
    CS_Lock(DWORD timeOutTime = 0)
	{
		m_pCS = new CRITICAL_SECTION_EX;
		m_TimeOutTime = timeOutTime;
	}

	/// Copy constructor
	CS_Lock(CS_Lock &rhs)
	{
		m_pCS = rhs.m_pCS;
		m_pCS->AddRef();
		m_TimeOutTime = rhs.m_TimeOutTime;
	}

	/// Destructor
    ~CS_Lock()
	{
		if(m_bCallStackEnabled) PopCallStack(m_pCS); // prevent zombie in callstack
		m_pCS->Release();
	}

	/// Copy operator
	CS_Lock &operator=(CS_Lock &rhs)
	{
		if(this != &rhs)
		{
			m_pCS->Release();
			m_pCS = rhs.m_pCS;
			m_pCS->AddRef();
			m_TimeOutTime = rhs.m_TimeOutTime;
		}
			
		return *this;
	}
    
	/// Lock the session
	void Lock()
	{
		if(m_bCallStackEnabled) PushCallStack(m_pCS, __FILE__, __LINE__);
		TryToEnterCriticalSection(__FILE__, __LINE__);
	}

	/// Lock the session with debug info
	void Lock(const char *filename, int line)
	{
		if(m_bCallStackEnabled) PushCallStack(m_pCS, filename, line);
		TryToEnterCriticalSection(filename, line);
	}

	/// Unlock the session
    void Unlock()
	{ 
		if(m_TimeOutTime == 0)
		{
			LeaveCriticalSection(m_pCS);
		}
		else
		{
			if((DWORD)m_pCS->OwningThread == GetCurrentThreadId())
			{
				LeaveCriticalSection(m_pCS);
			}
		}

		if(m_bCallStackEnabled) PopCallStack(m_pCS);
	}

	/// Initialize call stacks and enable/disable tracing CS_call
	static void InitializeCallStacks(bool bEnabled);

	/// Deinitialize call stacks
	static void DeinitializeCallStacks();

	/// Dump the call stacks to file
	static bool DumpCallStacks(const char *filename);
   	
private:

	// Try to enter critical section
	void TryToEnterCriticalSection(const char *filename, int line);

	// Push a lock call to stack
	static void PushCallStack(CRITICAL_SECTION *cs, const char *filename, int line);

	// Pop a lock call from stack
	static bool PopCallStack(CRITICAL_SECTION *cs);

	// Critical Section wrapper class
	struct CRITICAL_SECTION_EX : public CRITICAL_SECTION
	{
		CRITICAL_SECTION_EX()
		{
			m_RefCount = 1;
			InitializeCriticalSection(this);
		}
		
		void AddRef()
		{
			InterlockedIncrement(&m_RefCount);
		}
		
		void Release()
		{
			LONG count = InterlockedDecrement(&m_RefCount);
			if(count == 0)
			{
				DeleteCriticalSection(this);
				delete this;
			}
		}

		LONG m_RefCount;
	};


    CRITICAL_SECTION_EX *m_pCS;
	DWORD m_TimeOutTime;
	static bool m_bCallStackEnabled;
};

#elif defined(__GNUC__)

#include <pthread.h>

class CS_Lock
{
public:
    
	/// Constructor
    CS_Lock()
	{
		m_pCS = new CRITICAL_SECTION_EX;
	}
    
	/// Copy constructor
	CS_Lock(CS_Lock &rhs)
	{
		m_pCS = rhs.m_pCS;
		m_pCS->AddRef();
	}
    
	/// Destructor
    ~CS_Lock()
	{
		m_pCS->Release();
	}
    
	/// Copy operator
	CS_Lock &operator=(CS_Lock &rhs)
	{
		if(this != &rhs)
		{
			m_pCS->Release();
			m_pCS = rhs.m_pCS;
			m_pCS->AddRef();
		}
        
		return *this;
	}
    
	/// Lock the session
	void Lock()
	{
        pthread_mutex_lock(&m_pCS->m_Mutex);
	}
    
	/// Lock the session with debug info
	void Lock(const char *filename, int line)
	{
		pthread_mutex_lock(&m_pCS->m_Mutex);
	}
    
	/// Unlock the session
    void Unlock()
	{ 
		pthread_mutex_unlock(&m_pCS->m_Mutex);
	}
    
private:
    

	// Critical Section wrapper class
	struct CRITICAL_SECTION_EX
	{
		CRITICAL_SECTION_EX()
		{
			m_RefCount = 1;
            
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&m_Mutex, &attr);
            pthread_mutexattr_destroy(&attr);
		}
		
		void AddRef()
		{
			m_RefCount++;
		}
		
		void Release()
		{
			int count = --m_RefCount;
			if(count == 0)
			{
				pthread_mutex_destroy(&m_Mutex);
				delete this;
			}
		}
        
		int m_RefCount;
        pthread_mutex_t m_Mutex;
	};
    
        
    CRITICAL_SECTION_EX *m_pCS;
};

#endif


#endif
