#ifndef SENDFILES_H
#define SENDFILES_H

#include "calc.h"

BOOL SendFileToCalc(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, BOOL fAsync, SEND_FLAG destination = SEND_CUR);
void CancelFileThreadSend(const LPCALC lpCalc);
void WaitForCalcFileSendThread(const LPCALC lpCalc);

#endif
