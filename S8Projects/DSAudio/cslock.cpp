//////////////////////////////////////////////////////////
// File Name: cslock.cpp
//
// Author : Little Chio
// File created: 2010/8/26
// Last Modified : 2010/9/28
//
// Copyright (c) 2010, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#pragma warning (disable:4786)

#if defined(_WIN32)

#include "cslock.h"
#include <iostream>
#include <sstream>
#include <list>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////
// CS_Call : A call object when entering the critical section
//////////////////////////////////////////////////////////////////////////////////
struct CS_Call
{
	CRITICAL_SECTION *cs;	// used CS object
	const char *filename;	// cpp file name
	int line;				// line number in cpp
};

//////////////////////////////////////////////////////////////////////////////////
// CS_CallStack : A stack of critical section entering calls for a single thread
//////////////////////////////////////////////////////////////////////////////////
class CS_CallStack
{
public:

	// Assume the max depth of call stack is 32
	enum { MAX_CALL_DEPTH = 32 };

	// Constructor
	CS_CallStack()
	{
		m_ThreadId = 0;
		InitializeCriticalSection(&m_StackCS);
		Clear();
	}

	// Destructor
	~CS_CallStack()
	{
		DeleteCriticalSection(&m_StackCS);
	}

	// Push a call to stack
	bool Push(const CS_Call &call)
	{
		bool retval = false;
		EnterCriticalSection(&m_StackCS);
		
		if(m_LastCallIndex < (MAX_CALL_DEPTH-1))
		{
			m_CallList[++m_LastCallIndex] = call; // push back
			retval = true;
		}

		LeaveCriticalSection(&m_StackCS);
		return retval;
	}

	// Pop a call from stack
	bool Pop(CRITICAL_SECTION *cs)
	{
		bool retval = true;
		EnterCriticalSection(&m_StackCS);

		if(m_LastCallIndex >= 0)
		{
			if(m_CallList[m_LastCallIndex].cs == cs)
			{
				m_LastCallIndex --;  // just pop back
			}
			else
			{
				for(int i=m_LastCallIndex-1; i >= 0; i--)
				{
					if(m_CallList[i].cs == cs)  // search this cs and extract
					{
						memmove(&m_CallList[i], &m_CallList[i+1], sizeof(CS_Call)*(m_LastCallIndex-i));
						m_LastCallIndex --;
						retval = true;
						break;
					}
				}
			}
		}

		LeaveCriticalSection(&m_StackCS);
		return retval;
	}

	// Clear the stack
	void Clear()
	{
		EnterCriticalSection(&m_StackCS);
		memset(m_CallList, 0, sizeof(m_CallList));
		m_LastCallIndex = -1;
		LeaveCriticalSection(&m_StackCS);
	}

	// Export stack info to out stream
	void Export(std::ostream &oss)
	{
		char szMsg[256];

		EnterCriticalSection(&m_StackCS);

		for(int i=m_LastCallIndex; i >= 0; i--)
		{
			CRITICAL_SECTION *cs = m_CallList[i].cs;

			sprintf(szMsg, "CRITICAL_SECTION 0x%08X (OwningThread=%d LockCount=%d RecursionCount=%d)",
				cs, cs->OwningThread, cs->LockCount, cs->RecursionCount);

			oss << szMsg << " call from " << m_CallList[i].filename << ", line " << m_CallList[i].line << "\r\n";
		}

		LeaveCriticalSection(&m_StackCS);
	}

	// Find the last waiting call in the stack
	bool FindLastWaitingCall(CS_Call &call)
	{
		bool retval = false;
		EnterCriticalSection(&m_StackCS);

		if(m_LastCallIndex >= 0)
		{
			if(m_ThreadId != (DWORD)m_CallList[m_LastCallIndex].cs->OwningThread)
			{
				call = m_CallList[m_LastCallIndex];
				retval = true;
			}
		}

		LeaveCriticalSection(&m_StackCS);
		return retval;
	}

	DWORD GetThreadId() const { return m_ThreadId; }

private:

