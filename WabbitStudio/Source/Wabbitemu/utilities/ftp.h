#ifndef FTP_H
#define FTP_H

#ifdef WINVER
#include <WinInet.h>
#endif

HINTERNET OpenFtpConnection();

#endif