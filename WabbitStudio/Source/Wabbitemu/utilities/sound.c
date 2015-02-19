#include "stdafx.h"

#include "core.h"
#include "calc.h"
#include "sound.h"
#include "link.h"
#include "gif.h"


#ifdef _WINDOWS
static void CALLBACK FillSoundBuffer(HWAVEOUT,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR) {

	WAVEHDR* waveheader = (WAVEHDR*)dwParam1;
	AUDIO_t *audio = (AUDIO_t *)dwInstance;

	switch (uMsg) {
	case WOM_DONE:
	{
		waveOutUnprepareHeader(audio->hWaveOut, waveheader, sizeof(WAVEHDR));

		if (!audio->enabled) {
			if (audio->init) {
				memset(waveheader->lpData, 0x80, BANK_SIZE);
				waveOutPrepareHeader(audio->hWaveOut, waveheader, sizeof(WAVEHDR));
				waveOutWrite(audio->hWaveOut, waveheader, sizeof(WAVEHDR));
			} else audio->endsnd++;
		} else {
			if ((audio->PlayTime + (BANK_TIME * 1.5f)) < (audio->timer_c->elapsed)) {
				if ((audio->PlayTime + (BANK_TIME * ((float)(BUFFER_BANKS * 2)))) < audio->timer_c->elapsed) {

					audio->PlayTime = audio->timer_c->elapsed - (BANK_TIME * ((float)BUFFER_BANKS));
					audio->PlayPnt = (audio->CurPnt - (PREFERED_SAMPLES * BUFFER_BANKS)) % BUFFER_SMAPLES;
				}

				unsigned char *dataout = (unsigned char *)&audio->buffer[audio->PlayPnt];
				unsigned char *datain = (unsigned char *)waveheader->lpData;
				unsigned char *dataend = (unsigned char *)&audio->buffer[BUFFER_SMAPLES];
				for (int i = 0; i < BANK_SIZE; i++) {
					if (dataout >= dataend) {
						dataout = (unsigned char *)&audio->buffer[0];
					}

					datain[i] = dataout[0];
					dataout++;
				}

				waveheader->dwFlags = 0;
				waveOutPrepareHeader(audio->hWaveOut, waveheader, sizeof(WAVEHDR));

				audio->audio_frame_callback(audio->cpu);

				waveOutWrite(audio->hWaveOut, waveheader, sizeof(WAVEHDR));
				audio->PlayPnt = (audio->PlayPnt + PREFERED_SAMPLES) % BUFFER_SMAPLES;
				audio->PlayTime += BANK_TIME;
			} else {
				memset(waveheader->lpData, 0x80, BANK_SIZE);
				waveOutPrepareHeader(audio->hWaveOut, waveheader, sizeof(WAVEHDR));
				waveOutWrite(audio->hWaveOut, waveheader, sizeof(WAVEHDR));
			}

		}
		break;
	}
	case WOM_OPEN:
	{
		puts("WOM_OPEN");
		break;
	}
	case WOM_CLOSE:
	{
		puts("WOM_CLOSE");
		audio->endsnd = 100;
		break;
	}
	default:
	{
		puts("sound callback msg unknown");
		break;
	}
	}
	return;
}

#endif

