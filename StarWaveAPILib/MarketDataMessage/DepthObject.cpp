#include "DepthObject.h"
//----------------------------------------------------------------------------
DepthObject::DepthObject()
{
    Reset();
}
//----------------------------------------------------------------------------
int DepthObject::DepthEnumToInt( int depthEnum ) const
{
    int depth = 0;
    while( depthEnum > 0 )
    {
        depthEnum >>= 1;
        depth++;
    }
    return depth;
}
//----------------------------------------------------------------------------
int DepthObject::IntToDepthEnum( int Depth ) const
{
    if( Depth > 0 )
    {
        int Result = 0x01 << ( Depth - 1 );
        return Result;
	}
    return UnknownDepth;
}
//----------------------------------------------------------------------------
void DepthObject::Reset( void )
{
    FDepthCount = 0;
    FBidMap = 0;
    FAskMap = 0;
    
	for( register int i = 0; i < MAX_DEPTH; i++ )
    {
		FBidPx[ i ] = 0;
        FBidQty[ i ] = 0;
        FAskPx[ i ] = 0;
        FAskQty[ i ] = 0;
    }
}
//----------------------------------------------------------------------------
BOOL DepthObject::GetBidDepth( int Depth, double& Px, int& Qty ) const
{
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FBidMap ) > 0 )
		{
			Px = FBidPx[Depth - 1];
            Qty = FBidQty[Depth - 1];
            return TRUE;
        }
    }
    return FALSE;
}
//----------------------------------------------------------------------------
BOOL DepthObject::GetAskDepth( int Depth, double& Px, int& Qty ) const
{
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FAskMap ) > 0 )
		{
			Px = FAskPx[Depth - 1];
			Qty = FAskQty[Depth - 1];
			return TRUE;
		}
	}
	return FALSE;
}
//----------------------------------------------------------------------------
double DepthObject::GetBidPx( int Depth ) const
{
	double Px = 0;
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FBidMap ) > 0 )
			Px = FBidPx[Depth - 1];
	}
	return Px;
}
//----------------------------------------------------------------------------
double DepthObject::GetAskPx( int Depth ) const
{
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FAskMap ) > 0 )
			return FAskPx[Depth - 1];
	}
	return 0;
}
//----------------------------------------------------------------------------
int DepthObject::GetBidQty( int Depth ) const
{
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FBidMap ) > 0 )
			return FBidQty[Depth - 1];
	}
	return 0;
}
//----------------------------------------------------------------------------
int DepthObject::GetAskQty( int Depth ) const
{
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		int dep = IntToDepthEnum( Depth );

		if( ( dep & FAskMap ) > 0 )
			return FAskQty[Depth - 1];
	}
	return 0;
}
//----------------------------------------------------------------------------
BOOL DepthObject::SetBidDepth( int Depth, double Px, int Qty )
{
    if( Depth > 0 && Depth <= MAX_DEPTH )
    {
		FBidPx[Depth - 1] = Px;
		FBidQty[Depth - 1] = Qty;

		//UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Bid Depth:[%d] Px[%f] Qty[%d] bitmap:[%d]", Depth, Px, Qty, IntToDepthEnum( Depth ) );
        if( !( FBidMap & IntToDepthEnum( Depth ) ) )
        {
			FBidMap |= IntToDepthEnum( Depth );
            FDepthCount++;
        }
        return TRUE;
    }
    return FALSE;
}
//----------------------------------------------------------------------------
BOOL DepthObject::SetAskDepth( int Depth, double Px, int Qty )
{
    if( Depth > 0 && Depth <= MAX_DEPTH )
    {
		FAskPx[Depth - 1] = Px;
        FAskQty[Depth - 1] = Qty;

        //UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Ask Depth:[%d] Px[%f] Qty[%d] bitmap:[%d]", Depth, Px, Qty, IntToDepthEnum( Depth ) );
        if( !( FAskMap & IntToDepthEnum( Depth ) ) )
        {
            FAskMap |= IntToDepthEnum( Depth );
            FDepthCount++;
        }
        return TRUE;
    }
    return FALSE;
}
//----------------------------------------------------------------------------
void DepthObject::SaveToStream( UFC::PStream* Stream )
{
    UFC::NDouble NPx;
    UFC::NInt32 NQty;
    
    double Px;
	int Qty;

    UFC::NInt32 NDepthCount( FDepthCount );
    UFC::NInt32 NBidMap( FBidMap );
    UFC::NInt32 NAskMap( FAskMap );

    NDepthCount.SaveToStream( Stream );
    NBidMap.SaveToStream( Stream );
    NAskMap.SaveToStream( Stream );
    //UFC::BufferedLog::Printf( " FDepthCount:[%d] BidMap:[%d] AskMap:[%d]", FDepthCount, FBidMap, FAskMap );
    for( register int i = 1; i <= MAX_DEPTH; i++ )
    {   
        if( GetBidDepth( i, Px, Qty ) )
        {
            NPx = Px;
            NQty = Qty;
            
            NPx.SaveToStream( Stream );
            NQty.SaveToStream( Stream );
        }
    }

    for( register int i = 1; i <= MAX_DEPTH; i++ )
    {        
        if( GetAskDepth( i, Px, Qty ) )
        {
            NPx = Px;
            NQty = Qty;

            NPx.SaveToStream( Stream );
            NQty.SaveToStream( Stream );
        }
    }
}
//----------------------------------------------------------------------------
void DepthObject::LoadFromStream( UFC::PStream* Stream )
{
    UFC::NDouble NPx;
    UFC::NInt32 NQty, NDepthCount, NBidMap, NAskMap;

    Reset();
    
    NDepthCount.LoadFromStream( Stream );
    NBidMap.LoadFromStream( Stream );
    NAskMap.LoadFromStream( Stream );

    FDepthCount = NDepthCount.ToInt32();
    FBidMap = NBidMap.ToInt32();
    FAskMap = NAskMap.ToInt32();
	// UFC::BufferedLog::Printf( " FDepthCount:[%d] BidMap:[%d] AskMap:[%d]", FDepthCount, FBidMap, FAskMap );
    for( register int i = 1; i <= MAX_DEPTH; i++ )
    {
        if( ( FBidMap & IntToDepthEnum( i ) ) > 0 )
        {
            NPx.LoadFromStream( Stream );
            NQty.LoadFromStream( Stream );
			FBidPx[i - 1] = NPx.ToDouble();
			FBidQty[i - 1] = NQty.ToInt32();
		}
	}

	for( register int i = 1; i <= MAX_DEPTH; i++ )
	{
		if( ( FAskMap & IntToDepthEnum( i ) ) )
		{
			NPx.LoadFromStream( Stream );
			NQty.LoadFromStream( Stream );
			FAskPx[i - 1] = NPx.ToDouble();
            FAskQty[i - 1] = NQty.ToInt32();
        }
    }
}
//----------------------------------------------------------------------------
void DepthObject::PrintDepth( void )
{
    double Px;
    int Qty;
    
    for( register int i = 1; i <= MAX_DEPTH; i++ )
    {
        if( GetBidDepth( i, Px, Qty ) )
            UFC::BufferedLog::Printf( " BidPx %02d:[%f] BidQty %02d:[%d]", i, Px, i, Qty );
    }

    for( register int i = 1; i <= MAX_DEPTH; i++ )
    {
        if( GetAskDepth( i, Px, Qty ) )
            UFC::BufferedLog::Printf( " AskPx %02d:[%f] AskQty %02d:[%d]", i, Px, i, Qty );
    }
}
//----------------------------------------------------------------------------