	CS_Call m_CallList[MAX_CALL_DEPTH];
	int m_LastCallIndex;
	CRITICAL_SECTION m_StackCS;
	DWORD m_ThreadId;

	friend class CS_CallStackMap;
};

//////////////////////////////////////////////////////////////////////////////////
// CS_CallStackMap : Map for call stacks for all threads
//////////////////////////////////////////////////////////////////////////////////
class CS_CallStackMap
{
public:

	typedef std::list<CS_Call> CALL_LIST;

	// Assume max thread id is less then 0xFFFF for Win32
	enum { STACK_ARRAY_SIZE = 0xFFFF/4 }; 

	// Constructor
	CS_CallStackMap()
	{
		InitializeCriticalSection(&m_StackMapCS);
	}

	// Destructor
	~CS_CallStackMap()
	{
		if(m_pStackArray != NULL) delete [] m_pStackArray;
		DeleteCriticalSection(&m_StackMapCS);
	}

	// Reset the stack array
	void Reset()
	{
		EnterCriticalSection(&m_StackMapCS);

		if(m_pStackArray == NULL)
		{
			m_pStackArray = new CS_CallStack[STACK_ARRAY_SIZE];
		}

		for(DWORD i=0; i< STACK_ARRAY_SIZE; i++)
		{
			m_pStackArray[i].m_ThreadId = i << 2;
			m_pStackArray[i].Clear();
		}

		LeaveCriticalSection(&m_StackMapCS);
	}

	// Get a stack by the given thread
	CS_CallStack *GetCallStack(DWORD thread_id)
	{
		return &m_pStackArray[thread_id >> 2];
	}

	// Export all stacks' info to out stream
	void Export(std::ostream &oss)
	{
		char szMsg[256];
		int count = 1;

		LockAllStacks();

		for(DWORD i=0; i< STACK_ARRAY_SIZE; i++)
		{
			CS_CallStack *cs = m_pStackArray + i;
			
			if(cs->m_LastCallIndex >= 0)
			{
				sprintf(szMsg, "Call stack #%d [Thread %d]\r\n", count, cs->GetThreadId());
			
				oss << szMsg;
				cs->Export(oss);
				oss << "\r\n";

				count++;
			}
		}

		UnlockAllStacks();
	}

	// Trace the calls from stack to stack for dead-lock detection
	bool TraceDeadLock(CALL_LIST &call_path)
	{
		bool bDeadLockFound = false;
		CS_Call &call = call_path.back();

		CS_CallStack *stack = GetCallStack((DWORD)call.cs->OwningThread);
		CS_Call call2;

		if(stack->FindLastWaitingCall(call2) && call.cs != call2.cs)
		{
			// check if loop path is formed...
			CALL_LIST::iterator iter = call_path.begin();
			while(iter != call_path.end())
			{
				if((*iter).cs == call2.cs)
				{
					bDeadLockFound = true;
					call_path.push_back(call2); // record this call 
					break;
				}
				else iter++;
			}

			if(!bDeadLockFound)
			{
				call_path.push_back(call2);

				if(TraceDeadLock(call_path))
				{
					bDeadLockFound = true;
				}
				else
				{
					call_path.pop_back();
				}
			}
		}

		return bDeadLockFound;
	}
	
	// Analyze the call stacks and output the deadlock call-path
	void AnalyzeDeadlock(std::ostream &oss)
	{
		char szMsg[256];
		bool bDeadlockFound = false;

		LockAllStacks();

		for(DWORD i=0; i< STACK_ARRAY_SIZE; i++)
		{
			CS_Call call;
			CALL_LIST call_path;
			CS_CallStack *stack = m_pStackArray + i;
			
			if(stack->FindLastWaitingCall(call))
			{
				call_path.push_back(call);

				if(TraceDeadLock(call_path))
				{
					bDeadlockFound = true;

					oss << "Deadlock found! Call path: \r\n";
					CALL_LIST::iterator p = call_path.begin();
					while(p != call_path.end())
					{
						CRITICAL_SECTION *cs = (*p).cs;

						sprintf(szMsg, "CRITICAL_SECTION 0x%08X (OwningThread=%d LockCount=%d RecursionCount=%d)",
							cs, cs->OwningThread, cs->LockCount, cs->RecursionCount);

						oss << szMsg << " call from " << (*p).filename << ", line " << (*p).line << "\r\n";

						p ++;
					}

					break;
				}
			}
		}

		UnlockAllStacks();

		if(!bDeadlockFound)
		{
			oss << "Deadlock is not found.\r\n";
		}
	}

