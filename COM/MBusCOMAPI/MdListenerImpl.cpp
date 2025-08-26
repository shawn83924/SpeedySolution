// ---------------------------------------------------------------------------
// MDLISTENERIMPL : Implementation of TMdListenerImpl (CoClass: MdListener, Interface: IMdListener)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MdListenerImpl.h"
//---------------------------------------------------------------------------
extern bool CanHandleSubject( const AnsiString& Subject );
//---------------------------------------------------------------------------
void TMdListenerImpl::UpdateUniqueName( void )
{
	FName.Printf( "Subscriber_%s_%s_%d", FSubject.c_str(), FKey.c_str(), (int)this );
}
//---------------------------------------------------------------------------
void TMdListenerImpl::CreateSubscriber( MApp* pApp, const MString& Subject, const MString& Key  )
{
    FSubject = Subject;
    FKey     = Key;
    UpdateUniqueName();
    if( FSubscriber != NULL  ) ///< a Subscriber already exists.
    {
		if( FApp != NULL )
   	        FApp->DeleteSubscriber( FSubscriber ); ///< Tell MApp to remove this Subscriber.
        delete FSubscriber;                       ///< Delete the old Subscriber object.
    }
	///< Create a new Subscriber.
    FSubscriber = new MSubscriber( FName, FSubject, FKey ); ///< Create a new Subscriber.
	FSubscriber->addListener( FHandler );                       ///< Tell MApp to add this Subscriber.
    FApp = pApp;
    if( FApp != NULL )
  	    FApp->AddSubscriber( FSubscriber );
}
//---------------------------------------------------------------------------
void TMdListenerImpl::Update( void )
{
    UpdateUniqueName();
    if( FSubscriber != NULL ) ///< Delete the old Subscriber object
    {
        if( FApp != NULL )
            FApp->DeleteSubscriber( FSubscriber );
        delete FSubscriber;
    }
    FSubscriber = new MSubscriber( FName.c_str(), FSubject.c_str(), FKey.c_str() );
	FSubscriber->addListener( FHandler );
    if( FApp != NULL )
        FApp->AddSubscriber( FSubscriber );
}
//---------------------------------------------------------------------------
void TMdListenerImpl::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree )
{
	VARIANT    MTreeInstance = CastToVariant<MTree*>( pTree );
	WideString WideSubject( Subject.c_str() );
	WideString WideKey( Key.c_str() );
	TMdMessageImpl*	MBusMessage = new TMdMessageImpl();

	MBusMessage->Attach( &MTreeInstance );
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		 FEventList[i]->OnMessageReceived( WideSubject.c_bstr(), WideKey.c_bstr(), MBusMessage );
//	MBusMessage->Destroy();
}
// ---------------------------------------------------------------------------
// TMdListenerImpl
// ---------------------------------------------------------------------------
__fastcall TMdListenerImpl::TMdListenerImpl()
:FSubscriber( NULL )
,FApp( NULL )
,FSubject( "all" )
,FKey( "all" )
{
	FHandler     = new MsgHandler( this );
	UpdateUniqueName();
}
// ---------------------------------------------------------------------------
// TMdListenerImpl
// ---------------------------------------------------------------------------
__fastcall TMdListenerImpl::TMdListenerImpl(const System::_di_IInterface Controller)
:inherited(Controller)
,FSubscriber( NULL )
,FApp( NULL )
,FSubject( "all" )
,FKey( "all" )
{
	FHandler     = new MsgHandler( this );
	UpdateUniqueName();
}
// ---------------------------------------------------------------------------
// TMdListenerImpl
// ---------------------------------------------------------------------------
__fastcall TMdListenerImpl::TMdListenerImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
:inherited(Factory, Controller)
,FSubscriber( NULL )
,FApp( NULL )
,FSubject( "all" )
,FKey( "all" )
{
	FHandler     = new MsgHandler( this );
	UpdateUniqueName();
}
// ---------------------------------------------------------------------------
void __fastcall TMdListenerImpl::EventSinkChanged(const System::_di_IInterface EventSink)
{
	if( EventSink != NULL )
	{
		IMdListenerEventsDisp* NewEvents = new IMdListenerEventsDisp();
		FEventList.push_back( NewEvents );
		NewEvents->Bind( EventSink );
	}
}
// ---------------------------------------------------------------------------
// TMdListenerImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMdListenerImpl>(Comserv::GetComServer(),
						   __classid(TMdListenerImpl),
						   CLSID_MdListener,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::Create(IMdTransport* Transport, BSTR Subject, BSTR Key)


{
	if( Transport != NULL )
	{
		VARIANT TransportInstance = Transport->GetTransportInstance();
		MApp*   pMApp = CastToObject<MApp*>( &TransportInstance );
		if( pMApp != NULL )
		{
			AnsiString NewSubject( Subject );
			AnsiString NewKey( Key );

			if( CanHandleSubject( NewSubject ) == false )
				return Error("No right to use this subject", IID_IMdListener );
			CreateSubscriber( pMApp, NewSubject.c_str(), NewKey.c_str() );
			return S_OK;
		}
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::Destroy()
{
    if( FSubscriber != NULL ) ///< A Subscriber object exists.
    {
  	    if( FApp != NULL )                        ///< If MApp exists.
            FApp->DeleteSubscriber( FSubscriber ); ///< Tell MApp to remove this Subscriber.
        delete FSubscriber;                       ///< Delete the Subscriber object;
        FSubscriber = NULL;
    }
    FApp = NULL; ///< Set pointer to MApp to NULL.
    return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::get_Key(BSTR* Value)
{
    WideString WideKey( FKey.c_str() );
    *Value = WideKey.Copy();
    return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::get_Subject(BSTR* Value)
{
    WideString WideSubject( FSubject.c_str() );
	*Value = WideSubject.Copy();
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::IsValid(VARIANT_BOOL* Value)
{
    if( FSubscriber != NULL && FApp != NULL )
        *Value = true;
    else
        *Value = false;
  	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::set_Key(BSTR Value)
{
	AnsiString AnsiKey( Value );

    if( AnsiKey.AnsiCompare( FKey.c_str() ) != 0 )
    {
        FKey = AnsiKey.c_str();
        Update();
    }
  	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdListenerImpl::set_Subject(BSTR Value)
{
	AnsiString AnsiSubject( Value );

	if( CanHandleSubject( AnsiSubject ) == false )
		return Error("No right to use this subject", IID_IMdListener );

	if( AnsiSubject.AnsiCompare( FSubject.c_str() ) != 0 )
	{
		FSubject = AnsiSubject.c_str();
		Update();
	}
	return S_OK;
}
//---------------------------------------------------------------------------
