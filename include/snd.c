#ifndef SOUND_
#define SOUND_
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

typedef struct {
    char     riff[4];        // "RIFF" constant
    uint32_t fileSize;       // Size of the entire file minus 8 bytes
    char     wave[4];        // "WAVE" constant
    char     fmt[4];         // "fmt " constant
    uint32_t fmt_size;        // Size of the format chunk (16 bytes for PCM)
    uint16_t audio_format;    // Audio format (1 for PCM)
    uint16_t num_channels;    // Number of audio channels (1 for mono, 2 for stereo)
    uint32_t sample_rate;     // Sample rate (e.g., 44100 Hz)
    uint32_t byte_rate;       // Byte rate (SampleRate * NumChannels * BitsPerSample / 8)
    uint16_t block_align;     // Block align (NumChannels * BitsPerSample / 8)
    uint16_t sample_size;  // Bits per sample (e.g., 16 bits)
    char     data[4];        // "data" constant
    uint32_t data_size;       // Size of the data chunk
} OlWavHead;

typedef struct {
    void* bytes;
    size_t size;
    OlWavHead info;
} OlSoundBuffer;

void ol_play_buffer(OlSoundBuffer buf); // various implementations below

int ol_loadsnd_buffer(char* fname, OlSoundBuffer* buf)
{
    FILE* fp = fopen(fname, "rb");
    OlWavHead head;
    fread(&head, 1, sizeof(OlWavHead), fp);
    /*if(head.audio_format < 1)
    {
        fclose(fp);
        return -1;
    }*/
    buf->bytes = malloc(head.data_size);
    buf->size = head.data_size;
    buf->info = head;
    fread(buf->bytes, head.data_size, 1, fp);
    fclose(fp);
    return 0;
}

int ol_play_wav(char* fname) {
    OlSoundBuffer buf;
    if(ol_loadsnd_buffer(fname, &buf) == -1)
        return -1;
    ol_play_buffer(buf);
    free(buf.bytes);
    return 0;
}

#ifdef _WIN32
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>
#define BASE_FREQ 44100
#define SAMPLE_RATE 11025
#define BSIZE 4096

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

HWAVEOUT ol_wave_opendev(WAVEFORMATEX*  w)
{
    HWAVEOUT hWaveOut;
    waveOutOpen(NULL, WAVE_MAPPER, w, 0, 0, WAVE_FORMAT_QUERY);
    waveOutOpen(&hWaveOut, WAVE_MAPPER, w, 0, 0, CALLBACK_NULL);
    return hWaveOut;
}

/*
 * Windows implementation of ol_play_buffer
 */
void ol_play_buffer(OlSoundBuffer buf) {
    WAVEFORMATEX w;
    w.nChannels = buf.info.num_channels;
    w.nAvgBytesPerSec = buf.info.byte_rate;
    w.wFormatTag = WAVE_FORMAT_PCM;
    w.wBitsPerSample = buf.info.sample_rate;
    w.nBlockAlign = buf.info.block_align;
    HWAVEOUT dev = ol_wave_opendev(&w);

    WAVEHDR waveHeader;
    waveHeader.lpData = (LPSTR)buf.bytes;
    waveHeader.dwBufferLength = buf.size;
    waveHeader.dwBytesRecorded = 0;
    waveHeader.dwUser = 0;
    waveHeader.dwFlags = 0;
    waveHeader.dwLoops = 0;
    waveOutPrepareHeader(dev, &waveHeader, sizeof(WAVEHDR));
    waveOutWrite(dev, &waveHeader, sizeof(WAVEHDR));
    waveOutUnprepareHeader(dev, &waveHeader, sizeof(WAVEHDR));
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void ol_simple_sound(int freq, int duration)
{
    int value = (1193180 + freq / 2) / freq;
    int fd = open("/dev/console", O_RDWR);
    struct kbentry kb = {
        1,  // Click type (0 = off, 1 = on)
        value,         // Value for frequency (0 to 65535)
        duration,      // Duration in ms
    };
    if (ioctl(fd, KDMKTONE, &kb) < 0) {
        perror("Error in ioctl");
        close(fd);
        return;
    }
    close(fd);
}

#endif

#endif