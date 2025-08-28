/*///////////////////////////////////////////////////////
File Name: DSAudio.cpp

Description : implmentation for the DSWaveBuffer class.

Last Modify : 2002/10/01, 2004/05/12

Created by : Little Chio
Modified by : Little Chio, CakeChen
*////////////////////////////////////////////////////////

#define STRSAFE_NO_DEPRECATE

#include "tchars.h"
#include "dsaudio.h"
#include "osfunc.h"
#include "waveloader.h"


//////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////
DSWaveBuffer::DSWaveBuffer(DSAudio *owner):
m_Owner(owner),
m_lpDSB(NULL),
m_bLoopable(false),
m_Volume(1.0f),
m_Pan(0)
{
}

DSWaveBuffer::DSWaveBuffer(DSAudio *owner, 
LPDIRECTSOUNDBUFFER lpdsb, bool loop, float volume):
m_Owner(owner),
m_lpDSB(lpdsb),
m_bLoopable(loop),
m_Volume(volume),
m_Pan(0)
{
}

// Destructor
DSWaveBuffer::~DSWaveBuffer()
{
     if (m_lpDSB!=NULL)
    {
         m_lpDSB->Release();
         m_lpDSB=NULL;
    }

	if(m_Owner)
	{
	    for(unsigned int i=0; i< m_Owner->m_CloneWaves.size(); i++)
		  if(m_Owner->m_CloneWaves[i]==this)
		  {
				m_Owner->m_CloneWaves.erase(m_Owner->m_CloneWaves.begin()+i);
				break;
		  }
	}
}

//***********************************************
// SetLoop
//
// Determine whether the wave buffer will loop when playing
//***********************************************
void DSWaveBuffer::SetLoop(bool isloop)
{
     m_bLoopable = isloop;
     
     // If the buffer is playing, change the loop status at once.
     if(IsPlaying())
     {
         if (m_bLoopable)
             m_lpDSB->Play(0, 0, DSBPLAY_LOOPING);  
         else
             m_lpDSB->Play(0, 0, 0);
     }
}
     	
//***********************************************
// SetVolume
//
// Value of volume = 1.0 for max volume, volume = 0 for silence
//***********************************************
void DSWaveBuffer::SetVolume(float volume)
{
    if (m_lpDSB)
    {
		// clamp
		if(volume>1.0f) volume = 1.0f;
		else if(volume<0) volume = 0;

        m_Volume = volume;
        volume *= m_Owner->GetMasterVolume();
		
		LONG db, db_diff;
        
        if(volume==0)
        {
			db = DSBVOLUME_MIN;
			db_diff = db + 1;
        }
        else
        {
			db = (LONG)((DSBVOLUME_MAX-DSBVOLUME_MIN)*log(volume)/10 + DSBVOLUME_MAX);
			db_diff = db - 1;
        }

		// set different value to make sure the volume is set
		// (it's DirectSound's bug)
		m_lpDSB->SetVolume(db_diff);
		m_lpDSB->SetVolume(db);
    }
}

//***********************************************
// SetPanning
//
// Value of pan = -1.0 for most left, pan = 1.0 for most right
//***********************************************
void DSWaveBuffer::SetPan(float pan)
{
	if(m_lpDSB)
    {

#if defined(_XBOX)

	   m_lpDSB->SetPosition(-pan, 0, 0, DS3D_IMMEDIATE);

#else
    
       m_lpDSB->SetPan((LONG)((pan+1.0)*(DSBPAN_RIGHT-DSBPAN_LEFT)/2
                        +DSBPAN_LEFT));
#endif
	   m_Pan = pan;
    }

}

//***********************************************
// Play
//
// Play this wave buffer
//***********************************************
bool DSWaveBuffer::Play()
{
   if(m_lpDSB)
   {
        SetVolume(GetVolume());
		SetPan(m_Pan);

        m_lpDSB->SetCurrentPosition(0);
        m_lpDSB->Play(0, 0, m_bLoopable?DSBPLAY_LOOPING:0 );

		return true;
   }
	return false;
}

