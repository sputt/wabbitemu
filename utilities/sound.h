#ifndef SOUND_H
#define SOUND_H

#include "core.h"

#define	SampleRate		(48000)
#define Channels		(2)
#define SampleSize		(1)
#define PreferedSamples	(4096)
#define BufferBanks		(4)


#define BankTime		(((float)PreferedSamples)/((float)SampleRate))
#define SampleLength	((1.0f)/((float)SampleRate))
#define SampleSizeBits	(SampleSize<<3)
#define BankSize		(PreferedSamples*Channels*SampleSize)

#define BufferSamples	(SampleRate)
#define BufferSize		(BufferSamples*Channels*SampleSize)


typedef struct SAMPLE SAMPLE_t;

#pragma pack(1)
struct SAMPLE {
	unsigned char left;
	unsigned char right;
};

#pragma pack()


typedef struct {
	int init;
	int enabled;
	volatile int endsnd;
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfx;

	WAVEHDR waveheader[BufferBanks];
	SAMPLE_t playbuf[BufferBanks][PreferedSamples];


	SAMPLE_t buffer[BufferSamples];

	int CurPnt;
	int PlayPnt;
	
	double PlayTime;
	double LastSample;
	
	int LeftOn;
	double LastFlipLeft;
	double HighLengLeft;
	
	int RightOn;
	double LastFlipRight;
	double HighLengRight;
	
	double volume;

} AUDIO_t;

int soundinit();
int playsound();
int pausesound();
void togglesound();
int FlippedLeft(CPU_t *, int );
int FlippedRight(CPU_t *, int );
int nextsample(CPU_t *);
void KillSound(AUDIO_t* );

#endif
