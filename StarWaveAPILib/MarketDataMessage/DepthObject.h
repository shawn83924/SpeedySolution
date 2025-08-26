/* 
 * File:   DepthObject.h
 * Author: yuan
 *
 * Created on May 3, 2011, 11:13 AM
 */

#ifndef DEPTHOBJECT_H
#define	DEPTHOBJECT_H
#include "UFC.h"
//----------------------------------------------------------------------------
#define MAX_DEPTH  10
//----------------------------------------------------------------------------
enum
{
    UnknownDepth = 0x0000,
    Depth1   = 0x0001,
    Depth2   = 0x0002,
    Depth3   = 0x0004,
    Depth4   = 0x0008,
    Depth5   = 0x0010,
    Depth6   = 0x0020,
    Depth7   = 0x0040,
    Depth8   = 0x0080,
    Depth9   = 0x0100,
    Depth10  = 0x0200,
};
//----------------------------------------------------------------------------
class DepthObject
{
private:
	UFC::Int32          FDepthCount;
	double				FBidPx[MAX_DEPTH];
	UFC::Int32		    FBidQty[MAX_DEPTH];
	double				FAskPx[MAX_DEPTH];
	UFC::Int32			FAskQty[MAX_DEPTH];
	UFC::Int32          FBidMap;
	UFC::Int32          FAskMap;
public:
    DepthObject();
    int DepthCount( void ) { return FDepthCount; }
    void Reset( void );
	BOOL GetBidDepth( int Depth, double& Px, int& Qty ) const ;
	BOOL GetAskDepth( int Depth, double& Px, int& Qty ) const ;
	BOOL SetBidDepth( int Depth, double Px, int Qty );
	BOOL SetAskDepth( int Depth, double Px, int Qty );
	void SaveToStream( UFC::PStream* Stream );
	void LoadFromStream( UFC::PStream* Stream );
	void PrintDepth( void );
	double GetBidPx( int Depth ) const;
	double GetAskPx( int Depth ) const;
	int GetBidQty( int Depth ) const;
	int GetAskQty( int Depth ) const ;
private:
	int DepthEnumToInt( int depthEnum ) const;
	int IntToDepthEnum( int Depth ) const;
};
//----------------------------------------------------------------------------
#endif	/* DEPTHOBJECT_H */

