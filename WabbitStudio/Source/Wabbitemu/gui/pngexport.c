#include "stdafx.h"

#include "pngexport.h"
#include "gif.h"

extern BITMAPINFO *bi, *colorbi;

int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo *pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0) {
		return -1;
	}

	pImageCodecInfo = (ImageCodecInfo *)malloc(size);
	if (pImageCodecInfo == NULL) {
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

void export_png(LPCALC lpCalc, TCHAR *filename) {
	RECT rc;
	GetClientRect(lpCalc->hwndLCD, &rc);
	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	PixelFormat format;
	if (lcd->bytes_per_pixel == 3) {
		format = PixelFormat24bppRGB;
	} else {
		format = PixelFormat8bppIndexed;
	}

	BITMAPINFO *info = lpCalc->model >= TI_84PCSE ? colorbi : bi;
	uint8_t *image_ptr = lcd->image(lcd);
	Bitmap *image = new Bitmap(info, image_ptr);
	
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	WCHAR still_name[MAX_PATH];
#ifdef UNICODE
	StringCbCopy(stillName, sizeof(stillName), gif_file_name);
#else
	size_t converted;
	mbstowcs_s(&converted, still_name, filename, MAX_PATH);
#endif
	image->Save(still_name, &pngClsid, NULL);

	if (lcd->display_width != lcd->width) {
		delete image;
		image = new Bitmap(still_name);
		Bitmap *newBitmap = new Gdiplus::Bitmap(lcd->display_width, lcd->height);
		Graphics graphics(newBitmap);
		graphics.DrawImage(image, 0, 0, lcd->width, lcd->height);
		// delete so we close the output file
		delete image;

		newBitmap->Save(still_name, &pngClsid, NULL);
		delete newBitmap;
	} else {
		delete image;
	}

	free(image_ptr);
}