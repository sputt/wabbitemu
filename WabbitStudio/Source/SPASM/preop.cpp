#include "stdafx.h"

#include "preop.h"
#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "pass_one.h"
#include "parser.h"
#include "expand_buf.h"
#include "errors.h"

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
#define _L(Z) ((LONG) _DW(Z))
#define _DW(Z) ((((Z)&0xFF)<<24) + (((Z)&0xFF00)<<8) + (((Z)&0xFF0000)>>8) + (((Z)&0xFF000000)>>24))
#define _W(Z) ((((Z)&0xFF)<<8) + (((Z)&0xFF00)>>8))
#else
#define _L(Z) (Z)
#define _DW(Z) (Z)
#define _W(Z) (Z)
#endif

#define MASK(Z) ((1<<(Z))-1)
#define LUMINANCE(R,G,B) (((0.2126/255.0) * (R)) + ((0.7152/255.0) * (G)) + ((0.0722/255.0) * (B)))

typedef enum {
	OP_EQUAL = 0,
	OP_LESS,
	OP_GREATER,
	OP_NOT_EQUAL,
	OP_LESS_OR_EQUAL,
	OP_GREATER_OR_EQUAL,
	OP_LAST,
} compoperator;

static const char operators[OP_LAST][4] = {"=", "<", ">", "!=", ">=", "<="};

char *do_if (char *ptr, bool condition);
char *do_elif (char *ptr, bool condition);
char *handle_preop_define (const char *ptr);
static char *handle_preop_include (char *ptr);
char *handle_preop_import (char *ptr);
char *handle_preop_if (char *ptr);
char *handle_preop_elif (char *ptr);
char *skip_until (char *ptr, int argc, ...);


extern bool case_sensitive;
static define_t *last_define = NULL;

/*
 * Handles a preop, returns the new
 * location in the file
 */

char *handle_preop (char *ptr) {
	const char *preops[] = {"define", "include", "if", "ifdef", "ifndef", "else", "elif", "endif",
		"undef", "undefine", "comment", "endcomment", "macro", "endmacro", "import", "defcont", "region", "endregion", NULL};
	char *name_end, *name;
	int preop;

	//first get the name
	name_end = ptr;
	while (isalpha (*name_end))
		name_end++;

	name = strndup (ptr, name_end - ptr);

	//then try to match it against the list of preops
	preop = 0;
	while (preops[preop]) {
		if (!strcasecmp (preops[preop], name))
			break;
		preop++;
	}

	ptr = skip_whitespace (name_end);

	// Error if it doesn't match any
	if (!preops[preop])
	{
		SetLastSPASMError(SPASM_ERR_UNKNOWN_PREOP, name);
		free(name);
		return ptr;
	}

	free (name);

	//otherwise, decide what to do depending on what the preop is
	switch (preop) {
		case 0:	//DEFINE
		{
			ptr = handle_preop_define (ptr);
			break;
		}
		case 1: //INCLUDE
		{
			ptr = handle_preop_include (ptr);
			break;
		}
		case 2: //IF
		{
			ptr = handle_preop_if (ptr);
			break;
		}
		case 3: //IFDEF
		{
			char *name_end, *name;
			define_t *define;
			bool condition;

			//get the name of the define to test
			if (is_end_of_code_line (ptr)) {
				show_fatal_error ("#IFDEF is missing condition");
				return ptr;
			}

			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);
			//if it's defined, do all the normal #if stuff
			define = search_defines (name);
			condition = (define != NULL) && (define->contents!= NULL);
			ptr = do_if (name_end, condition);
			free (name);
			break;
		}
		case 4: //IFNDEF
		{
			char *name_end, *name;
			define_t *define;
			bool condition;

			//get the name of the define to test
			if (is_end_of_code_line (ptr)) {
				show_fatal_error ("#IFNDEF is missing condition");
				return ptr;
			}

			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);

			//same as #ifdef, but reversed
			define = search_defines (name);
			condition = (define == NULL) || (define && (define->contents == NULL));
			ptr = do_if (name_end, condition);
			free (name);
			break;
		}
		case 5: //ELSE
		case 6: //ELSE IF
		{
			ptr = skip_until (ptr, 1, "#endif");
			break;
		}
		case 7: //ENDIF
		{
			//show_warning("Stray #ENDIF");
			break;
		}
		case 8: //UNDEF
		case 9: //UNDEFINE
		{
			//get the name of the define to remove
			if (is_end_of_code_line (ptr)) {
				show_error ("#UNDEF is missing name");
				return ptr;
			}

			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);
			remove_define (name);
			free (name);
			ptr = name_end;
			break;
		}
		case 10: //COMMENT
		{
			ptr = skip_until (ptr, 1, "#endcomment");
			break;
		}
		case 12: //MACRO
		{
			char *name_end, *macro_end;
			define_t *macro;

			if (is_end_of_code_line (ptr)) {
				show_fatal_error ("Macro is missing name");
				return ptr;
			}

			//get the name
			name_end = skip_to_name_end (ptr);
			char *name = strndup(ptr, name_end - ptr);
			
			macro = add_define (name, NULL);

			name_end = skip_whitespace (name_end);
			if (*name_end == '(') {
				//it has arguments, so parse them
				ptr = parse_arg_defs (++name_end, macro);
				if (ptr == NULL)
					return NULL;
			} else
				ptr = name_end;

			ptr = skip_to_next_line(ptr);
			line_num++;

			macro->line_num++;
			
			//now find the end of the macro (at the end of the file or an #endmacro directive)
			//ptr = skip_to_next_line (ptr);
			macro_end = skip_until (ptr, 1, "#endmacro");

			//...and copy everything up to the end into the contents
			set_define (macro, ptr, macro_end - ptr - strlen("#endmacro"), false);
			ptr = macro_end;
			break;
		}
		case 14: //IMPORT
		{
			ptr = handle_preop_import (ptr);
			break;
		}
		case 15: //DEFCONT
		{
			if (last_define == NULL) {
				show_error("DEFCONT used without previous define");
				break;
			}
			char *defcont_start = ptr;
			ptr = skip_to_next_line(ptr);
			int defcont_len = strlen(last_define->name) + (ptr - defcont_start) + 1;
			char *new_contents = (char *) malloc(defcont_len + 1);
			strcpy(new_contents, last_define->name);
			strcat(new_contents, " ");
			strncat(new_contents, defcont_start, ptr - defcont_start);
			new_contents[defcont_len] = '\0';
			set_define(last_define, new_contents, -1, true);
			ptr--;
		}
	}

	return ptr;
}


