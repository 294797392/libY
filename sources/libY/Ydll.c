#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"
#include "Yerrno.h"
#include "Ylist.h"
#include "Ydll.h"

typedef struct Ydll_mgr_s
{
	Ylist *dlllist;
}Ydll_mgr;

static Ydll_mgr *dllmgr = NULL;

int Y_load_dll(const YCHAR *dll_path, Ydll **dll)
{
	HMODULE lib = LoadLibrary(dll_path);
	if(lib == NULL)
	{
		return YERR_FAILED;
	}
}

void Y_unload_dll(Ydll *dll)
{

}
