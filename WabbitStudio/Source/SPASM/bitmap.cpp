#include "stdafx.h"

#include "spasm.h"
#include "errors.h"
#include "utils.h"
#include "storage.h"
#include "pass_one.h"
#include "parser.h"

#ifndef WIN32
typedef u_int32_t LONG;
typedef u_int16_t UINT;
typedef u_int8_t BYTE;
typedef u_int16_t WORD;
//typedef u_int32_t DWORD;
#define LOBYTE(w)	((BYTE)(w))
#define HIBYTE(w)	((BYTE)(((WORD)(w)>>8)&0xFF))
#define BI_RGB 0

typedef struct tagRECT {
	LONG top, left, right, bottom;
} RECT;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} __attribute__((packed)) RGBQUAD,*LPRGBQUAD;


typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER,*LPBITMAPFILEHEADER,*PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER,*LPBITMAPINFOHEADER,*PBITMAPINFOHEADER;
#endif

#ifdef __BIG_ENDIAN__
#define _ZL(Z) ((LONG) _DW(Z))
#define _DW(Z) ((((Z)&0xFF)<<24) + (((Z)&0xFF00)<<8) + (((Z)&0xFF0000)>>8) + (((Z)&0xFF000000)>>24))
#define _W(Z) ((((Z)&0xFF)<<8) + (((Z)&0xFF00)>>8))
#else
#define _ZL(Z) (Z)
#define _DW(Z) (Z)
#define _W(Z) (Z)
#endif

#define MASK(Z) ((1<<(Z))-1)
#define LUMINANCE(R,G,B) (((0.2126/255.0) * (R)) + ((0.7152/255.0) * (G)) + ((0.0722/255.0) * (B)))



static void handle_bitmap_header(const RECT *r, const BITMAPFILEHEADER *bf, const BITMAPINFOHEADER *bi) {
	// Handle an optional header
	if (define_with_value("__BM_HDR", 1)) {
		char *hdr, *p;
		define_t *def_hdr_fmt = search_defines("__BM_HDR_FMT");
		if (def_hdr_fmt == NULL)
			return;
		
		hdr = strdup(def_hdr_fmt->contents);
		reduce_string(hdr);

		for (p = strtok(hdr, ",;x "); p; p = strtok(NULL, ",;x ")) {

			if (!strcasecmp(p, "w")) {
				if (r->right - r->left > 255)
					SetLastSPASMWarning(SPASM_WARN_BITMAP_WIDTH_OVERFLOW);
				write_out(r->right - r->left);
			}
			else if (!strcasecmp(p, "ww")) {
				write_out((r->right - r->left) & 0xFF);
				program_counter++;
				write_out((r->right - r->left) >> 8);
			}
			else if (!strcasecmp(p, "h")) {
				if ((r->bottom - r->top) > 255)
					SetLastSPASMWarning(SPASM_WARN_BITMAP_HEIGHT_OVERFLOW);
				write_out((r->bottom - r->top));
			}
			else if (!strcasecmp(p, "hh")) {
				write_out((r->bottom - r->top) & 0xFF);
				program_counter++;
				write_out((r->bottom - r->top) >> 8);
			}
			else if (!strcasecmp(p, "b"))
				write_out(parse_f("__BM_SHD"));
			else if (!strcasecmp(p, "s") || !strcasecmp(p, "ss")) {
				int size = ((r->right - r->left) + 7)/8 * (r->bottom - r->top);
				if (define_with_value ("__BM_SHD", 4)) size *=2;
				else if (define_with_value ("__BM_SHD", 8)) size *=4;
				if (define_with_value ("__BM_MASK", 1)) size += ((r->right - r->left) + 7)/8 * (r->bottom - r->top);

				if (!strcasecmp(p, "s")) {
					if (size > 255)
						SetLastSPASMWarning(SPASM_WARN_BITMAP_SIZE_OVERFLOW);
					write_out(size);
				} else {
					write_out(size & 0xFF);
					program_counter++;
					write_out((size >> 8) & 0xFF);
				}
				
			} else {
				SetLastSPASMWarning(SPASM_WARN_BITMAP_UNKNOWN_TOKEN, p);
				write_out(0);
			}
			program_counter++;
		}
		
		free(hdr);
	}
}

#ifdef _WIN32
static unsigned int log2(unsigned int value)
{
	unsigned int l = 0;
	while( (value >> l) > 1 ) ++l;
	return l;
}
#endif

