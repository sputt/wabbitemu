#ifndef SOUND_H
#define SOUND_H

#include "core.h"

#define	SAMPLE_RATE			(48000)
#define CHANNELS			(2)
#define SAMPLE_SIZE			(1)
#define PREFERED_SAMPLES	(4096)
#define BUFFER_BANKS		(4)


#define BankTime			(((float) PREFERED_SAMPLES) / ((float) SAMPLE_RATE))
#define SampleLength		((1.0f) / ((float) SAMPLE_RATE))
#define SampleSizeBits		(SAMPLE_SIZE << 3)
#define BankSize			(PREFERED_SAMPLES * CHANNELS * SAMPLE_SIZE)

#define BUFFER_SMAPLES		(SAMPLE_RATE)
#define AUDIO_BUFFER_SIZE	(BUFFER_SMAPLES * CHANNELS * SAMPLE_SIZE)


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
#ifdef WINVER
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfx;

	WAVEHDR waveheader[BUFFER_BANKS];
#endif
	SAMPLE_t playbuf[BUFFER_BANKS][PREFERED_SAMPLES];


	SAMPLE_t buffer[BUFFER_SMAPLES];

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
	timerc *timer_c;

} AUDIO_t;


int soundinit(void *);
int playsound(AUDIO_t *);
int pausesound(AUDIO_t *);
void togglesound(AUDIO_t *);
int FlippedLeft(CPU_t *, int );
int FlippedRight(CPU_t *, int );
int nextsample(CPU_t *);
void KillSound(AUDIO_t *);

#endif
