#ifndef _SENDFILE_H
#define _SENDFILE_H

#include "link.h"

LINK_ERR SendFile(const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination);

#endif