#include <windows.h>
#include <Aviriff.h>
#include <string.h>
#include <stdio.h>
#include <mmsystem.h>

#include "calc.h"
#include "lcd.h"
#include "gif.h"

#pragma pack(push, 1)
typedef struct {
	FOURCC fcc;			// RIFF
	DWORD cbFileSize;
	FOURCC fccType;		// AVI 
} RIFFHEADER;



typedef struct {
	FOURCC chkID;
	DWORD cbSize;
	BYTE ckData[0];
} CHUNK;

typedef struct {
	FOURCC fcc;
	DWORD cbSize;
	FOURCC fccType;
	BYTE lstData[0];
} LIST;

typedef struct {
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
} AVIINDEXENTRY;

#pragma pack(pop)

extern BITMAPINFO *bi;

static int frame_count = 0;
static int audio_frame_count = 0;
static FILE *file;


const int amh_framecount = 0x30;
const int ash_framecount = 0x8C;
static int movi_len;

unsigned char *index;

int WriteRIFFHeader() {
	
	RIFFHEADER rf;
	rf.fcc = 'FFIR'; //'RIFF';
	rf.cbFileSize = 0;
	rf.fccType = ' IVA'; //'AVI ';

	LIST *hdrl = malloc(sizeof(LIST) + sizeof(AVIMAINHEADER));
	AVIMAINHEADER amh;
	
	amh.fcc = 'hiva'; //'avih';
	amh.cb = sizeof(AVIMAINHEADER) - 8;
	amh.dwMicroSecPerFrame = 100000;
	amh.dwMaxBytesPerSec = 512000;
	amh.dwPaddingGranularity = 0;
	amh.dwFlags = 0x10;
	amh.dwTotalFrames = frame_count; //dunno 
	amh.dwInitialFrames = 0;
	amh.dwStreams = 2;
	amh.dwSuggestedBufferSize = 0;
	amh.dwWidth = 96;
	amh.dwHeight = 64;
	ZeroMemory(amh.dwReserved, sizeof(amh.dwReserved));
		
	hdrl->fcc = 'TSIL'; //'LIST';
	hdrl->cbSize = 3*sizeof(LIST) + (2*sizeof(AVISTREAMHEADER)) + (sizeof(CHUNK)+sizeof(BITMAPINFOHEADER)) + (sizeof(CHUNK)+sizeof(WAVEFORMATEX)) + sizeof(AVIMAINHEADER) - 8;
	hdrl->fccType = 'lrdh'; //'hdrl';
	memcpy(hdrl->lstData, &amh, sizeof(AVIMAINHEADER));
	
	LIST *strl = malloc(sizeof(LIST) + sizeof(AVISTREAMHEADER) + 2*(sizeof(CHUNK)+sizeof(BITMAPINFOHEADER)));
	AVISTREAMHEADER ash;
	
	ash.fcc = 'hrts'; //'strh';
	ash.cb = sizeof(ash) - 8;
	ash.fccType = 'sdiv'; //'vids';
	ash.fccHandler = ' BID';
	ash.dwFlags = 0;
	ash.wPriority = 0;
	ash.wLanguage = 0;
	ash.dwInitialFrames = 0;
	ash.dwScale = 1;
	ash.dwRate = 1000/ (gif_base_delay*10); //FPS
	ash.dwStart = 0;
	ash.dwLength = frame_count; //dunno
	ash.dwSuggestedBufferSize = 18432;
	ash.dwQuality = -1;
	ash.dwSampleSize = 0;
	ash.rcFrame.left = 0;
	ash.rcFrame.top = 0;
	ash.rcFrame.right = 96;
	ash.rcFrame.bottom = 64;
	
	// Define the bitmap chunk
	CHUNK *ckBitmap = malloc(sizeof(CHUNK) + sizeof(BITMAPINFOHEADER));
	ckBitmap->chkID = 'frts'; //'strf';
	ckBitmap->cbSize = sizeof(BITMAPINFOHEADER);
	
	BITMAPINFOHEADER lbi = bi->bmiHeader;
	
	lbi.biBitCount = 24;
	lbi.biWidth = 96;
	lbi.biHeight = -64;
	lbi.biSizeImage = 96*64*3;
	lbi.biClrUsed = 0;
	lbi.biClrImportant = 0;
	memcpy(ckBitmap->ckData, &lbi, sizeof(BITMAPINFOHEADER));
	
	strl->fcc = 'TSIL'; //'LIST';
	strl->cbSize = sizeof(LIST) + sizeof(AVISTREAMHEADER) + (sizeof(CHUNK)+sizeof(BITMAPINFOHEADER)) - 8;
	strl->fccType = 'lrts'; //'strl';
	memcpy(&strl->lstData[0], &ash, sizeof(AVISTREAMHEADER));
	memcpy(&strl->lstData[sizeof(AVISTREAMHEADER)], ckBitmap, sizeof(CHUNK) + sizeof(BITMAPINFOHEADER));
	
	
	// Write out what we have so far
	file = fopen("output.avi", "wb");
	
	fwrite(&rf, sizeof(RIFFHEADER), 1, file);
	fwrite(hdrl,sizeof(LIST) + sizeof(AVIMAINHEADER), 1, file);
	fwrite(strl, sizeof(LIST) + sizeof(AVISTREAMHEADER) + (sizeof(CHUNK)+sizeof(BITMAPINFOHEADER)), 1, file);
	
	
	// DO the wave file stream header
	
	CHUNK *ckWave = malloc(sizeof(CHUNK) + sizeof(WAVEFORMATEX));
	ckWave->chkID = 'frts';
	ckWave->cbSize = sizeof(WAVEFORMATEX);
	memcpy(ckWave->ckData, &calcs[gslot].audio->wfx, sizeof(WAVEFORMATEX));
	
	ash.fccType = 'sdua'; //'auds';
	ash.dwFlags = 0;
	ash.wPriority = 0;
	ash.wLanguage = 0;
	ash.dwInitialFrames = 0;
	ash.dwSampleSize = calcs[gslot].audio->wfx.nBlockAlign;
	ash.dwRate = 48000;
	ash.dwLength = calcs[gslot].audio->wfx.nSamplesPerSec*calcs[gslot].audio->wfx.wBitsPerSample*calcs[gslot].audio->wfx.nChannels;
	
	ash.fccHandler = 0;
	
	strl->cbSize = sizeof(LIST) + sizeof(AVISTREAMHEADER) + (sizeof(CHUNK)+sizeof(WAVEFORMATEX)) - 8;
	memcpy(&strl->lstData[0], &ash, sizeof(AVISTREAMHEADER));
	memcpy(&strl->lstData[sizeof(AVISTREAMHEADER)], ckWave, sizeof(CHUNK) + sizeof(WAVEFORMATEX));
	
	fwrite(strl, sizeof(LIST) + sizeof(AVISTREAMHEADER) + (sizeof(CHUNK)+sizeof(WAVEFORMATEX)), 1, file);
	
	CHUNK *ckJunk = malloc(sizeof(CHUNK) + 7972);
	ckJunk->chkID = 'KNUJ';
	ckJunk->cbSize = 7972;
	memset(ckJunk->ckData, 0xFF, 7972);
	
	
	LIST *movi = malloc(sizeof(LIST));

	
	movi->fcc = 'TSIL';
	movi->cbSize = sizeof(LIST) + (sizeof(CHUNK) + 96*64*3)*frame_count - 8;
	movi->fccType = 'ivom'; //'movi';
	
	

	fwrite(ckJunk, sizeof(CHUNK) + ckJunk->cbSize, 1, file);
	movi_len = ftell(file) + 4;
	fwrite(movi, sizeof(LIST), 1, file);
	
	index = malloc(512000);
	
	return 0;
}