int soundinit(AUDIO_t *audio) {
	int i, b;

	puts("Sound initialize");

	audio->init = 0;
	audio->enabled = 0;


	for (i = 0; i < BUFFER_SMAPLES; i++) {
		audio->buffer[i].left = 0x80;
		audio->buffer[i].right = 0x80;
	}


	for (b = 0; b < BUFFER_BANKS; b++) {
		for (i = 0; i < PREFERED_SAMPLES; i++) {
			audio->playbuf[b][i].left = 0x80;
			audio->playbuf[b][i].right = 0x80;
		}
	}


	audio->PlayPnt = 0;
	audio->CurPnt = BUFFER_BANKS * PREFERED_SAMPLES;

	audio->PlayTime = audio->timer_c->elapsed - (((float)BUFFER_BANKS) * ((float)PREFERED_SAMPLES)) / ((float)SAMPLE_RATE);
	audio->LastFlipLeft = audio->timer_c->elapsed;
	audio->HighLengLeft = 0;
	audio->LastFlipRight = audio->timer_c->elapsed;
	audio->HighLengRight = 0;
	audio->LastSample = audio->timer_c->elapsed;
	audio->LeftOn = 0;
	audio->RightOn = 0;

	audio->volume = 0.33f;

#ifdef _WINDOWS
	audio->wfx.nSamplesPerSec = SAMPLE_RATE;
	audio->wfx.wBitsPerSample = SAMPLE_SIZE_BITS;
	audio->wfx.nChannels = CHANNELS;
	audio->wfx.cbSize = 0;
	audio->wfx.wFormatTag = WAVE_FORMAT_PCM;
	audio->wfx.nBlockAlign = (audio->wfx.wBitsPerSample >> 3) * audio->wfx.nChannels;
	audio->wfx.nAvgBytesPerSec = audio->wfx.nBlockAlign * audio->wfx.nSamplesPerSec;



	if (waveOutOpen(&audio->hWaveOut,
		WAVE_MAPPER,
		&audio->wfx,
		(DWORD_PTR)FillSoundBuffer,
		(DWORD_PTR)audio,
		CALLBACK_FUNCTION
		) != MMSYSERR_NOERROR) {

		audio->enabled = 0;
		MessageBox(NULL, _T("Unable to open audio device."), _T("Error"), MB_OK);
		return 1;
	}

	audio->init = 1;
	audio->enabled = 1;

	for (i = 0; i < BUFFER_BANKS; i++) {
		audio->waveheader[i].lpData = (char *)audio->playbuf[i];
		audio->waveheader[i].dwBufferLength = BANK_SIZE;
		audio->waveheader[i].dwFlags = 0;
		waveOutPrepareHeader(audio->hWaveOut, &audio->waveheader[i], sizeof(WAVEHDR));
		waveOutWrite(audio->hWaveOut, &audio->waveheader[i], sizeof(WAVEHDR));
	}
#endif

	return 0;
}

void KillSound(AUDIO_t* audio) {
	if (audio == NULL) {
		return;
	}
	if (audio->init) {
		int i;
		audio->endsnd = 0;
		audio->enabled = FALSE;
		audio->init = 0;
#ifdef _WINDOWS
		for (i = 0; audio->endsnd < BUFFER_BANKS && i < 200; i++) Sleep(5);
		waveOutClose(audio->hWaveOut);
		for (i = 0; audio->endsnd < 100 && i < 200; i++) Sleep(5);
#endif
	}
}

void togglesound(AUDIO_t *audio) {
	if (audio->enabled) pausesound(audio);
	else playsound(audio);
}

int playsound(AUDIO_t *audio) {
	if (audio->init == 0) {
		soundinit(audio);
	} else {
		int i, b;
		audio->PlayTime = audio->timer_c->elapsed - (BANK_TIME * ((float)BUFFER_BANKS));
		audio->PlayPnt = (audio->CurPnt - (PREFERED_SAMPLES * BUFFER_BANKS)) % BUFFER_SMAPLES;
		for (b = 0; b < BUFFER_BANKS; b++) {
			for (i = 0; i < PREFERED_SAMPLES; i++) {
				audio->playbuf[b][i].left = 0x80;
				audio->playbuf[b][i].right = 0x80;
			}
		}

#ifdef _WINDOWS
		waveOutRestart(audio->hWaveOut);
#endif
		audio->enabled = 1;
	}
	return 0;
}

int pausesound(AUDIO_t *audio) {
	if (audio->init == 0) return 0;
	audio->enabled = 0;
#ifdef _WINDOWS
	waveOutPause(audio->hWaveOut);
#endif
	return 0;
}