	void LockAllStacks()
	{
		EnterCriticalSection(&m_StackMapCS);

		for(DWORD i=0; i< STACK_ARRAY_SIZE; i++)
		{
			EnterCriticalSection(&m_pStackArray[i].m_StackCS);
		}
	}

	void UnlockAllStacks()
	{
		for(DWORD i=0; i< STACK_ARRAY_SIZE; i++)
		{
			LeaveCriticalSection(&m_pStackArray[i].m_StackCS);
		}

		LeaveCriticalSection(&m_StackMapCS);
	}

private:

	CS_CallStack *m_pStackArray;
	CRITICAL_SECTION m_StackMapCS;
};

static CS_CallStackMap g_CSCallStackMap;

//////////////////////////////////////////////////////////////////////////////////
// CS_Lock's member functions
//////////////////////////////////////////////////////////////////////////////////
bool CS_Lock::m_bCallStackEnabled = false;

/// Initialize call stacks and enable/disable tracing CS_call
void CS_Lock::InitializeCallStacks(bool bEnabled)
{
	m_bCallStackEnabled = false;
	g_CSCallStackMap.Reset();
	m_bCallStackEnabled = bEnabled;
}

/// Deinitialize call stacks
void CS_Lock::DeinitializeCallStacks()
{
	m_bCallStackEnabled = false;
}

/// Dump the call stacks to file
bool CS_Lock::DumpCallStacks(const char *filename)
{
	FILE *fp = fopen(filename, "wb");

    if(fp!=NULL)
    {
		std::string p;

		if(m_bCallStackEnabled)
		{
			std::ostringstream outs;
			g_CSCallStackMap.Export(outs);
			g_CSCallStackMap.AnalyzeDeadlock(outs);
			p = outs.str();	// get the full string of content
			
		}
		else
		{
			p = "Call-stack tracing is disabled. No call stack info is dumped.\r\n";
		}

		fwrite(p.c_str(), sizeof(char), p.size(), fp);
        fclose(fp);
        return true;
    }

	return false;
}

// Try to enter critical section
void CS_Lock::TryToEnterCriticalSection(const char *filename, int line)
{
	if(m_TimeOutTime == 0)
	{
		EnterCriticalSection(m_pCS);
	}
	else
	{
		if(TryEnterCriticalSection(m_pCS) == FALSE)
		{
			DWORD last_tick = timeGetTime();
			while(TryEnterCriticalSection(m_pCS) == FALSE)
			{
				if(timeGetTime() - last_tick < m_TimeOutTime)
				{
					Sleep(1);
				}
				else
				{
					printf("CS_Lock::Lock() Try failed at file %s, line %d\r\n", filename, line);
					break;
				}
			}
		}
	}
}

// Push a lock call to stack   	
void CS_Lock::PushCallStack(CRITICAL_SECTION *cs, const char *filename, int line)
{
	CS_Call call = { cs, filename, line };
	bool retval = g_CSCallStackMap.GetCallStack(GetCurrentThreadId())->Push(call);
	if(!retval)
	{
		printf("Invalid CS_Lock::Lock(), the call stack is overflow!\r\n");
	}
}

// Pop a lock call from stack
bool CS_Lock::PopCallStack(CRITICAL_SECTION *cs)
{
	bool retval = g_CSCallStackMap.GetCallStack(GetCurrentThreadId())->Pop(cs);
	if(!retval)
	{
		printf("Invalid CS_Lock::Unlock(), pop from call stack failed!\r\n");
	}

	return retval;
}

#endif


