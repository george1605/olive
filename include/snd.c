#ifdef _WIN32
#include <windows.h>
#include <mmreg.h>
#include <stdint.h>
#define BASE_FREQ 44100
#define SAMPLE_RATE 11025
#define BSIZE 4096
#define M_PI 3.1415926

WAVEFORMATEX ol_waveformat(int freq, int chann)
{
    WAVEFORMATEX w;
    w.nChannels = chann;
    w.nAvgBytesPerSec = chann * freq * 2;
    w.nSamplesPerSec = 44100;       // Sample rate (e.g., 44100 Hz)
    w.nBlockAlign = 2;              // Block alignment (channels * bits/sample / 8)
    w.wBitsPerSample = 16;
    w.wFormatTag = WAVE_FORMAT_PCM;
    return w;
}

WAVEHDR ol_wavehdr(uint16_t * data)
{
    WAVEHDR waveHeader;
    waveHeader.lpData = (LPSTR)data;
    waveHeader.dwBufferLength = // Length of your sound data buffer
    waveHeader.dwFlags = 0;
    return waveHeader;
}

// play via PC Speaker
void ol_play_array(uint16_t* array, int size)
{
    for(int i = 0;i < size;i++)
        Beep(array[i], 20);
}

void ol_sound_sine(char pBuffer[], int iFreq)
{
    static double fAngle = 0; // Starting phase of the sine wave from 0 to 2*PI
    int i;                    // Loop counter from 0 to number of samples

    for (i = 0; i < BSIZE; ++i)
    {
	pBuffer[i] = (char) (127 + 127 * sin(fAngle));
	fAngle += 2 * M_PI * iFreq / SAMPLE_RATE;
	if (fAngle > 2 * M_PI)
	    fAngle -= 2 * M_PI;
    }
}

#elif defined(__linux__)
// TO DO HERE


#endif