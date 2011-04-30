#ifndef MINIUNZ_H
#define MINIUNZ_H

#include "unzip.h"

//extracts zip file to dir specified dir
int extract_zip(unzFile uf, TCHAR *dirToExtract);

#endif //MINIUNZ_H