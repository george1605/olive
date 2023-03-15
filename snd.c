#ifdef _WIN32
#include <windows.h>
#include <mmreg.h>
#define BASE_FREQ 44100
#define SAMPLE_RATE 11025
#define BSIZE 4096
#define M_PI 3.1415926

WAVEFORMAT ol_waveformat(int freq, int chann)
{
    WAVEFORMAT w;
    w.nChannels = chann;
    w.nAvgBytesPerSec = chann * freq * 2;
    w.wFormatTag = WAVE_FORMAT_PCM;
    return w;
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

#endif