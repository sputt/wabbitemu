#ifndef SENDFILES_H
#define SENDFILES_H

#include "calc.h"

BOOL SendFileToCalc(const LPCALC lpCalc, LPCTSTR lpszFileName, BOOL fAsync, SEND_FLAG destination = SEND_CUR);

#endif
