/*
   miniunz.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
		 
		 Canabalized for parts by BuckeyeDude
*/

#ifndef _WIN32
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef unix
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "miniunz.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

int do_extract_currentfile(unzFile uf, TCHAR *dirToExtractTo)
{
    char filename_inzip[256];
    char* filename_withoutpath;
    char* p;
    int err = UNZ_OK;
    FILE *fout = NULL;
    void *buf;
    uInt size_buf;
	TCHAR write_filename[MAX_PATH];

    unz_file_info64 file_info;
    uLong ratio = 0;
    err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

    if (err != UNZ_OK)
    {
        printf(_T("error %d with zipfile in unzGetCurrentFileInfo\n"), err);
        return err;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void *) malloc(size_buf);
    if (buf == NULL)
    {
        printf(_T("Error allocating memory\n"));
        return UNZ_INTERNALERROR;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p)=='/') || ((*p)=='\\'))
            filename_withoutpath = p+1;
        p++;
    }

	//make sure this exists
	_mkdir(dirToExtractTo);
    strcpy(write_filename, dirToExtractTo);
	strcat(write_filename, _T("/"));
	strcat(write_filename, filename_withoutpath);

    err = unzOpenCurrentFile(uf);
    if (err != UNZ_OK)
    {
        printf(_T("error %d with zipfile in unzOpenCurrentFile\n"), err);
    }

    if (err == UNZ_OK)
    {
        fout = fopen64(write_filename, _T("wb"));

        if (fout == NULL)
        {
            printf(_T("error opening %s\n"), write_filename);
        }
    }

    if (fout != NULL)
    {
        printf(_T(" extracting: %s\n"), write_filename);

        do
        {
            err = unzReadCurrentFile(uf, buf, size_buf);
            if (err  <0)
            {
                printf(_T("error %d with zipfile in unzReadCurrentFile\n"), err);
                break;
            }
            if (err > 0)
                if (fwrite(buf, err, 1, fout) != 1)
                {
                    printf(_T("error in writing extracted file\n"));
                    err = UNZ_ERRNO;
                    break;
                }
        } while (err > 0);
        if (fout)
			fclose(fout);

        /*if (err == 0)
            change_file_date(write_filename,file_info.dosDate, file_info.tmu_date);*/
    }

    if (err == UNZ_OK)
    {
        err = unzCloseCurrentFile (uf);
        if (err != UNZ_OK)
        {
            printf(_T("error %d with zipfile in unzCloseCurrentFile\n"), err);
        }
    }
    else
        unzCloseCurrentFile(uf); /* don't lose the error */

    free(buf);
    return err;
}

int extract_zip(unzFile uf, TCHAR *dirToExtractTo)
{
    uLong i;
    unz_global_info64 gi;
    int err;
    FILE* fout = NULL;

    err = unzGetGlobalInfo64(uf, &gi);
    if (err != UNZ_OK)
        printf(_T("error %d with zipfile in unzGetGlobalInfo \n"), err);

    for (i = 0; i < gi.number_entry; i++)
    {
        if (do_extract_currentfile(uf, dirToExtractTo) != UNZ_OK)
            break;

        if ((i + 1) < gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK)
            {
                printf(_T("error %d with zipfile in unzGoToNextFile\n"), err);
                break;
            }
        }
    }

    return 0;
}