bool DSWaveBuffer::Play(float volume, float pan)
{
    if(m_lpDSB)
    {
        SetVolume(volume);
		SetPan(pan);

        m_lpDSB->SetCurrentPosition(0);
        m_lpDSB->Play(0, 0, m_bLoopable?DSBPLAY_LOOPING:0 );

		return true;
    }
	return false;
}

//***********************************************
// GetStatus
//
// Return status of this buffer. Three possible flags are:
//   1. DSBSTATUS_BUFFERLOST
//          The buffer is lost and must be restored before it can be played or locked. 
//   2. DSBSTATUS_LOOPING 
//          The buffer is being looped. 
//   3. DSBSTATUS_PLAYING 
//          The buffer is playing. If this value is not set, the buffer is stopped. 
//***********************************************
DWORD DSWaveBuffer::GetStatus() const
{
     if (m_lpDSB)
    {
         DWORD dwStatus;
         if (m_lpDSB->GetStatus(&dwStatus) != DS_OK) return 0;
         return dwStatus;
     }
     else return 0;
}

//***********************************************
// Stop
//
// Stop the sound if the wave buffer is playing.
//***********************************************
void DSWaveBuffer::Stop()
{
       if (m_lpDSB!=NULL) 
           m_lpDSB->Stop();
}

//***********************************************
// Clone
//
// Duplicate this wave buffer. The wave data  are shared with the 
// duplicated wave buffer.
//***********************************************
DSWaveBuffer *DSWaveBuffer::Clone()
{
     LPDIRECTSOUNDBUFFER dsb;

#if defined(_XBOX)

     BYTE    *waveBits = NULL;	
     BYTE    *waveBits2 = NULL;	
     DWORD   dwLength = 0;
     DWORD   dwLength2 = 0;
	 DSBUFFERDESC dsbd;

	 ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));

	 dsbd.dwSize = sizeof(DSBUFFERDESC);
	 dsbd.dwBufferBytes = 0;
	 dsbd.dwFlags = DSBCAPS_CTRL3D;
	 dsbd.lpwfxFormat = &m_WaveFormat;

	 if (DS_OK == m_Owner->m_lpDS->CreateSoundBuffer(&dsbd, &dsb, NULL))
	 {
       // Lock the DirectSound buffer to get wave data address
       if (DS_OK == m_lpDSB->Lock( 0, 0, 
             (void **)&waveBits,         // The first block of sound data
             &dwLength,       // The length of the first block of data
             (void **)&waveBits2,        // The second block of sound data
             &dwLength2,      // The length of the second block of data
             DSBLOCK_ENTIREBUFFER))
	   {
         // Unlock our buffer
          m_lpDSB->Unlock(waveBits, dwLength, waveBits2, dwLength2);
		  dsb->SetBufferData(waveBits, dwLength+dwLength2);

		  return new DSWaveBuffer(m_Owner, dsb, m_bLoopable, m_Volume);
	   }
	 }

#else

     if(m_Owner->m_lpDS->DuplicateSoundBuffer(m_lpDSB, &dsb)==DS_OK)
     {
         DSWaveBuffer *pWave = new DSWaveBuffer(m_Owner, dsb, m_bLoopable, m_Volume);
		 pWave->m_Name = m_Name;
		 return pWave;
     }

#endif

     return NULL;
}