int FlippedLeft(CPU_t *cpu, int on) {
	link_t *link = cpu->pio.link;
	AUDIO_t *audio = &link->audio;
	if (!audio->enabled) return 1;
	if (on == 1) {
		audio->LastFlipLeft = cpu->timer_c->elapsed;
	} else if (on == 0) {
		audio->HighLengLeft += (cpu->timer_c->elapsed - audio->LastFlipLeft);
	}
	audio->LeftOn = on;
	return 0;
}

int FlippedRight(CPU_t *cpu, int on) {
	link_t* link = cpu->pio.link;
	AUDIO_t* audio = &link->audio;
	if (!audio->enabled) return 1;
	if (on == 1) {
		audio->LastFlipRight = cpu->timer_c->elapsed;
	} else if (on == 0) {
		audio->HighLengRight += (cpu->timer_c->elapsed - audio->LastFlipRight);
	}
	audio->RightOn = on;
	return 0;
}

int nextsample(CPU_t *cpu) {
	link_t* link = cpu->pio.link;
	AUDIO_t* audio = &link->audio;
	double tmp;
	double max = 255.0f * audio->volume;
	double lower = (255.0f - max) / 2.0f;

	unsigned char left;
	unsigned char right;
	if (!audio->enabled) return 1;

	if (cpu->timer_c->elapsed < (audio->LastSample + SAMPLE_LENGTH)) return 0;

	if (audio->RightOn == 1) {
		if ((audio->LastSample + SAMPLE_LENGTH) > audio->LastFlipRight) {
			audio->HighLengRight += ((audio->LastSample + SAMPLE_LENGTH) - audio->LastFlipRight);
			audio->LastFlipRight = audio->LastSample + SAMPLE_LENGTH;
		}
	}

	if (audio->LeftOn == 1) {
		if ((audio->LastSample + SAMPLE_LENGTH) > audio->LastFlipLeft) {
			audio->HighLengLeft += ((audio->LastSample + SAMPLE_LENGTH) - audio->LastFlipLeft);
			audio->LastFlipLeft = audio->LastSample + SAMPLE_LENGTH;
		}
	}

	if (audio->HighLengLeft < 0) {
		puts("Left less than 0");
		audio->HighLengLeft = 0;
	}
	if (audio->HighLengLeft > SAMPLE_LENGTH) {
		//		printf("Left %Lf > %Lf \n",(double)audio->HighLengLeft,(double)SAMPLE_LENGTH);
		audio->HighLengLeft = SAMPLE_LENGTH;
	}

	if (audio->HighLengRight < 0) {
		puts("right less than 0");
		audio->HighLengRight = 0;
	}
	if (audio->HighLengRight > SAMPLE_LENGTH) {
		audio->HighLengRight = SAMPLE_LENGTH;
		//		puts("right greater than Sample length");
	}

	tmp = (audio->HighLengLeft * max * SAMPLE_RATE) + lower;
	if (tmp < 0) {
		puts("Left less than 0");
		tmp = 0;
	}
	if (tmp > 255) {
		puts("Left greater than 255");
		tmp = 255;
	}
	left = (unsigned char)tmp;

	tmp = (audio->HighLengRight * max * SAMPLE_RATE) + lower;
	if (tmp < 0) {
		puts("Right less than 0");
		tmp = 0;
	}
	if (tmp > 255) {
		puts("Right greater than 255");
		tmp = 255;
	}
	right = (unsigned char)tmp;

	audio->buffer[audio->CurPnt].left = left;
	audio->buffer[audio->CurPnt].right = right;

	audio->CurPnt = (audio->CurPnt + 1) % BUFFER_SMAPLES;

	audio->HighLengRight = 0;
	audio->HighLengLeft = 0;
	audio->LastSample += SAMPLE_LENGTH;

	if ((audio->LastSample + (SAMPLE_LENGTH*2.0f)) < cpu->timer_c->elapsed) {
		puts("Last sample out of sync");
		audio->LastSample = cpu->timer_c->elapsed;
	}

	return 0;
}