/*
 * Handles #DEFINE statement,
 * returns pointer to new location
 * in file
 */

char *handle_preop_define (const char *ptr) {
	char *name_end, *value_end;
	define_t *define;
	bool redefined;

	if (is_end_of_code_line (ptr)) {
		show_error ("#DEFINE is missing name");
		return (char *) ptr;
	}

	//get the name
	name_end = skip_to_name_end (ptr);
	define = add_define (strndup (ptr, name_end - ptr), &redefined);
	if (define == NULL)
		return skip_to_code_line_end (ptr);

	last_define = define;
	
	if (*name_end == '(') {
		//it's a simple macro, so get all the arguments
		ptr = parse_arg_defs (++name_end, define);
		if (ptr == NULL)
			return (char *) ptr;
		
		//then find the function it's defining
		if (is_end_of_code_line (ptr)) {
			show_error ("#DEFINE macro is missing function");
			return (char *) ptr;
		}

		value_end = skip_to_code_line_end (ptr);
		set_define (define, ptr, value_end - ptr, redefined);
		ptr = value_end;
	} else {
		//it's a normal define without arguments
		
		char word[256];
		const char *eval_ptr = ptr = name_end;

		if (is_end_of_code_line (skip_whitespace (ptr))) {
			//if there's no value specified, then set it to 1
			set_define (define, "1", 1, false);
			return (char *) ptr;
		}

		//check for certain special functions
		read_expr (&eval_ptr, word, "(");
		//handle EVAL, evaluate the contents
		if (!strcasecmp (word, "eval")) {
			char expr[256], *new_value;
			if (*eval_ptr == '(')
				eval_ptr++;
			read_expr (&eval_ptr, expr, ")");
			new_value = eval (expr);
			set_define (define, new_value, -1, redefined);
			free (new_value);
			ptr = eval_ptr;

		//handle CONCAT, concatenate contents
		} else if (!strcasecmp (word, "concat")) {
			
			expand_buf *buffer;
			int value;

			buffer = eb_init(-1);
			if (*eval_ptr == '(')
				eval_ptr++;

			char *arg = NULL;
			arg_context_t context = ARG_CONTEXT_INITIALIZER;
			while ((arg = extract_arg_string(&eval_ptr, &context)) != NULL)
			{
				if (arg[0] == '\"')
				{
					reduce_string (arg);
					eb_insert (buffer, -1, arg, -1);
				}
				else
				{
					define_t *define;

					if ((define = search_defines (arg)) != NULL)
					{
						if (define->contents == NULL)
						{
							show_error("'%s' is not yet fully defined", arg);
						
						}
						else
						{
							char *define_buffer = strdup(define->contents);
							reduce_string(define_buffer);
							eb_insert (buffer, -1, define_buffer, -1);
							free(define_buffer);
						}
					}
					else if (parse_num(arg, &value) == true)
					{
						char num_buf[256];
						sprintf (num_buf, "%d", value);
						eb_insert (buffer, -1, num_buf, -1);
					}
				}
			}
			set_define (define, eb_extract (buffer), -1, redefined);
			eb_free(buffer);
			ptr = eval_ptr;
			
		} else {
			value_end = skip_to_code_line_end (ptr);
			set_define (define, ptr, value_end - ptr, redefined);
			ptr = value_end;
		}
	}

	return (char *) ptr;
}



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
					show_warning("Bitmap width overflows 'w'; use 'ww' instead");
				write_out(r->right - r->left);
			}
			else if (!strcasecmp(p, "ww")) {
				write_out((r->right - r->left) & 0xFF);
				program_counter++;
				write_out((r->right - r->left) >> 8);
			}
			else if (!strcasecmp(p, "h")) {
				if ((r->bottom - r->top) > 255)
					show_warning("Bitmap height overflows 'h'; use 'hh' instead");
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
						show_warning("Bitmap size overflows 's'; use 'ss' instead");
					write_out(size);
				} else {
					write_out(size & 0xFF);
					program_counter++;
					write_out((size >> 8) & 0xFF);
				}
				
			} else {
				show_warning("Unknown BM_HEADER token '%s'", p);
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

static void handle_bitmap (FILE *file, const RECT *r, const BITMAPFILEHEADER *bf, const BITMAPINFOHEADER *bi, const LPRGBQUAD bmiColors) {
	//printf("handle_bitmap on: %d %d %d %d\n", r->left, r->top, r->right, r->bottom);
	
	// Bytes, padded to the nearest 32-bit
	const LONG biScanWidth = ((_L(bi->biWidth) * _W(bi->biBitCount)) + 31) / 32 * 4;
	const DWORD biImageSize = (_DW(bf->bfSize) - _DW(bf->bfOffBits));
//	if (biImageSize % biScanWidth != 0) {
//		printf("Scan width calculation incorrect! (image size: %ld, scan: %ld)\n", biImageSize, biScanWidth);
//		return;
//	}
	fseek(file, bf->bfOffBits + (biScanWidth * r->top), SEEK_SET);
	int min_w = parse_f("__BM_MIN_W");
	
	// Read in the image
#define BITS_ACCESS(zr, zc) pBits[((zr) * biScanWidth) + (zc)]
	//BYTE (*pBits)[biScanWidth] = malloc_chk(biScanWidth * (r->bottom - r->top));
	BYTE *pBits = (BYTE *) malloc_chk(biScanWidth * (r->bottom - r->top));
	fread ((BYTE *) pBits, biScanWidth * (r->bottom - r->top), 1, file);

	// Create the mask buffer
	const DWORD biOutputRowSize = ((max(r->right - r->left, min_w) + 7) / 8) * 8;
	const DWORD biByteSize = (r->bottom - r->top) * biOutputRowSize;
	
#define OUTPUT_ACCESS(zr, zc) pOutput[((zr) * biOutputRowSize) + (zc)]
	//BYTE (*pOutput)[biOutputRowSize] = malloc_chk(biByteSize);
	BYTE *pOutput = (BYTE *) malloc_chk(biByteSize);
	memset (pOutput, 0, biByteSize);
	
#define MASK_ACCESS(zr, zc) pMask[((zr) * biOutputRowSize) + (zc)]
	//BYTE (*pMask)[biOutputRowSize] = malloc_chk(biByteSize);
	BYTE *pMask = (BYTE *) malloc_chk(biByteSize);
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
			
			if (bit >= _L(bi->biWidth) * _W(bi->biBitCount) || bit >= r->right * _W(bi->biBitCount)) {
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
					int col, db_out = 0;
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
					int col;
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

/*
 * Given a filename (which may be surrounded in quotes), return
 * an allocated full path of that filename
 * return NULL if there's no matching path
 */

char *full_path (const char *filename) {
	list_t *dir;
	char *full_path;
#ifdef WIN32
	if (is_abs_path(filename) && (GetFileAttributes(filename) != 0xFFFFFFFF))
#else
	if (is_abs_path(filename) && (access (filename, R_OK) == 0))
#endif
		return strdup (filename);
	
	dir = include_dirs;
	full_path = NULL;
	do if (dir) {
		expand_buf_t *eb = eb_init (-1);
		
		eb_append (eb, (char *) dir->data, -1);
		eb_append (eb, "/", 1);
		eb_append (eb, filename, -1);
		free (full_path);
		full_path = eb_extract (eb);
		fix_filename (full_path);
		eb_free (eb);
		dir = dir->next;
#ifdef WIN32
	} while (GetFileAttributes(full_path) == 0xFFFFFFFF && dir);
#else
	} while (access (full_path, R_OK) && dir);
#endif

#ifdef WIN32
	if (GetFileAttributes(full_path) != 0xFFFFFFFF)
#else
	if (access (full_path, R_OK) == 0)
#endif
		return full_path;
	
	free (full_path);
	return NULL;
}

/*
 * Handles #INCLUDE statement, returns pointer to new location
 * in file
 */

static char *handle_preop_include (char *ptr)
{
	char name[MAX_PATH], *file_path;
	FILE *file;
	char *qs, *alloc_path, *input_contents, *old_input_file, *old_line_start;
	char *base_name = NULL, *suffix = NULL;
	int old_line_num, old_in_macro, old_old_line_num;

	if (is_end_of_code_line (ptr)) {
		show_error ("#INCLUDE is missing file name");
		return ptr;
	}

	//get the name of the file to include	
	read_expr (&ptr, name, "");
	fix_filename (name);
	
	qs = skip_whitespace (name);
	if (*qs == '"') {
		int i;
		qs++;
		for (i = 0; qs[i] != '"' && qs[i] != '\0'; i++);
		qs[i] = '\0';
	}

	//now see where it is, using include directories
	file_path = full_path (qs);

	//finally, now that we've got the full path, determine file type
	if (!file_path || !(file = fopen (file_path, "rb")))
	{
		SetLastSPASMError(SPASM_ERR_FILE_NOT_FOUND, name);
		show_error ("%s: No such file or directory", name);
		if (file_path) free (file_path);
		return ptr;
	}
	
	BITMAPFILEHEADER bf;
	fread (&bf, sizeof (BITMAPFILEHEADER), 1, file);

	if (!(LOBYTE(_W(bf.bfType)) == 'B' && HIBYTE(_W(bf.bfType)) == 'M')) {
		fclose (file);
		
		input_contents = get_file_contents (file_path);
		if (!input_contents) {
			show_error ("Couldn't open #included file %s", file_path);
			free (file_path);
			return ptr;
		}

		//add it to the list of input files
		alloc_path = strdup (file_path);
		input_files = list_prepend (input_files, alloc_path);
		free (file_path);

		//make sure the listing for this line is finished up BEFORE
		// the new file is parsed and writes its listing stuff all over
		if (mode & MODE_LIST && listing_on && !listing_for_line_done)
			do_listing_for_line (skip_to_next_line (line_start));

		if (mode & MODE_LIST && listing_on) {
			char include_banner[MAX_PATH + 64];
			snprintf(include_banner, sizeof (include_banner), "Listing for file \"%s\"" NEWLINE, fix_filename (alloc_path));
			listing_offset = eb_insert (listing_buf, listing_offset, include_banner, strlen (include_banner));
		}
		
		//swap out the old curr_X values, and swap in the new ones
		old_input_file = curr_input_file;
		old_line_num = line_num;
		old_in_macro = in_macro;
		old_line_start = line_start;
		old_old_line_num = old_line_num;
		curr_input_file = alloc_path;

		//now parse the file
		run_first_pass (input_contents);

		//when done, swap the old curr_X values back in
		curr_input_file = old_input_file;
		line_num = old_line_num;
		in_macro = old_in_macro;
		line_start = old_line_start;
		old_line_num = old_old_line_num;

		if (mode & MODE_LIST && listing_on && !listing_for_line_done)
			listing_for_line_done = true;

		if (mode & MODE_LIST && listing_on) {
			char include_banner[MAX_PATH + 64];
			snprintf(include_banner, sizeof (include_banner), "Listing for file \"%s\"" NEWLINE, curr_input_file);
			listing_offset = eb_insert (listing_buf, listing_offset, include_banner, strlen (include_banner));
		}
		
		//and free up stuff
		//free (input_contents);
		release_file_contents(input_contents);

		return ptr;
	}
	else
	{
		BITMAPINFOHEADER bi;
		RGBQUAD *bmiColors;
		DWORD nColors = 0;
		RECT r;
		LONG padding = parse_f("__BM_PAD");
		define_t *img_map = search_defines("__BM_MAP");
		int min_w = parse_f("__BM_MIN_W");
		int bmp_num = 1;

		free (file_path);
		fread (&bi, sizeof (BITMAPINFOHEADER), 1, file);

		if (_DW(bi.biSize) != sizeof (BITMAPINFOHEADER) || _DW(bi.biCompression) != BI_RGB) {
			show_error ("Bitmap file of this type not supported: size: %ld, compression: %ld\n", _DW(bi.biSize), _DW(bi.biCompression));
			return ptr;
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
			return ptr;
		}

		if (last_label != NULL && last_label->value == program_counter) {
			base_name = (char *) malloc(strlen(last_label->name) + 32);
			strcpy(base_name, last_label->name);
			suffix = base_name + strlen(base_name);
		}
	
		if (img_map == NULL || parse_f("__BM_MAP") == 0) {
			int width = _L(bi.biWidth);
			int height = _L(bi.biHeight);
			
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
					handle_bitmap(file, &r, &bf, &bi, bmiColors);
					r.left += min_w;
					w -= min_w;
				}
			} else {
				handle_bitmap(file, &r, &bf, &bi, bmiColors);
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
				show_error("At least either of width or height must be given for an image map");
				goto map_done;
			}

			width 	= (_L(bi.biWidth) - (width * 2 * padding)) / width;
			height 	= (_L(bi.biHeight) - (height * 2 * padding)) / height;

			if (base_name) {
				strcpy(suffix, "_WIDTH");
				add_label(strdup(base_name), width);
			
				strcpy(suffix, "_HEIGHT");
				add_label(strdup(base_name), height);
			}
		
			for (left = 0; left < _L(bi.biWidth); left += width + 2*padding) {
				for (top = _L(bi.biHeight) - height - 2*padding; top >= 0 ; top -= height + 2*padding) {
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
							handle_bitmap(file, &r, &bf, &bi, bmiColors);
							r.left += min_w;
							w -= min_w;
						}
					} else {
						handle_bitmap(file, &r, &bf, &bi, bmiColors);
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
		fclose (file);
	
		return ptr;
	}
}


/*
 * Handles #IMPORT statement,
 * returns pointer to new location
 * in file
 */

char *handle_preop_import (char *ptr) {
	FILE *import_file;
	int c;
	char name[256];

	if (is_end_of_code_line (ptr)) {
		show_error ("#IMPORT is missing file name");
		return ptr;
	}

	//get the name of the file to include	
	read_expr (&ptr, name, "");
	fix_filename (name);
	
	char *qs = skip_whitespace(name);
	if (*qs == '"') {
		int i;
		qs++;
		for (i = 0; qs[i] != '"' && qs[i] != '\0'; i++);
		qs[i] = '\0';
	}

	//now see where it is, using include directories
	char *file_path = full_path(qs);

	//...and read it in
	import_file = fopen(file_path, "rb");
	if (!import_file)
	{
		SetLastSPASMError(SPASM_ERR_FILE_NOT_FOUND, file_path);
		free(file_path);
		return ptr;
	}

	free(file_path);

	//write the contents to the output
	while ((c = fgetc(import_file)) != EOF)
	{
		write_out (c);
		program_counter++;
	}

	fclose (import_file);
	return ptr;
}


/*
 * Handles #IF statement,
 * returns pointer to new
 * location in file
 */

char *handle_preop_if (char *ptr) {
	char *expr_end, *expr;
	int condition;

	if (is_end_of_code_line (ptr)) {
		show_fatal_error ("#IF is missing condition");
		return ptr;
	}

	expr_end = next_code_line (ptr) - 1;
	while (is_end_of_code_line (expr_end))
		expr_end--;
//	expr_end = skip_to_code_line_end (ptr);
	
	expr = strndup (ptr, expr_end - ptr + 1);
	
	parse_num (expr, &condition);
	free(expr);
	
	return do_if (expr_end + 1, condition);
}

/*
 * Handles #IF statement,
 * returns pointer to new
 * location in file
 */

char *handle_preop_elif (char *ptr) {
	char *expr_end, *expr;
	int condition;

	if (is_end_of_code_line (ptr)) {
		show_fatal_error ("#ELIF is missing condition");
		return ptr;
	}

	expr_end = next_code_line (ptr) - 1;
	while (is_end_of_code_line (expr_end))
		expr_end--;
//	expr_end = skip_to_code_line_end (ptr);
	
	expr = strndup (ptr, expr_end - ptr + 1);
	
	parse_num (expr, &condition);
	free(expr);
	
	return do_elif(expr_end + 1, condition);
}




/*
 * Skips the appropriate
 * parts of #IF blocks,
 * returns pointer to location
 * in file, needs to be passed
 * whether the condition is
 * true or false
 */

char *do_if (char *ptr, bool condition) {
	char *temp;
	if (condition) return ptr;
	
	return skip_until (ptr, 3, "#else", "#elif", "#endif");
}

/*
 * Skips the appropriate
 * parts of #IF blocks,
 * returns pointer to location
 * in file, needs to be passed
 * whether the condition is
 * true or false
 */

char *do_elif (char *ptr, bool condition) {
	char *temp;
	if (condition) {
		while(!line_has_word(ptr, _T("#elif"), 5) &&
				!line_has_word(ptr, _T("#else"), 5) &&
				!line_has_word(ptr, _T("#endif"), 6))
		{
			ptr = run_first_pass_line(ptr);
			ptr = skip_to_next_line(ptr);
		}
		return skip_until(ptr, 1, _T("#endif"));
	}
	
	return skip_until (ptr, 3, _T("#else"), _T("#elif"), _T("#endif"));
}


/*
 * Parses argument definitions
 * for a macro, returns a pointer
 * to the end of the args
 */

char *parse_arg_defs (const char *ptr, define_t *define) {
	char *word;

	define->num_args = 0;
	arg_context_t context = ARG_CONTEXT_INITIALIZER;
	context.fExpectingMore = false;
	while ((word = extract_arg_string(&ptr, &context)) != NULL)
	{
		bool is_dup = false;
		int i;
		
		for (i = 0; i < define->num_args && !is_dup; i++) {
			if (case_sensitive) {
				if (strcmp(word, define->args[i]) == 0)
					is_dup = true;
			} else if (strcasecmp(word, define->args[i]) == 0)
				is_dup = true;
		}
		if (is_dup) {
			show_fatal_error("Duplicate argument name '%s'", strdup (word));
			return NULL;
		}
		define->args[define->num_args++] = strdup (word);
	}
	
	if (*ptr == ')') ptr++;
	return (char *) ptr;
}


char *skip_until (char *ptr, int argc, ...)
{
	int level = 0;
	va_list argp;

	if (argc == 0) return ptr;
	
	while (*ptr && !error_occurred) {
		char *line = ptr;
		char *line_end = skip_to_next_line(ptr);
		
		do {
			bool word_found = false;
			
			// Test for nesting
			if (line_has_word (line, "#IF", 3)) {
				level++;
			} else if (level > 0 && line_has_word (line, "#ENDIF", 6)) {
				level--;
			} else if (level == 0){
				int i;
				// Test all of the words that mark the end of the skipping
				va_start(argp, argc);
				i = argc;
				while (i-- && !word_found) {
					char *word = va_arg(argp, char *);
					if (line_has_word (line, word, strlen (word))) {
						word_found = true;
						
						//a little hacky but bleh...
						if (!strcmp(word, _T("#elif"))) {
							return handle_preop_elif(ptr + 5);
							
						}
						return next_expr(line, "\\\r\n");
					}
				}
				va_end(argp);
			}
			
			line = next_code_line(line);
		} while (line < line_end && !error_occurred);
			
		ptr = line_end;
		line_num++;
	}
	
	return ptr;
}