bool IsFileBitmap(FILE *file)
{
	fseek(file, 0, SEEK_SET);

	BITMAPFILEHEADER bf;
	fread (&bf, sizeof (BITMAPFILEHEADER), 1, file);

	if (LOBYTE(_W(bf.bfType)) == 'B' && HIBYTE(_W(bf.bfType)) == 'M')
	{
		return true;
	}
	else
	{
		return false;
	}
}

static void handle_bitmap_internal(FILE *file, const RECT *r, const BITMAPFILEHEADER *bf, const BITMAPINFOHEADER *bi, const LPRGBQUAD bmiColors)
{
	//printf("handle_bitmap on: %d %d %d %d\n", r->left, r->top, r->right, r->bottom);
	
	// Bytes, padded to the nearest 32-bit
	const LONG biScanWidth = ((_ZL(bi->biWidth) * _W(bi->biBitCount)) + 31) / 32 * 4;
	const DWORD biImageSize = (_DW(bf->bfSize) - _DW(bf->bfOffBits));
//	if (biImageSize % biScanWidth != 0) {
//		printf("Scan width calculation incorrect! (image size: %ld, scan: %ld)\n", biImageSize, biScanWidth);
//		return;
//	}
	fseek(file, bf->bfOffBits + (biScanWidth * r->top), SEEK_SET);
	int min_w = parse_f("__BM_MIN_W");
	
	// Read in the image
#define BITS_ACCESS(zr, zc) pBits[((zr) * biScanWidth) + (zc)]
	//BYTE (*pBits)[biScanWidth] = malloc(biScanWidth * (r->bottom - r->top));
	BYTE *pBits = (BYTE *) malloc(biScanWidth * (r->bottom - r->top));
	fread ((BYTE *) pBits, biScanWidth * (r->bottom - r->top), 1, file);

	// Create the mask buffer
	const DWORD biOutputRowSize = ((max(r->right - r->left, min_w) + 7) / 8) * 8;
	const DWORD biByteSize = (r->bottom - r->top) * biOutputRowSize;
	
#define OUTPUT_ACCESS(zr, zc) pOutput[((zr) * biOutputRowSize) + (zc)]
	//BYTE (*pOutput)[biOutputRowSize] = malloc(biByteSize);
	BYTE *pOutput = (BYTE *) malloc(biByteSize);
	memset (pOutput, 0, biByteSize);
	
#define MASK_ACCESS(zr, zc) pMask[((zr) * biOutputRowSize) + (zc)]
	//BYTE (*pMask)[biOutputRowSize] = malloc(biByteSize);
	BYTE *pMask = (BYTE *) malloc(biByteSize);
	memset (pMask, 1, biByteSize);
	
	RGBQUAD rgbMask = {0, 255, 0, 0};
	DWORD value = parse_f ("__BM_MSK_RGB");
	
	rgbMask.rgbRed 		= (value >> 16) & 0xFF;
	rgbMask.rgbGreen 	= (value >> 8) & 0xFF;
	rgbMask.rgbBlue 	= (value) & 0xFF;

	bool HandleInv = false;
	RGBQUAD rgbInv;
	if ((value = parse_f("__BM_INV_RGB")) != -1) {
		rgbInv.rgbRed 	= (value >> 16) & 0xFF;
		rgbInv.rgbGreen = (value >> 8) & 0xFF;
		rgbInv.rgbBlue 	= (value) & 0xFF;
		HandleInv = true;
	}
	WORD biOutputShades = parse_f("__BM_SHD");
	
	int row;
	for (row = r->bottom - r->top - 1; row >= 0; row--) {
		int col, bit;
		for (	col = 0, bit = r->left * _W(bi->biBitCount); 
				bit < (max((r->right * _W(bi->biBitCount)), ((r->left + min_w) * _W(bi->biBitCount)))); 
				col++, bit += _W(bi->biBitCount)) 
		{
			RGBQUAD rgb = {255, 255, 255, 0};
			
			if (bit >= _ZL(bi->biWidth) * _W(bi->biBitCount) || bit >= r->right * _W(bi->biBitCount)) {
				// Output the brightest shade, then continue
				//pOutput[r->bottom - r->top - 1 - row][col]
				OUTPUT_ACCESS(r->bottom - r->top - 1 - row, col) = 0;
				continue;
			} else if (_W(bi->biBitCount) < 16 || _DW(bi->biClrUsed) != 0) {
				BYTE bMask = MASK(_W(bi->biBitCount)) << ((8 - _W(bi->biBitCount)) - (bit % 8));
				
				UINT iColor = 0;
				int i = 0;
				do {
					iColor <<= 8;
					//iColor += (pBits[row][bit / 8] & bMask) >> (8 - _W(bi->biBitCount) - (bit % 8));
					iColor += (BITS_ACCESS(row, bit / 8) & bMask) >> (8 - _W(bi->biBitCount) - (bit % 8));
				} while (++i < (_W(bi->biBitCount) / 8));

				rgb = bmiColors[iColor];
			} else {
				WORD biCmBitCount = _W(bi->biBitCount) / 3;
				DWORD dwData = 0;
				int i;
				for (i = 0; i < (_W(bi->biBitCount) / 8); i++) {
					dwData <<= 8;
					//dwData += pBits[row][(bit / 8) + i];
					dwData += BITS_ACCESS(row, (bit / 8) + i);
				}

				rgb.rgbRed 		= (dwData) & MASK(biCmBitCount); 
				rgb.rgbGreen 	= (dwData >> biCmBitCount) & MASK(biCmBitCount); 
				rgb.rgbBlue 	= (dwData >> (2 * biCmBitCount)) & MASK(biCmBitCount); 
			}

			double lum = LUMINANCE(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
			
			if (HandleInv && (memcmp(&rgb, &rgbInv, sizeof(RGBQUAD)) == 0)) {
				//pOutput[r->bottom - r->top - 1 - row][col] = biOutputShades - 1;
				OUTPUT_ACCESS(r->bottom - r->top - 1 - row, col) = biOutputShades - 1;
			} else {
				// Calculate the output shade (use 0.99 instead of 1.0 to stick within the range)
				//pOutput[r->bottom - r->top - 1 - row][col] = (0.99 - lum) * biOutputShades;
				OUTPUT_ACCESS(r->bottom - r->top - 1 - row, col) = (BYTE) ((0.99 - lum) * biOutputShades);
			
				// Calculate the mask
				if (memcmp (&rgb, &rgbMask, sizeof (RGBQUAD)) != 0)
				//	pMask[r->bottom - r->top - 1 - row][col] = 0;
					MASK_ACCESS(r->bottom - r->top - 1 - row, col) = 0;
			}
		}
	}

	int image_order = 1;
	int mask_order = 2;
	
	if (define_with_value ("__BM_MSK_1ST", 1)) {
		mask_order = 1;
		image_order = 2;
	}
	
	int order;
	for (order = 1; order <= 2; order++) {
		
		if (order == image_order) {
			// Write out all image layers
			int i;
			for (i = log2(biOutputShades) - 1; i >= 0; i--) {
				int row;
				for (row = 0; row < r->bottom - r->top; row++) {
					DWORD col;
					int db_out = 0;
					for (col = 0; col < biOutputRowSize; col++) {
						//db_out |= ((pOutput[row][col] >> i) & 1) << (7 - (col % 8));
						db_out |= ((OUTPUT_ACCESS(row, col) >> i) & 1) << (7 - (col % 8));
						
						if (col % 8 == 7) {
							write_out(db_out);
							program_counter++;
							db_out = 0;
						}
					}
				}
			}
			free (pOutput);
		}
		
		if (order == mask_order) {
			// Write out the mask
			int mask_chk = 0;
			if (define_with_value ("__BM_MSK", 1)) {
				int row;
				for (row = 0; row < r->bottom - r->top; row++) {
					DWORD col;
					BYTE db_out = 0;
					for (col = 0; col < biOutputRowSize; col++) {
						//db_out |= (pMask[row][col] & 1) << (7 - (col % 8));
						db_out |= (MASK_ACCESS(row, col) & 1) << (7 - (col % 8));
						
						if (col % 8 == 7) {
							if (define_with_value ("__BM_MSK_INV", 1))
								write_out(~db_out);
							else
								write_out(db_out);
							program_counter++;
							mask_chk |= db_out;
							db_out = 0;
						}
					}
				}
		
				//if (mask_chk == 0)
					//show_warning("Bitmap mask is blank - %d null bytes written", (int) biByteSize/8);
			}
		}
	}

	free (pBits);
	free (pMask);
}

void handle_bitmap(FILE *file)
{
	char *base_name = NULL, *suffix = NULL;
	BITMAPINFOHEADER bi;
	RGBQUAD *bmiColors;
	DWORD nColors = 0;
	RECT r;
	LONG padding = parse_f("__BM_PAD");
	define_t *img_map = search_defines("__BM_MAP");
	int min_w = parse_f("__BM_MIN_W");
	int bmp_num = 1;

	fseek(file, 0, SEEK_SET);

	BITMAPFILEHEADER bf;
	fread (&bf, sizeof (BITMAPFILEHEADER), 1, file);

	fread (&bi, sizeof (BITMAPINFOHEADER), 1, file);

	if (_DW(bi.biSize) != sizeof (BITMAPINFOHEADER) || _DW(bi.biCompression) != BI_RGB) {
		SetLastSPASMError (SPASM_ERR_BITMAP_INVALID, _DW(bi.biSize), _DW(bi.biCompression));
		return;
	}

	if (_W(bi.biBitCount) < 16 || _DW(bi.biClrUsed) != 0) {
		// If biClrUsed is 0, then the amount of RGBQUADs listed is the total possible
		// as given by biBitCount
		nColors = (_DW(bi.biClrUsed == 0)) ? 1 << _W(bi.biBitCount) : _DW(bi.biClrUsed);
	}

	// Read in the palette
	bmiColors = (LPRGBQUAD) calloc(nColors, sizeof(RGBQUAD));
	fread(bmiColors, sizeof(RGBQUAD), nColors, file);

	if (ftell (file) != _DW(bf.bfOffBits)) {
		printf("invalid file structure!\n");
		return;
	}

	if (last_label != NULL && last_label->value == program_counter) {
		base_name = (char *) malloc(strlen(last_label->name) + 32);
		strcpy(base_name, last_label->name);
		suffix = base_name + strlen(base_name);
	}

	if (img_map == NULL || parse_f("__BM_MAP") == 0) {
		int width = _ZL(bi.biWidth);
		int height = _ZL(bi.biHeight);

		r.left = 0;
		r.top = 0;
		r.right = width;
		r.bottom = height;

		if (base_name) {
			strcpy(suffix, "_WIDTH");
			add_label(strdup(base_name), width);

			strcpy(suffix, "_HEIGHT");
			add_label(strdup(base_name), height);
		}

		handle_bitmap_header(&r, &bf, &bi);


		if (min_w > 0 && width > min_w) {
			int w = width;
			while (w > 0) {
				r.right = r.left + min(min_w, w);
				handle_bitmap_internal(file, &r, &bf, &bi, bmiColors);
				r.left += min_w;
				w -= min_w;
			}
		} else {
			handle_bitmap_internal(file, &r, &bf, &bi, bmiColors);
		}
	} else {
		int left, top, width = 0, height = 0, count = 0, i = 0;
		char *p;
		char *fmt = strdup(skip_whitespace(img_map->contents));
		reduce_string(fmt);

		for (p = strtok(fmt, ",;x "); p; p = strtok(NULL, ",;x ")) {
			if (i == 0) width = parse_f(p);
			else if (i == 1) height = parse_f(p);
			else if (i == 2) count = parse_f(p) + 1;
			i++;
		}
		free(fmt);

		if (padding == -1) padding = 0;

		if (width == 0 && height == 0) {
			SetLastSPASMError(SPASM_ERR_BITMAP_NO_DIM);
			goto map_done;
		}

		width 	= (_ZL(bi.biWidth) - (width * 2 * padding)) / width;
		height 	= (_ZL(bi.biHeight) - (height * 2 * padding)) / height;

		if (base_name) {
			strcpy(suffix, "_WIDTH");
			add_label(strdup(base_name), width);

			strcpy(suffix, "_HEIGHT");
			add_label(strdup(base_name), height);
		}

		for (left = 0; left < _ZL(bi.biWidth); left += width + 2*padding) {
			for (top = _ZL(bi.biHeight) - height - 2*padding; top >= 0 ; top -= height + 2*padding) {
				int min_w = parse_f("__BM_MIN_W");
				if (count == 1) goto map_done;
				if (count != 0) count--;

				if (base_name) {
					char num_buf[8];
					sprintf(num_buf, "%d", bmp_num);
					strcpy(suffix, num_buf);
					add_label(strdup(base_name), program_counter);
				}

				bmp_num++;

				r.left = left + padding;
				r.top = top + padding;
				r.right = r.left + width;
				r.bottom = r.top + height;
				handle_bitmap_header(&r, &bf, &bi);

				if (min_w > 0 && width > min_w) {
					int w = width;
					while (w > 0) {
						r.right = r.left + min(min_w, w);
						handle_bitmap_internal(file, &r, &bf, &bi, bmiColors);
						r.left += min_w;
						w -= min_w;
					}
				} else {
					handle_bitmap_internal(file, &r, &bf, &bi, bmiColors);
				}
			}
		}
map_done:
		if (base_name) {
			strcpy(suffix, "_COUNT");
			add_label(strdup(base_name), bmp_num - 1);
		}

		set_define(img_map, "0", -1, NULL);
	}
	if (base_name)
		free(base_name);
	free (bmiColors);
}