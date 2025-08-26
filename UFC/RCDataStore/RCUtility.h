//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCUtility_H
#define __RCUtility_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCUtility
//------------------------------------------------------------------------------------------------------------------------
#ifndef mAbs
#define mAbs(a)      (((a) < 0) ? ((a)*(-1)) : (a))
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef mMax
#define mMax(a,b)    (((a) > (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef mMin
#define mMin(a,b)    (((a) < (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef mFloatEqual
#define mFloatEqual_Tolerance 0.0000001
#define mFloatEqual(x,v) ( ( ( (v) - mFloatEqual_Tolerance) < (x) ) && ( (x) < ( (v) + mFloatEqual_Tolerance ) ) )
#endif
//------------------------------------------------------------------------------------------------------------------------
// class RCUtility
//------------------------------------------------------------------------------------------------------------------------
class RCUtility
{
public: ///< For BCD Pack Decode
	static void	PackBcdDecode( UInt8* Target, UInt8* Source, int SourceLen );
	static void	PackBcdDecodeToCStr( UInt8* Target, UInt8* Source, int SourceLen );
public: ///< For Numerical conversion or processing
	static double	Round( double Value, int Precision );	///< Currency practice
	static double	Ceiling( double Value, int Precision );	///< Currency practice
	static double	Floor( double Value, int Precision );	///< Currency practice
	static double	ShiftIntToDouble( int SrcValue, int Digit );
	static int	ShiftDoubleToInt( double SrcValue, int Digit );
	static void	AssignDoubleToString( double SrcValue, UFC::AnsiString& DestStr, int ShowPoint );
public: ///< For Date Time format
	static int	GetHHMMSSmmmAsInt( void );		///< get time for HHMMSSmmm as Int
	static Int64	GetHHMMSSm6AsInt64( void );		///< get time for HHMMSSmmmmmm as Int64
	static Int64	time_us( void );			///< get microseconds of today
	static void	GetHH_MM_SS_m6( char* strBuf );	///< get time for HH:MM:SS.mmmmmm X(15)
	static Int64	GetHH_MM_SS_m6AndTime_us( char* strBuf ); ///< get time for HH:MM:SS.mmmmmm X(15) and return time_us()
        static void     IntToHH_MM_SSString( Int32 Time, char* strBuf ); ///< HHMMSSm3 Int32 Time to HH::MM::SS String
public: ///< For Multifarious
	static int	RightTrimLineFeed( char* strBuf );
	static BOOL	IsFileExist( const char* strPath );
};
//------------------------------------------------------------------------------------------------------------------------
// Memory Queue Functionality
//------------------------------------------------------------------------------------------------------------------------
typedef UFC::PtrQueue<UFC::AnsiString>	TMemoryQueue;
//------------------------------------------------------------------------------------------------------------------------
// class MemoryQueueListener
//------------------------------------------------------------------------------------------------------------------------
class MemoryQueueListener
{
public:
	virtual void OnQueueDataArrived( UFC::AnsiString* QueueData ) = 0;
	virtual ~MemoryQueueListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class MemoryQueueSubject
//------------------------------------------------------------------------------------------------------------------------
class MemoryQueueSubject : public UFC::PThread
{
private:
	TMemoryQueue*			FMemoryQueue;
	MemoryQueueListener*	FQueueListenerObject;
private: ///< PThread implementation
	void Execute( void );
public:
	void Run( void );
public:
	MemoryQueueSubject( TMemoryQueue* MemoryQueue, MemoryQueueListener* QueueListenerObject );
	virtual ~MemoryQueueSubject() {}
};
//------------------------------------------------------------------------------------------------------------------------
// TrashCan Functionality ( this functionality no work if cross day system )
//------------------------------------------------------------------------------------------------------------------------
// class RCCommTrashBase
//------------------------------------------------------------------------------------------------------------------------
class RCCommTrashBase
{
//-------------------------
friend class RCCommTrashCan;
//-------------------------
private:
	Int32	FSecondsToday;
	Int32	FStaySeconds;
private:
	void CreateGlobalTrashCanIfNeed( void );
public:
	void ToTrashCan( Int32 StaySeconds );
public:
	RCCommTrashBase();
	virtual ~RCCommTrashBase() {}
};
//------------------------------------------------------------------------------------------------------------------------
typedef vector< RCCommTrashBase* > TCont_CommTrashCan;
//------------------------------------------------------------------------------------------------------------------------
// class RCCommTrashCan
//------------------------------------------------------------------------------------------------------------------------
class RCCommTrashCan : public UFC::PThread
{
//-------------------------
friend class RCCommTrashBase;
//-------------------------
private:
	Int32					FCircuitSeconds;
	Int32					FCircuitMS;
	UFC::PCriticalSection	FTrashContainer_CS;	///< Critical session to protect Trash Can Container.
	TCont_CommTrashCan		FTrashContainer;	///< Trash Can Container
public: ///< PThread::Execut() implementation.
	void Execute( void );
public:
	RCCommTrashCan( Int32 CircuitSeconds = 1 );
	~RCCommTrashCan() {}
};
//------------------------------------------------------------------------------------------------------------------------
extern UFC::PCriticalSection	gRCCommTrashCan_CS;
extern RCCommTrashCan*			gRCCommTrashCan;
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
