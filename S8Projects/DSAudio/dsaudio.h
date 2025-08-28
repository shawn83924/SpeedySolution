/**
 * @file	DSAudio.h
 * @author	Little Chio, CakeChen
 * @date	2003/10/03, 2004/06/01, 2004/06/23
 *
 *   The implementation classes DSWaveBuffer and DSAudio for the interface
 *   GdAudioWave and GdAudio.
 */

#ifndef __DSAUDIO_H__
#define __DSAUDIO_H__

#pragma warning (disable: 4786)

#if defined(_XBOX)

#include <xtl.h>
#include <dsstdfx.h>

#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

#else

#include <windows.h>
#include <dshow.h>
#include <dsound.h>

#endif

#include <string>
#include <vector>
#include "gdaudio.h"

class DSAudio;


class DSWaveBuffer : public GdAudioWave
{
public:
	
     DSWaveBuffer(DSAudio *owner);
     virtual ~DSWaveBuffer();

     bool Play();
	 bool Play(float volume, float pan);
     void SetVolume(float volume);	///< Value of volume = 1.0 for max volume, volume = 0 for silence
     void SetPan(float pan);
     void Stop();
     void SetLoop(bool isloop);

     float GetVolume() const { return m_Volume; }
     float GetPan() const { return m_Pan; }
     bool IsPlaying() const { return (GetStatus()&DSBSTATUS_PLAYING)!=0; }
     bool IsLoopable() const { return m_bLoopable; }
     const char* GetName() const { return m_Name.c_str();}

     DSWaveBuffer *Clone();
     bool LoadFromFile(const char *filename);
     bool LoadFromMemory(LPVOID lpBuffer);          

protected:

     DSWaveBuffer(DSAudio *owner, LPDIRECTSOUNDBUFFER lpdsb, bool loop, float vol);
     DWORD GetStatus() const;
     
	 std::string m_Name;
	 DSAudio *m_Owner;
     bool m_bLoopable;
     float m_Volume;
	 float m_Pan;
     LPDIRECTSOUNDBUFFER m_lpDSB;
     WAVEFORMATEX  m_WaveFormat;

	 friend class DSAudio;

};


class DSAudio : public GdAudio
{
public:
     
     DSAudio();
     virtual ~DSAudio();
 
	 bool InitDevice(void *device_id);
	 void DeinitDevice();
	 void Update(){}
 
     void ClearAll();
	 void StopAll();
	 
	 GdAudioWave *LoadBaseWave(const char* name, const char *filename);
     GdAudioWave *CreateWave(const char* name);
     void SetMasterVolume(float volume);	///< Value of volume = 1.0 for max volume, volume = 0 for silence
     float GetMasterVolume() const { return m_MasterVolume; }
	 GdAudioWave *EnumBaseWave(int index) const { return m_Waves[index]; }
	 GdAudioWave *GetBaseWave(const char* name) const;
     int GetBaseWaveCount() const { return m_Waves.size(); }

	 void Clear();    
     bool Stop(const std::string &name);
     LPDIRECTSOUND GetDS() const { return m_lpDS; }	///< Return the underlying DirectSound object. (Please do not release the returned pointer because it has NOT been AddRef()ed.)

protected:

     float m_MasterVolume;
	 std::vector<DSWaveBuffer *> m_Waves;
     std::vector<DSWaveBuffer *> m_CloneWaves;
     LPDIRECTSOUND m_lpDS;

	 friend class DSWaveBuffer;
};


#endif