int WriteRIFFIndex() {
	
	CHUNK ckIdx;
	ckIdx.chkID = '1xdi'; //idx1
	ckIdx.cbSize = sizeof(AVIINDEXENTRY)*(frame_count+audio_frame_count);
	
	fwrite(&ckIdx, sizeof(CHUNK), 1, file);
	
	fwrite(index, sizeof(AVIINDEXENTRY), frame_count+audio_frame_count, file);
	
	
	long length = ftell(file) - 8;
	
	fseek(file, 4, SEEK_SET);
	fwrite(&length, 4, 1, file);
	
	length = frame_count;
	fseek(file, amh_framecount, SEEK_SET);
	fwrite(&length, 4, 1, file);
	
	fseek(file, ash_framecount, SEEK_SET);
	fwrite(&length, 4, 1, file);
	
	fseek(file, movi_len, SEEK_SET);
	length = sizeof(LIST) + (sizeof(CHUNK) + 96*64*3)*frame_count + 
		(audio_frame_count*(sizeof(CHUNK) + 4096*calcs[gslot].audio->wfx.nChannels))- 8;
	fwrite(&length, 4, 1, file);
	
	fclose(file);
}


int WriteAVIFrame() {
	CHUNK *ckFrame;
	ckFrame = malloc(sizeof(CHUNK) + 96*64*3);
	
	ckFrame->chkID = 'bd00'; //'00db';
	ckFrame->cbSize = 96*64*3;
	
	unsigned char *gray = LCD_image(calcs[gslot].cpu.pio.lcd);
	int x, y;
	for (y = 0; y < 64; y++) {
		for (x = 0; x < 96; x++) {
			RGBQUAD rgb = bi->bmiColors[gray[y*128+x]];
			ckFrame->ckData[(y*96*3)+(x*3)+0] = rgb.rgbBlue;
			ckFrame->ckData[(y*96*3)+(x*3)+1] = rgb.rgbGreen;
			ckFrame->ckData[(y*96*3)+(x*3)+2] = rgb.rgbRed;
			
		}
	}
	
	AVIINDEXENTRY aie;
	
	aie.ckid = ckFrame->chkID;
	aie.dwChunkLength = ckFrame->cbSize;
	aie.dwChunkOffset = movi_len+8+(sizeof(CHUNK)+96*64*3)*frame_count + (sizeof(CHUNK)+4096*calcs[gslot].audio->wfx.nChannels)*audio_frame_count;
	aie.dwFlags = AVIIF_KEYFRAME;
	
	memcpy(&index[sizeof(AVIINDEXENTRY)*(frame_count+audio_frame_count)], &aie, sizeof(aie));
	
	frame_count++;
	fwrite(ckFrame, sizeof(CHUNK)+96*64*3, 1, file);
	free(ckFrame);
}

void WriteAVIAudioFrame(unsigned char* data, int size) {
	CHUNK *ckFrame;
	ckFrame = malloc(sizeof(CHUNK) + size);
	
	ckFrame->chkID = 'bw10'; //'01wb';
	ckFrame->cbSize = size;
	
	fwrite(ckFrame, sizeof(CHUNK), 1, file);
	fwrite(data, size, 1, file);
	
	AVIINDEXENTRY aie;
	
	aie.ckid = ckFrame->chkID;
	aie.dwChunkLength = ckFrame->cbSize;
	aie.dwChunkOffset = movi_len+8+(sizeof(CHUNK)+96*64*3)*frame_count + (sizeof(CHUNK)+4096*calcs[gslot].audio->wfx.nChannels)*audio_frame_count;
	aie.dwFlags = 0;
	
	memcpy(&index[sizeof(AVIINDEXENTRY)*(frame_count+audio_frame_count)], &aie, sizeof(aie));
	
	audio_frame_count++;
	free(ckFrame);
}