//*************************************************
// LoadFromFile
//
// Load the WAV file as sound data and place it in a DirectSound 
// secondary buffer.
//*************************************************
bool DSWaveBuffer::LoadFromFile(const char *filename)
{
  BYTE *lpWaveData = NULL;
  WaveLoader   WaveFile;	// The wave file we will load
  
  // These variables hold the sound data information for our DirectSound buffer
  BYTE    *waveBits  = NULL;	// A pointer to the first block of sound data
  BYTE    *waveBits2 = NULL;	// A pointer to the second block of sound data
  DWORD   dwLength;			// Length of the first block of data
  DWORD   dwLength2;		// Length of the second block of data
  DSBUFFERDESC dsbd;

  if(WaveFile.LoadFromFile(filename))
  {
	  lpWaveData = WaveFile.GetWaveData();
  }

  if(!lpWaveData)
  {
	  char str[512];
	  sprintf(str, "Failed to load wave file: %s\n", filename);
	  OutputDebugStringA( str );
	  return false;
  }

  // Set up our direct sound buffer. 
	 ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	 dsbd.dwSize = sizeof(DSBUFFERDESC);

  // Now set up a buffer on the sound card's memory
#if defined(_XBOX)
     dsbd.dwFlags = DSBCAPS_CTRL3D;
#else
     dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;

#endif

	 m_WaveFormat.cbSize = sizeof(WAVEFORMATEX);
	 m_WaveFormat.nChannels = WaveFile.GetNumChannels();
	 m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	 m_WaveFormat.nSamplesPerSec = WaveFile.GetSampleRate();
	 m_WaveFormat.wBitsPerSample = WaveFile.GetBitsPerSample();
	 m_WaveFormat.nBlockAlign = m_WaveFormat.nChannels * m_WaveFormat.wBitsPerSample / 8;
	 m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nSamplesPerSec * m_WaveFormat.nBlockAlign;
  
    // Set our wave size and format which must be a PCM format
    dsbd.dwBufferBytes = WaveFile.GetDataSize(); 
    dsbd.lpwfxFormat   = &m_WaveFormat;

    // Now, create our sound buffer
    if (DS_OK != m_Owner->m_lpDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL))
	{
      OutputDebugString(_T("Failed to create sound buffer\n"));
      return false;
	}

  // We now have a pointer to a DirectSoundBuffer. 
  // We can copy blocks of sound data into the buffer 
  // using the Lock and Unlock interfaces on the DirectSoundBuffer:

  // Lock the DirectSound buffer
   if (DS_OK == m_lpDSB->Lock
            (0,               // The offset into the buffer where we will start writing
             dsbd.dwBufferBytes, // The size of the wave file to copy in
             (void **)&waveBits,         // The first block of sound data
             &dwLength,       // The length of the first block of data
             (void **)&waveBits2,        // The second block of sound data
             &dwLength2,      // The length of the second block of data
             0L))             // Flags
    {
       // Copy the first block of data from the wave file
       memcpy(waveBits, lpWaveData, dwLength);

       // Copy the second block of data from the wave file
       if (dwLength2) memcpy(waveBits2, lpWaveData+dwLength , dwLength2);

       // Free the memory which was allocated in the WAVE_LoadFile function
       // now that it has been copied to the buffer
       //delete [] lpWaveData;

       // Unlock our buffer
       if (DS_OK != m_lpDSB->Unlock(waveBits, dwLength, waveBits2, dwLength2))
	   {
           OutputDebugString(_T("Unlock failed"));
           return false;
	   }
   }
   else
   {
      OutputDebugString(_T("Lock failed"));
      return false;
   }
 
   return true;
}

