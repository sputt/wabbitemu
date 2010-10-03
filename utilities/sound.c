#include "stdafx.h"

#include "core.h"
#include "calc.h"
#include "sound.h"
#include "link.h"
#include "gif.h"



static void CALLBACK FillSoundBuffer(HWAVEOUT hWaveOut,
									 UINT uMsg,
									 DWORD dwInstance,
									 DWORD dwParam1,
									 DWORD dwParam2 ) {

	WAVEHDR* waveheader = (WAVEHDR*) dwParam1;
	AUDIO_t* audio = (AUDIO_t*)dwInstance;
	int i;

	switch(uMsg) {
		case WOM_DONE:
		{

			waveOutUnprepareHeader(audio->hWaveOut,waveheader,sizeof(WAVEHDR));

			if ((!audio->enabled) || (!calcs[gslot].running) ) {
				if (audio->init) {
					memset(waveheader->lpData,0x80,BankSize);
					waveOutPrepareHeader(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
					waveOutWrite(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
				} else audio->endsnd++;
			} else {

				if ( ( audio->PlayTime+(BankTime*1.5f) ) < ( tc_elapsed(calcs[gslot].cpu.timer_c) ) ) {

					if ( ( audio->PlayTime+(BankTime*((float)(BufferBanks*2))) ) < tc_elapsed(calcs[gslot].cpu.timer_c) ) {

						audio->PlayTime = tc_elapsed(calcs[gslot].cpu.timer_c)-(BankTime*((float)BufferBanks));
						audio->PlayPnt = (audio->CurPnt-(PreferedSamples*BufferBanks))%BufferSamples;
					}
					unsigned char* dataout	= (unsigned char*)&audio->buffer[audio->PlayPnt];
					unsigned char* datain	= (unsigned char*)waveheader->lpData;
					unsigned char* dataend	= (unsigned char*)&audio->buffer[BufferSamples];
					for(i=0;i<BankSize;i++) {
						if ( dataout >= dataend) {
							dataout = (unsigned char*)&audio->buffer[0];
						}
						datain[i] = dataout[0];
						dataout++;
					}
					waveheader->dwFlags = 0;
					waveOutPrepareHeader(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
					waveOutWrite(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
					audio->PlayPnt = (audio->PlayPnt + PreferedSamples) % BufferSamples;
					audio->PlayTime += BankTime;
					if (gif_write_state == GIF_FRAME) {
						//WriteAVIAudioFrame(datain, BankSize);
					}
				} else {

					memset(waveheader->lpData,0x80,BankSize);
					waveOutPrepareHeader(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
					waveOutWrite(audio->hWaveOut,waveheader,sizeof(WAVEHDR));
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


int soundinit() {
	AUDIO_t* audio = calcs[gslot].audio;
	int i,b;
	
	puts("Sound intial");
	
	audio->init		= 0;
	audio->enabled	= 0;


	for(i=0;i<BufferSamples;i++) {
		audio->buffer[i].left = 0x80;
		audio->buffer[i].right = 0x80;
	}


	for(b=0;b<BufferBanks;b++) {
		for(i=0;i<PreferedSamples;i++) {
			audio->playbuf[b][i].left = 0x80;
			audio->playbuf[b][i].right = 0x80;
		}
	}


	audio->PlayPnt			= 0;
	audio->CurPnt			= BufferBanks*PreferedSamples;

	audio->PlayTime			= tc_elapsed(calcs[gslot].cpu.timer_c)-(((float)BufferBanks)*((float)PreferedSamples)) /((float)SampleRate);
	audio->LastFlipLeft		= tc_elapsed(calcs[gslot].cpu.timer_c);
	audio->HighLengLeft		= 0;
	audio->LastFlipRight	= tc_elapsed(calcs[gslot].cpu.timer_c);
	audio->HighLengRight	= 0;
	audio->LastSample		= tc_elapsed(calcs[gslot].cpu.timer_c);
	audio->LeftOn			= 0;
	audio->RightOn			= 0;
	
	audio->volume			= 0.33f;


	audio->wfx.nSamplesPerSec	= SampleRate;
	audio->wfx.wBitsPerSample	= SampleSizeBits;
	audio->wfx.nChannels		= Channels;
	audio->wfx.cbSize			= 0;
	audio->wfx.wFormatTag		= WAVE_FORMAT_PCM;
	audio->wfx.nBlockAlign		= (audio->wfx.wBitsPerSample >> 3) * audio->wfx.nChannels;
	audio->wfx.nAvgBytesPerSec	= audio->wfx.nBlockAlign * audio->wfx.nSamplesPerSec;



	if( waveOutOpen(	&audio->hWaveOut, 
						WAVE_MAPPER, 
						&audio->wfx, 
						(DWORD_PTR)FillSoundBuffer, 
						(DWORD)audio, 
						CALLBACK_FUNCTION
					) != MMSYSERR_NOERROR ) {

		audio->enabled		= 0;
	    MessageBox(NULL, _T("Unable to open audio device."), _T("Error"), MB_OK);
	    return 1;
	}
	
	audio->init =1;
	audio->enabled =1;

	for(i=0;i<BufferBanks;i++) {	
		audio->waveheader[i].lpData				= (char*)audio->playbuf[i];
		audio->waveheader[i].dwBufferLength		= BankSize;
		audio->waveheader[i].dwFlags			= 0;
		waveOutPrepareHeader(audio->hWaveOut,&audio->waveheader[i],sizeof(WAVEHDR));
		waveOutWrite(audio->hWaveOut,&audio->waveheader[i],sizeof(WAVEHDR));
	}


	return 0;
}

void KillSound(AUDIO_t* audio) {
	if (audio == NULL)
	{
		return;
	}
	if (audio->init) {
		int i;
		audio->endsnd = 0;
		audio->enabled	= FALSE;
		audio->init = 0;
		for(i=0;audio->endsnd<BufferBanks && i<200;i++) Sleep(5);
		waveOutClose(audio->hWaveOut);
		for(i=0;audio->endsnd<100 && i<200;i++) Sleep(5);
	}
}
	


void togglesound() {
	if (calcs[gslot].audio->enabled) pausesound();
	else playsound();
}

int playsound() {
	AUDIO_t * audio = calcs[gslot].audio;
	if (audio->init == 0) {
		soundinit();
	} else {
		int i,b;
		audio->PlayTime = tc_elapsed(calcs[gslot].cpu.timer_c)-(BankTime*((float)BufferBanks));
		audio->PlayPnt = (audio->CurPnt-(PreferedSamples*BufferBanks))%BufferSamples;
		for(b=0;b<BufferBanks;b++) {
			for(i=0;i<PreferedSamples;i++) {
				audio->playbuf[b][i].left = 0x80;
				audio->playbuf[b][i].right = 0x80;
			}
		}
		waveOutRestart(audio->hWaveOut);
		audio->enabled =1;
	}
	return 0;
}

int pausesound() {
	AUDIO_t * audio = calcs[gslot].audio;
	if (audio->init == 0) return 0;
	audio->enabled =0;
	waveOutPause(audio->hWaveOut);
	return 0;
}

int FlippedLeft(CPU_t *cpu, int on) {
	link_t* link = cpu->pio.link;
	AUDIO_t* audio = &link->audio;
	if (!audio->enabled) return 1;
	if (on == 1) {
		audio->LastFlipLeft = tc_elapsed(calcs[gslot].cpu.timer_c);
	} else if (on == 0) {
		audio->HighLengLeft += (tc_elapsed(calcs[gslot].cpu.timer_c)-audio->LastFlipLeft);
	}
	audio->LeftOn = on;
	return 0;
}

int FlippedRight(CPU_t *cpu, int on) {
	link_t* link = cpu->pio.link;
	AUDIO_t* audio = &link->audio;
	if (!audio->enabled) return 1;
	if (on == 1) {
		audio->LastFlipRight = tc_elapsed(calcs[gslot].cpu.timer_c);
	} else if (on == 0) {
		audio->HighLengRight += (tc_elapsed(calcs[gslot].cpu.timer_c)-audio->LastFlipRight);
	}
	audio->RightOn = on;
	return 0;
}

int nextsample(CPU_t *cpu) {
	link_t* link = cpu->pio.link;
	AUDIO_t* audio = &link->audio;
	double tmp;
	double max		=	255.0f * audio->volume;
	double lower	=	(255.0f - max)/2.0f;
	
	unsigned char left;
	unsigned char right;
	if (!audio->enabled) return 1;
	
	if (tc_elapsed(calcs[gslot].cpu.timer_c) < (audio->LastSample+SampleLength)) return 0;
	
	if (audio->RightOn == 1) {
		if ( (audio->LastSample+SampleLength)>audio->LastFlipRight) {
			audio->HighLengRight += ((audio->LastSample+SampleLength)-audio->LastFlipRight);
			audio->LastFlipRight = audio->LastSample+SampleLength;
		} 			
	}
	
	if (audio->LeftOn == 1) {
		if ( (audio->LastSample+SampleLength)>audio->LastFlipLeft) {
			audio->HighLengLeft += ((audio->LastSample+SampleLength)-audio->LastFlipLeft);
			audio->LastFlipLeft = audio->LastSample+SampleLength;
		}
	}

	if (audio->HighLengLeft < 0) {
		puts("Left less than 0");
		audio->HighLengLeft = 0;
	}
	if (audio->HighLengLeft > SampleLength) {
//		printf("Left %Lf > %Lf \n",(double)audio->HighLengLeft,(double)SampleLength);
		audio->HighLengLeft = SampleLength;
	}

	if (audio->HighLengRight < 0) {
		puts("right less than 0");
		audio->HighLengRight = 0;
	}
	if (audio->HighLengRight > SampleLength) {
		audio->HighLengRight =SampleLength;
//		puts("right greater than Sample length");
	}

	tmp = (audio->HighLengLeft*max*SampleRate)+lower;
	if (tmp < 0) {
		puts("Left less than 0");
		tmp=0;
	}
	if (tmp > 255) {
		puts("Left greater than 255");
		tmp=255;
	}
	left = (unsigned char) tmp;
	
	tmp = (audio->HighLengRight*max*SampleRate)+lower;
	if (tmp < 0) {
		puts("Right less than 0");
		tmp = 0;
	}
	if (tmp > 255) {
		puts("Right greater than 255");
		tmp=255;
	}
	right = (unsigned char) tmp;

	audio->buffer[audio->CurPnt].left		=left;
	audio->buffer[audio->CurPnt].right		=right;

	audio->CurPnt		=  (audio->CurPnt+1)%BufferSamples;

	audio->HighLengRight	=	0;
	audio->HighLengLeft		=	0;
	audio->LastSample		+=	SampleLength;

	if ( (audio->LastSample+(SampleLength*2.0f)) < tc_elapsed(calcs[gslot].cpu.timer_c)) {
		puts("Last sample out of sync");
		audio->LastSample = tc_elapsed(calcs[gslot].cpu.timer_c);
	}

	return 0;
}
		
	

