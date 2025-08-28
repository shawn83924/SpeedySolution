#ifndef __WAVELOADER_H__
#define __WAVELOADER_H__

#if	defined(_WIN32)
#include "windows.h"
#else
#include <unistd.h>
#endif

class WaveLoader
{
public:
     
	WaveLoader();
	~WaveLoader();
	bool LoadFromFile(const char *filename);
	bool LoadFromMemory(void* pBuffer);
	void Destroy();
	long AddRef();
	long Release();

	DWORD GetSampleRate() const { return m_SampleRate; }
	WORD GetNumChannels() const { return m_NumChannels; }
	WORD GetBitsPerSample() const { return m_BitsPerSample; }
	DWORD GetDataSize() const { return m_DataSize; }
	BYTE *GetWaveData() const { return m_pWaveData; }

private:

	WaveLoader(const WaveLoader &rhs);
	WaveLoader& operator=(const WaveLoader &rhs);

	long m_RefCount;
	DWORD m_SampleRate;
	WORD m_NumChannels;
	WORD m_BitsPerSample;
	DWORD m_DataSize;
	BYTE *m_pWaveData;
	BYTE *m_pMemoryChunk;
};

#endif
