#include "stdafx.h"

#include "print.h"
#include "label.h"
#include "core.h"


#define press_text(sztext, zcolor) press_textA(sztext, zcolor, &r, hdc)

static BOOL calc_size = FALSE;
static size_t mspf_size = 0;
static u_int mspf_break = 9999;

void press_textA(TCHAR *szText, COLORREF zcolor, RECT *r, HDC hdc) {
	RECT tr;

	tr.left = 0; tr.right = 1;
	SetTextColor(hdc, zcolor);
	DrawText(hdc, szText, -1, &tr, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
	r->right = r->left + tr.right;
	
	size_t index = mspf_size;
	mspf_size += (int) _tcslen(szText);
	if (calc_size == FALSE) {
		const TCHAR *dot_strings[] = {_T("."), _T(".."), _T("...")};
		TCHAR szNew[1024];
		
		if (index >= mspf_break || (index < mspf_break && index+_tcslen(szText) > mspf_break)) {
			int break_index = (int) (max(index, mspf_break));
			int break_string_index = break_index - (int) index;
			int str_left = (int) _tclen(&szText[break_string_index]);
			
			if (str_left > 3)
				str_left = 3;

			if (index > mspf_break)
				str_left -= (int) (index - mspf_break);
			
			if (str_left < 1)
				str_left = 1;
			
#ifdef WINVER
			StringCbCopy(szNew, sizeof(szNew), szText);
			StringCbCopy(&szNew[break_string_index], _tcslen(dot_strings[str_left-1]) + 1, dot_strings[str_left-1]);
#else
			strcpy(szNew, szText);
			strcpy(&szNew[break_string_index], dot_strings[str_left-1]);
#endif
			
			szText = szNew;
		}
		
		DrawText(hdc, szText, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	OffsetRect(r, tr.right, 0);
}


void MyDrawText(LPCALC lpCalc, HDC hdc, RECT *rc, Z80_info_t* zinf, ViewType type, const TCHAR *fmt, ...) {
	TCHAR *p;
	va_list argp;
	RECT r = *rc;
	
	mspf_size = 0;
	mspf_break = 999;
	
	if (calc_size == FALSE) {
		calc_size = TRUE;
		
		MyDrawText(lpCalc, hdc, rc, zinf, REGULAR, fmt, zinf->a1, zinf->a2, zinf->a3, zinf->a4);
		
		TCHAR szFilltext[1024];
		memset(szFilltext, 'A', mspf_size);
		szFilltext[mspf_size] = '\0';

		RECT hr;
		CopyRect(&hr, rc);
		DrawText(hdc, szFilltext, -1, &hr, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_MODIFYSTRING);

		mspf_break = (int) _tcslen(szFilltext);

		if (mspf_break < mspf_size) {
			mspf_break -= 3;
		} else {
			mspf_break++;
		}
		calc_size = FALSE;
	}
	
	mspf_size = 0;
	
	// Initialize arguments
	va_start(argp, fmt);
	for (p = (TCHAR *) fmt; *p && (mspf_size < mspf_break+3); p++) {
		if(*p != '%') {
			TCHAR szChar[2] = _T("x");
			szChar[0] = *p;
			press_text(szChar, DBCOLOR_BASE);
		} else {
			switch(*++p) {
				case 'c': {		//condition
					TCHAR *s = va_arg(argp, TCHAR *);
					press_text(s, DBCOLOR_CONDITION);
					break;
				}
				case 'h': {		//offset
					char val	= (char) va_arg(argp, INT_PTR);
					TCHAR szOffset[8];
					if (val & 0x80) {
						_stprintf_s(szOffset, _T("%d"), val);
					} else {
						_stprintf_s(szOffset, _T("+%d"), val);
					}

					press_text(szOffset, RGB(0, 0, 0));
					break;
				}
				case 'd': {		//number
					int val	= (int) va_arg(argp, INT_PTR);
					TCHAR szAddr[16];
					_stprintf_s(szAddr, _T("%d"), val);

					press_text(szAddr, RGB(0, 0, 0));		
					break;
				}
				case 'l':
				{
					TCHAR *s = va_arg(argp, TCHAR *);
					press_text(s, RGB(0, 0, 0));
					break;
				}		
				case 's':
				{
					TCHAR *s = va_arg(argp, TCHAR *);
					press_text(s, DBCOLOR_BASE);
					break;
				}
				case 'g':
				{
					waddr_t waddr = OffsetWaddr(lpCalc->cpu.mem_c, REGULAR, zinf->waddr, 2 + ((char) va_arg(argp, INT_PTR)));
					TCHAR *name;
					
					name = FindAddressLabel(lpCalc, waddr);
					
					if (name) {
						press_text(name, RGB(0, 0, 0));
					} else {
						TCHAR szAddr[16];
						_stprintf_s(szAddr, _T("$%04X"), waddr.addr);
						press_text(szAddr, RGB(0, 0, 0));
					}
					break;
				}
				case 'a': //address
					{
						waddr_t waddr = OffsetWaddr(lpCalc->cpu.mem_c, REGULAR, zinf->waddr, 2);
						TCHAR *name;
						int val = (int) va_arg(argp, INT_PTR);

						name = FindAddressLabel(lpCalc, addr_to_waddr(lpCalc->cpu.mem_c, val));
						
						if (name) {
							press_text(name, RGB(0, 0, 0));
						} else {
							TCHAR szAddr[16];
							_stprintf_s(szAddr, _T("$%04X"), val);
							press_text(szAddr, RGB(0, 0, 0));
						}
						break;
					}
				case 'r':
				{
					TCHAR *szReg = va_arg(argp, TCHAR *);
					if (!_tcscmp(szReg, _T("(hl)"))) {
						press_text(_T("("), DBCOLOR_BASE);
						press_text(_T("hl"), DBCOLOR_HILIGHT);
						press_text(_T(")"), DBCOLOR_BASE);
					} else
						press_text(szReg, DBCOLOR_HILIGHT);
					break;
				}
				case 'x':
				{
					int val	= (int) va_arg(argp, INT_PTR);
					TCHAR szAddr[16];
					StringCbPrintf(szAddr, sizeof(szAddr), _T("$%02X"), val);
					press_text(szAddr, RGB(0, 0, 0));	
					break;	
				}
			}
		}
	}
	va_end(argp);
}

void mysprintf(LPCALC lpCalc, TCHAR *output, int outputLength, Z80_info_t* zinf, ViewType type, const TCHAR *fmt, ...) {
	TCHAR *p;
	va_list argp;

	if (output == NULL) {
		return;
	}
	*output = '\0';
	
	mspf_size = 0;
	mspf_break = 999;
	
	mspf_size = 0;
	
	// Initialize arguments
	va_start(argp, fmt);
	for (p = (TCHAR *) fmt; *p && (mspf_size < mspf_break+3); p++) {
		if(*p != '%') {
			TCHAR szChar[2] = _T("x");
			szChar[0] = *p;
			StringCbCat(output, outputLength, szChar);
		} else {
			switch(*++p) {
				case 'c': {//condition
					TCHAR *s = va_arg(argp, TCHAR *);
					StringCbCat(output, outputLength, s);
					break;
				}
				case 'h': {//offset
					int val	= (int) va_arg(argp, INT_PTR);
					TCHAR szOffset[8];
#ifdef WINVER
					_stprintf_s(szOffset, _T("%+d"),val);
#else
					sprintf(szOffset, "%+d",val);
#endif
					StringCbCat(output, outputLength, szOffset);
					break;
				}
				case 'd': //number
				{
					int val	= (int) va_arg(argp, INT_PTR);
					TCHAR szAddr[16];
#ifdef WINVER
					_stprintf_s(szAddr, _T("%d"), val);
#else
					sprintf(szAddr, "%d",val);
#endif
					StringCbCat(output, outputLength, szAddr);		
					break;
				}
				case 'l':
				{
					TCHAR *s = va_arg(argp, TCHAR *);
					StringCbCat(output, outputLength, s);
					break;
				}		
				case 's':
				{
					TCHAR *s = va_arg(argp, TCHAR *);
					StringCbCat(output, outputLength, s);
					break;
				}
				case 'g':
				{
					waddr_t waddr = OffsetWaddr(lpCalc->cpu.mem_c, type, zinf->waddr, 2 + (char) va_arg(argp, INT_PTR));
					TCHAR *name;

					name = FindAddressLabel(lpCalc, waddr);
					
					if (name) {
						StringCbCat(output, outputLength, name);
					} else {
						TCHAR szAddr[16];
						StringCbPrintf(szAddr, sizeof(szAddr), _T("$%04X"), waddr.addr);
						StringCbCat(output, outputLength, szAddr);
					}
					break;
				}
				case 'a': //address
					{
						unsigned short addr = zinf->waddr.addr + 2;
						TCHAR *name;
						int val;
						val = (int) va_arg(argp, INT_PTR);

						name = FindAddressLabel(lpCalc, addr_to_waddr(lpCalc->cpu.mem_c, val));
						
						if (name) {
							StringCbCat(output, outputLength, name);
						} else {
							TCHAR szAddr[16];
							StringCbPrintf(szAddr, sizeof(szAddr), _T("$%04X"), val);
							StringCbCat(output, outputLength, szAddr);
						}
						break;
					}
				case 'r':
				{
					TCHAR *szReg = va_arg(argp, TCHAR *);
					if (!_tcscmp(szReg, _T("(hl)"))) {
						StringCbCat(output, outputLength, _T("(hl)"));
					} else
					StringCbCat(output, outputLength, szReg);
					break;
				}
				case 'x':
				{
					int val	= (int) va_arg(argp, INT_PTR);
					TCHAR szAddr[16];
#ifdef WINVER
					StringCbPrintf(szAddr, sizeof(szAddr), _T("$%02X"), val);
#else
					sprintf(szAddr, "$%02X", val);
#endif
					StringCbCat(output, outputLength, szAddr);
					break;	
				}
			}
		}
	}
	va_end(argp);
}