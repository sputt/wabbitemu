#include "stdafx.h"

#include "calc.h"
#include "linksendvar.h"
#include "label.h"

//Sends a file to the given calculator
//from the given filename
LINK_ERR SendFile(const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination)
{
	TIFILE_t *var = importvar(lpszFileName, FALSE);

	LINK_ERR result = LERR_FILE;
	if (var != NULL)
	{
		switch(var->type)
		{
		case GROUP_TYPE:
		case VAR_TYPE:
		case FLASH_TYPE:
			{
				if (var->type == FLASH_TYPE) {
					lpCalc->running = FALSE;
					lpCalc->fake_running = TRUE;
				}
				lpCalc->cpu.pio.link->vlink_size = var->length;
				lpCalc->cpu.pio.link->vlink_send = 0;

				result = link_send_var(&lpCalc->cpu, var, (SEND_FLAG) Destination);
				if (var->type == FLASH_TYPE)
				{
					applist_t applist;
					// Rebuild the applist
					state_build_applist(&lpCalc->cpu, &applist);

					u_int i;
					for (i = 0; i < applist.count; i++) {
						if (_tcsncmp((TCHAR *) var->flash->name, applist.apps[i].name, 8) == 0) {
							lpCalc->last_transferred_app = applist.apps[i];
							break;
						}
					}
					if (var->flash->type == FLASH_TYPE_OS) {
						calc_reset(lpCalc);
						//calc_turn_on(lpCalc);
					}
					lpCalc->running = TRUE;
					lpCalc->fake_running = FALSE;
				}
				break;
			}
		case BACKUP_TYPE:
			lpCalc->cpu.pio.link->vlink_size = var->length;
			lpCalc->cpu.pio.link->vlink_send = 0;
			result = link_send_backup(&lpCalc->cpu, var);
			break;
		case ROM_TYPE:
		case SAV_TYPE:
			{
				FreeTiFile(var);
				var = NULL;
				if (rom_load(lpCalc, lpszFileName) == TRUE) {
					result = LERR_SUCCESS;
				} else {
					result = LERR_LINK;
				}
				break;
			}
		case LABEL_TYPE:
			{
				VoidLabels(lpCalc);
				labels_app_load(lpCalc, lpszFileName);
				result = LERR_SUCCESS;
				break;
			}
		case BREAKPOINT_TYPE:
			break;
		}
		if (var)
		{
			FreeTiFile(var);
		}

		return result;
	}
	else
	{
		return LERR_FILE;
	}
}