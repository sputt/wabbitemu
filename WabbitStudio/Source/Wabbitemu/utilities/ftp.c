#include "stdafx.h"
#include "ftp.h"

#ifdef _WINDOWS

HINTERNET OpenFtpConnection() {
	HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	hInternet = InternetConnect(hInternet, _T("buckeyedude.zapto.org"), INTERNET_DEFAULT_FTP_PORT,
		_T("wabbitemubug"), _T("wabbitemu"), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL);
	return hInternet;
}

#endif