//*************************************************
// LoadFromMemory
//
// Load pre-loaded WAV data from the memory and place it in a 
// DirectSound secondary buffer.
//*************************************************
bool DSWaveBuffer::LoadFromMemory(LPVOID lpBuffer)
{
  BYTE*        lpWaveData = NULL;
  WaveLoader    WaveFile;		// The wave file we will load
  
  // These variables hold the sound data information for our DirectSound buffer
  BYTE    *waveBits         = NULL;	// A pointer to the first block of sound data
  BYTE    *waveBits2        = NULL;	// A pointer to the second block of sound data
  DWORD   dwLength;				// Length of the first block of data
  DWORD   dwLength2;				// Length of the second block of data
  DSBUFFERDESC dsbd;

   if(WaveFile.LoadFromMemory(lpBuffer))
   {
	   lpWaveData = WaveFile.GetWaveData();
   }

  // Set up our direct sound buffer. 
  // Set up our direct sound buffer. 
	 ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	 dsbd.dwSize = sizeof(DSBUFFERDESC);

  // Now set up a buffer on the sound card's memory 
#if defined(_XBOX)
     dsbd.dwFlags = DSBCAPS_CTRL3D;
#else
     dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;

#endif
  
	 m_WaveFormat.cbSize = sizeof(WAVEFORMATEX);
	 m_WaveFormat.nChannels = WaveFile.GetNumChannels();
	 m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	 m_WaveFormat.nSamplesPerSec = WaveFile.GetSampleRate();
	 m_WaveFormat.wBitsPerSample = WaveFile.GetBitsPerSample();
	 m_WaveFormat.nBlockAlign = m_WaveFormat.nChannels * m_WaveFormat.wBitsPerSample / 8;
	 m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nSamplesPerSec * m_WaveFormat.nBlockAlign;
  
    // Set our wave size and format which must be a PCM format
    dsbd.dwBufferBytes = WaveFile.GetDataSize(); 
    dsbd.lpwfxFormat   = &m_WaveFormat;

    // Now, create our sound buffer
    if (DS_OK != m_Owner->m_lpDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL))
	{
      OutputDebugString(_T("Failed to create sound buffer\n"));
      return false;
	}

  // We now have a pointer to a DirectSoundBuffer. 
  // We can copy blocks of sound data into the buffer 
  // using the Lock and Unlock interfaces on the DirectSoundBuffer:

  // Lock the DirectSound buffer
  if (DS_OK == m_lpDSB->Lock
            (0,               // The offset into the buffer where we will start writing
             dsbd.dwBufferBytes, // The size of the wave file to copy in
             (void **)&waveBits,         // The first block of sound data
             &dwLength,       // The length of the first block of data
             (void **)&waveBits2,        // The second block of sound data
             &dwLength2,      // The length of the second block of data
             0L))             // Flags
    {
       // Copy the first block of data from the wave file
       memcpy(waveBits, lpWaveData, dwLength);

       // Copy the second block of data from the wave file
      if (dwLength2) memcpy(waveBits2, lpWaveData+dwLength , dwLength2);

      // Unlock our buffer
      if (DS_OK != m_lpDSB->Unlock(waveBits, dwLength, waveBits2, dwLength2))
	  {
        OutputDebugString(_T("Unlock failed"));
        return false;
	  }
    }
    else
    {
       OutputDebugString(_T("Lock failed"));
       return false;
    }
  
    return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DSAudio::DSAudio():
m_lpDS(0),
m_MasterVolume(1.0f)
{
}

DSAudio::~DSAudio()
{
    DeinitDevice();
}

//*************************************************
//  Init
//
//  Initialize DirectSound. Window handle is required for exclusive mode
//*************************************************
bool DSAudio::InitDevice(void *device_id)
{
     DeinitDevice();

    //----- DirectSound create -----
    if (FAILED(DirectSoundCreate(NULL, &m_lpDS, NULL)))
	  return false;

    if (FAILED(m_lpDS->SetCooperativeLevel((HWND)device_id, DSSCL_NORMAL)))
	  return false;
	
#if defined(_XBOX)

    // We have to select one 3D mixing method...
	FILE *fp = fopen("D:\\dsstdfx.bin", "rb");

	if(fp)
	{
	   fseek(fp, 0, SEEK_END);
	   int size = ftell(fp);
	   fseek(fp, 0, SEEK_SET);

	   void *pBuffer = new BYTE[size];
	   fread(pBuffer, 1, size, fp);
	   fclose(fp);

       LPDSEFFECTIMAGEDESC pDesc;
       DSEFFECTIMAGELOC EffectLoc;

       EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
       EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

       m_lpDS->DownloadEffectsImage( pBuffer, size, &EffectLoc, &pDesc );

       delete[] pBuffer;
    }
	else
	{
       OutputDebugString("Failed to load effect image.\n");
	}

	DirectSoundUseLightHRTF();

    // Initialize listener's orientation and position

	m_lpDS->SetPosition(0, 0, 0, DS3D_IMMEDIATE);
	m_lpDS->SetOrientation(0, 0, 1.0f, 0, 1.0f, 0, DS3D_IMMEDIATE);

#endif

    return true;
}

void DSAudio::DeinitDevice()
{
     ClearAll();

     if(m_lpDS)
	 {
		 m_lpDS->Release();
		 m_lpDS = 0;
	 }
}

