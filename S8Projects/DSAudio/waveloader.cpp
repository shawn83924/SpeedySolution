#include "waveloader.h"
#include "osfunc.h"

#define WAVEFORMAT_HEADER_SIZE	18

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif				

WaveLoader::WaveLoader():
m_RefCount(1),
m_NumChannels(0),
m_SampleRate(0),
m_BitsPerSample(0),
m_DataSize(0),
m_pWaveData(NULL),
m_pMemoryChunk(NULL)
{
}

WaveLoader::~WaveLoader()
{
	Destroy();
}

// dummy, object is not copyable
WaveLoader::WaveLoader(const WaveLoader &rhs):
m_RefCount(1),
m_NumChannels(0),
m_SampleRate(0),
m_BitsPerSample(0),
m_DataSize(0),
m_pWaveData(NULL),
m_pMemoryChunk(NULL)
{
}

// dummy, object is not copyable
WaveLoader& WaveLoader::operator=(const WaveLoader &rhs)
{
	return *this;
}

// Load WAV file
bool WaveLoader::LoadFromFile(const char *filename)
{
    FILE *fp;

    if((fp = fopen(filename,"rb"))!=NULL)
	{
		fseek(fp, 0, SEEK_END);
		DWORD filesize = ftell(fp);
		
		BYTE *lpMemory = new BYTE[filesize];
		fseek(fp, 0, SEEK_SET);
		fread(lpMemory, 1, filesize, fp);
		fclose(fp);

		if (LoadFromMemory(lpMemory))
		{
			m_pMemoryChunk = lpMemory;
			return true;
		}

		delete [] lpMemory;
	}
	
    return false;
}

//*************************************************
//   Parses the loaded wave file in memory into its header and samples.
//   To do this, search for the "fmt " and "data" fields.
//*************************************************
bool WaveLoader::LoadFromMemory(void* lpMemory)
{
	Destroy();
    
    // Set up a pointer to indicate the start of the wave memory.
    BYTE *pointMem = (BYTE *)lpMemory;

    // Get the type and length of the wave memory
    DWORD dwRiff = *(DWORD *)pointMem;
    DWORD dwLength = *(DWORD *)(pointMem + 4);
    DWORD dwType = *(DWORD *)(pointMem + 8);
    pointMem += 12;

    // Use the mmioFOURCC macro from the Windows SDK to verify
    // that this is a RIFF WAVE memory chunk
    if (dwRiff != MAKEFOURCC('R', 'I', 'F', 'F'))
      return false;      // not even RIFF

    if (dwType != MAKEFOURCC('W', 'A', 'V', 'E'))
      return false;      // not a WAV

    // Get the pointer to the end of our wave memory
    BYTE *pdwEnd = pointMem + dwLength - 4;

    // Walk through the bytes and locate the tags
    while (pointMem < pdwEnd)
    {
        memcpy(&dwType, pointMem, sizeof(DWORD));
        memcpy(&dwLength, pointMem+4, sizeof(DWORD));
        pointMem += 8;

      switch (dwType)
        {
        // Located the format portion
        case MAKEFOURCC('f', 'm', 't', ' '):

            if (dwLength < WAVEFORMAT_HEADER_SIZE-2)
              return false; // Drop out because this is not a WAV

                memcpy(&m_NumChannels, pointMem + 2, sizeof(WORD));
                memcpy(&m_SampleRate, pointMem + 4, sizeof(DWORD));
                memcpy(&m_BitsPerSample, pointMem + 14, sizeof(WORD));

            // See if we have the bits and the size of the
            // bits filled out yet. If they are, then we have fully parsed
            // the memory and can return happy
            if (m_pWaveData && m_DataSize!=0)
            {
              return true;
            }

          break;

        // We are at the the samples - fill the values to return 
		// for the sample and size
        case MAKEFOURCC('d', 'a', 't', 'a'):

          if (m_pWaveData == NULL || m_DataSize == 0)
            {
            // Set the samples pointer to point to this location
            // in the memory.
               m_pWaveData = pointMem;

            // Set the size of the wave
               m_DataSize = dwLength;

            // If the header pointer is filled, we can return happy..
            if (m_NumChannels!=0 && m_SampleRate!=0 && m_BitsPerSample!=0)
              return true;
            }
          break;

        } // End case

       // Set the pointer to the next portion of memory
       pointMem = pointMem + ((dwLength+1)&~1);
      }

  // If we get to this point, we failed and did not aquire all of the
  // wave data - Return Failure.
  return false;
}

void WaveLoader::Destroy()
{
	if(m_pMemoryChunk)
	{
		delete [] m_pMemoryChunk;
		m_pMemoryChunk = NULL;
	}

	m_pWaveData = NULL;
	m_DataSize = 0;
	m_NumChannels = 0;
	m_SampleRate = 0;
	m_BitsPerSample = 0;
}

long WaveLoader::AddRef()
{
	return m_RefCount > 0 ? ++m_RefCount : 0;
}

long WaveLoader::Release()
{
	long count = m_RefCount--;
	if(count == 0) delete this;
	return count;
}