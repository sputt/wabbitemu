#include "MapHierarchy.h"
#include "MapView.h"

static int dict[1024];                         //two ints per dictionary entry
static int dict_index;
static int comp_index, comp_iter;

static void PutSymbol(unsigned short tCode, LPBYTE lpBuffer, int cbBuffer) {
    unsigned short temp = tCode;
    unsigned char small_temp;
    
	if (comp_index >= cbBuffer)
		return;

    if (comp_iter!=8) tCode <<= comp_iter;  //leave 6 slots to the right
    small_temp = (tCode & 0xFF00)>>8;
    lpBuffer[comp_index] = lpBuffer[comp_index] | small_temp;
    comp_index++;

	if (comp_index >= cbBuffer)
		return;

    small_temp = tCode & 0x00FF;
    lpBuffer[comp_index] = lpBuffer[comp_index] | small_temp;
    if (comp_iter==8) comp_index++;
    comp_iter-=2;
    if (!comp_iter) comp_iter=8;
}


int CompressMap(LPMAPVIEWSETTINGS lpmvs, LPBYTE lpBuffer, int cbBuffer) {
	unsigned short this_code,next_code,temp_code;
	int map_index;

	comp_index = 0;		//where we start
	comp_iter = 6;		//signal to compress iteration

	ZeroMemory(lpBuffer, cbBuffer);
	ZeroMemory(dict, sizeof(dict));
	dict_index = 0;

	this_code = lpmvs->pMapData[0];
	map_index = 1;

	for (;;) {
		PutSymbol(this_code, lpBuffer, cbBuffer);
		next_code = lpmvs->pMapData[map_index++];

		if (map_index > 256)
			break;

		dict[dict_index++] = this_code;
		dict[dict_index++] = next_code;

		for (int i = 0; i < dict_index; i+=2) {
			if ((dict[i] == next_code) && (dict[i + 1] == lpmvs->pMapData[map_index])) {
				next_code = (i / 2) + 256;
				map_index++;
			}
		}

		this_code = next_code;
	}
	PutSymbol(0x03FF, lpBuffer, cbBuffer);
	if (comp_iter != 6) {
		comp_index++;
	}
	return comp_index;
}