//*************************************************
//  Load WAV file as sound data and name this sound by 'name'
//*************************************************
GdAudioWave *DSAudio::LoadBaseWave(const char *name, const char *fileName)
{
      if(m_lpDS==NULL) return NULL;                // must have directsound!
      
      DSWaveBuffer *pBuffer = new DSWaveBuffer(this);
      if(pBuffer->LoadFromFile(fileName))
      {
           m_Waves.push_back(pBuffer);
		   pBuffer->m_Name = name;
           pBuffer->SetVolume(1.0f);          // initialize the volume to max
           return pBuffer;
      }
      
      delete pBuffer;
      return NULL;
}

//*************************************************
//  ClearAll
//
//  Clear all wave buffers.
//*************************************************
void DSAudio::ClearAll()
{
	 
	 std::vector<DSWaveBuffer*>::iterator p = m_Waves.begin();
	 while(p!=m_Waves.end())
	 {
		 (*p)->m_Owner = 0;
		 delete (*p);
		 p++;
	 }

     m_Waves.clear();

	 std::vector<DSWaveBuffer *>::iterator q = m_CloneWaves.begin();
	 while(q!=m_CloneWaves.end())
	 {
         (*q)->m_Owner = 0;
		 delete *q;
		 q++;
	 }

	 m_CloneWaves.clear();
}


GdAudioWave *DSAudio::CreateWave(const char *name)
{
     DSWaveBuffer *pBuffer = (DSWaveBuffer *)GetBaseWave(name);
     if(pBuffer)
     {
          DSWaveBuffer *pClone = pBuffer->Clone();
          if(pClone)
          {
               m_CloneWaves.push_back(pClone); 
               return pClone;
          }
     }
     
     return 0;
}

//*************************************************
//  Stop
//
//  Stop the wave play by sound_name.
//*************************************************
bool DSAudio::Stop(const std::string &sound_name)
{
     DSWaveBuffer *pBuffer = (DSWaveBuffer *)GetBaseWave(sound_name.c_str());
     if(pBuffer)
     {
          pBuffer->Stop();
          return true;
     }
     
     return false;	
}

//*************************************************
//  StopAll
//
//  Stop all the waves.
//*************************************************
void DSAudio::StopAll()
{
	 std::vector<DSWaveBuffer*>::iterator p = m_Waves.begin();

	 while (p !=m_Waves.end() )
	 {
		 (*p)->Stop();
		 p++;
	 }
     
	 std::vector<DSWaveBuffer *>::iterator q = m_CloneWaves.begin();
	 while (q!=m_CloneWaves.end())
	 {
		 (*q)->Stop();
		 q++;
	 }
}

//*************************************************
//  GetBuffer
//
//  Return the wave buffer by sound_name for purpose use.
//*************************************************
GdAudioWave *DSAudio::GetBaseWave(const char *name) const
{
	 std::vector<DSWaveBuffer *>::const_iterator p= m_Waves.begin(); 
	 DSWaveBuffer *shp_temp=0;
	 while ( p!= m_Waves.end())
	 {
        shp_temp = *p;
		if (shp_temp->m_Name == name)
		{
		   return shp_temp;
		}
	    else p++;
	 }

	 return 0;
}     

//*************************************************
//  SetMasterVolume
//
//  Set the master volume
//*************************************************
void DSAudio::SetMasterVolume(float volume)
{
     // clamp
     if(volume>1.0f) m_MasterVolume = 1.0f;
     else if(volume<0) m_MasterVolume = 0;
     else m_MasterVolume = volume;
     
     // validate the loaded wave's volume....

	 std::vector<DSWaveBuffer*>::iterator p = m_Waves.begin();
	 while (p!=m_Waves.end())
	 {
	     DSWaveBuffer *wave = *p;
	     wave->SetVolume(wave->GetVolume());
		 p++;
	 }
     
	 std::vector<DSWaveBuffer *>::iterator q = m_CloneWaves.begin();
	 while (q!=m_CloneWaves.end())
	 {
		 (*q)->SetVolume((*q)->GetVolume());
		 q++;
	 }
}

