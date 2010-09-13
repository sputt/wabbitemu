#include "stdafx.h"

#include "print.h"
#include "label.h"
#include "calc.h"
#include "core.h"


#define press_text(sztext, zcolor) press_textA(sztext, zcolor, &r, hdc)

static BOOL calc_size = FALSE;
static size_t mspf_size = 0;
static int mspf_break = 9999;

void press_textA(char *szText, COLORREF zcolor, RECT *r, HDC hdc) {
	RECT tr;
	tr.left = 0; tr.right = 1;
	SetTextColor(hdc, zcolor);
	DrawText(hdc, szText, -1, &tr, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
	r->right = r->left + tr.right;
	
	size_t index = mspf_size;
	mspf_size += (int) strlen(szText);
	if (calc_size == FALSE) {
		const char *dot_strings[] = {".", "..", "..."};
		char szNew[1024];
		
		if (index >= mspf_break || (index < mspf_break && index+strlen(szText) > mspf_break)) {
			int break_index = (int) (max(index, mspf_break));
			int break_string_index = break_index - (int) index;
			int str_left = (int) strlen(&szText[break_string_index]);
			
			if (str_left > 3)
				str_left = 3;

			if (index > mspf_break)
				str_left -= (int) (index - mspf_break);
			
			if (str_left < 1)
				str_left = 1;
			
#ifdef WINVER
			strcpy_s(szNew, szText);
			strcpy_s(&szNew[break_string_index], strlen(dot_strings[str_left-1]) + 1, dot_strings[str_left-1]);
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

void mysprintf(HDC hdc, Z80_info_t* zinf, RECT *rc, const char *fmt, ...) {
    char *p;
    va_list argp;
    RECT r = *rc;
    
    mspf_size = 0;
    mspf_break = 999;
    
    if (calc_size == FALSE) {
    	calc_size = TRUE;
    	
    	mysprintf(hdc, zinf, rc, fmt, zinf->a1, zinf->a2, zinf->a3, zinf->a4);
    	
    	char szFilltext[1024];
    	memset(szFilltext, 'A', mspf_size);
    	szFilltext[mspf_size] = '\0';

    	RECT hr;
    	CopyRect(&hr, rc);
    	DrawText(hdc, szFilltext, -1, &hr, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_CALCRECT | DT_END_ELLIPSIS | DT_MODIFYSTRING);
    	mspf_break = (int) strlen(szFilltext);

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
    for (p = (char*) fmt; *p && (mspf_size < mspf_break+3); p++) {
        if(*p != '%') {
            char szChar[2] = "x";
            szChar[0] = *p;
            press_text(szChar, DBCOLOR_BASE);
        } else {
            switch(*++p) {
                case 'c': {//condition
					char *s = va_arg(argp, char *);
					press_text(s, DBCOLOR_CONDITION);
	            	break;
                }
                case 'h': {//offset
                	int val	= (int) va_arg(argp, INT_PTR);
                	char szOffset[8];
#ifdef WINVER
					sprintf_s(szOffset, "%+d",val);
#else
                	sprintf(szOffset, "%+d",val);
#endif
                	press_text(szOffset, RGB(0, 0, 0));
					break;
                }
                case 'd': //number
				{
					int val	= (int) va_arg(argp, INT_PTR);
					char szAddr[16];
#ifdef WINVER
					sprintf_s(szAddr, "%d",val);
#else
					sprintf(szAddr, "%d",val);
#endif
					press_text(szAddr, RGB(0, 0, 0));		
                	break;
				}
				case 'l':
				{
					char *s = va_arg(argp, char *);
					press_text(s, RGB(0, 0, 0));
                	break;
				}		
                case 's':
				{
					char *s = va_arg(argp, char *);
					press_text(s, DBCOLOR_BASE);
                	break;
				}
                case 'g':
				{
					unsigned short addr = zinf->addr + 2;
					char *name;
					int val;
					
					val = (addr + ((char) va_arg(argp, INT_PTR)));
					name = FindAddressLabel(gslot,calcs[gslot].cpu.mem_c->banks[(val>>14)&0x03].ram, calcs[gslot].cpu.mem_c->banks[(val>>14)&0x03].page,val);
					
					if (name) {
						press_text(name, RGB(0, 0, 0));
					} else {
						char szAddr[16];
#ifdef WINVER
						sprintf_s(szAddr, "$%04X",val);
#else
						sprintf(szAddr, "$%04X",val);
#endif
						press_text(szAddr, RGB(0, 0, 0));
					}
                	break;
				}
				case 'a': //address
					{
						unsigned short addr = zinf->addr + 2;
						char *name;
						int val;
						val = (int) va_arg(argp, INT_PTR);

						name = FindAddressLabel(gslot,calcs[gslot].cpu.mem_c->banks[(val>>14)&0x03].ram, calcs[gslot].cpu.mem_c->banks[(val>>14)&0x03].page,val);
						
						if (name) {
							press_text(name, RGB(0, 0, 0));
						} else {
							char szAddr[16];
#ifdef WINVER
							sprintf_s(szAddr, "$%04X",val);
#else
							sprintf(szAddr, "$%04X",val);
#endif
							press_text(szAddr, RGB(0, 0, 0));
						}
	                	break;
					}
                case 'r':
				{
					char *szReg = va_arg(argp, char *);
					if (!strcmp(szReg, "(hl)")) {
						press_text("(", DBCOLOR_BASE);
						press_text("hl", DBCOLOR_HILIGHT);
						press_text(")", DBCOLOR_BASE);
					} else
					press_text(szReg, DBCOLOR_HILIGHT);
					break;
				}
				case 'x':
				{
					int val	= (int) va_arg(argp, INT_PTR);
					char szAddr[16];
#ifdef WINVER
					sprintf_s(szAddr, "$%02X", val);
#else
					sprintf(szAddr, "$%02X", val);
#endif
					press_text(szAddr, RGB(0, 0, 0));	
					break;	
				}
            }
        }
    }
    va_end(argp);
}
