#ifndef __GDAUDIO_H__
#define __GDAUDIO_H__

#include "vectors.h"

class GdAudioWave
{
public:

	GdAudioWave();
	virtual ~GdAudioWave(){}

	virtual bool Play() = 0;
	virtual bool Play(float volume, float pan) = 0;
	virtual void SetVolume(float vol) = 0;
	virtual void SetPan(float pan) = 0;
	virtual void Stop() = 0;
	virtual void SetLoop(bool flag) = 0;

	virtual float GetVolume() const = 0;
	virtual float GetPan() const = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool IsLoopable() const = 0;
	virtual const char *GetName() const = 0;


	float m_MinDist, m_MaxDist;
	DSAud::Vector m_ListenerPos;
	DSAud::Vector m_ListenerLeftDir;
	DSAud::Vector m_EmitterPos;

	virtual void Update3D(float VolumeScale);

};


class GdAudio
{
public:

	GdAudio(){}
	virtual ~GdAudio(){}

	virtual bool InitDevice(void *device_id) = 0;
	virtual void DeinitDevice() = 0;

	virtual void Update() = 0;
	virtual void ClearAll() = 0;
	virtual void StopAll() = 0;

	virtual GdAudioWave *LoadBaseWave(const char *name, const char *filename) = 0;
	virtual GdAudioWave *CreateWave(const char *name) = 0;
    virtual void SetMasterVolume(float volume) = 0;
    virtual float GetMasterVolume() const = 0;
	virtual GdAudioWave *EnumBaseWave(int index) const = 0;
    virtual GdAudioWave *GetBaseWave(const char *name) const = 0;
	virtual int GetBaseWaveCount() const = 0;

};


//===========================================================
// class AudioWave
//===========================================================
// Constructor
inline
GdAudioWave::GdAudioWave():
m_MinDist(1.0f), 
m_MaxDist(100.0f),
m_ListenerPos(0,0,0),
m_ListenerLeftDir(1.0f,0,0),
m_EmitterPos(0,0,0)
{}

// Update 3D sound
inline
void GdAudioWave::Update3D(float VolumeScale)
{
	DSAud::Vector v = m_EmitterPos-m_ListenerPos;
	float distance = v.Norm();

	if(distance > m_MinDist && distance <= m_MaxDist)
	{
		float r = (m_MaxDist-distance) / (m_MaxDist-m_MinDist);
    	SetVolume(r * VolumeScale);
		SetPan((m_ListenerLeftDir*v) * (1.0f-r) / (4.0f*m_ListenerLeftDir.Norm()*distance));
	}
	else if(distance <= m_MinDist)
	{
        SetVolume(VolumeScale);
		SetPan(0);
	}
	else
	{
		SetVolume(0.0f);
	}
}


#endif
