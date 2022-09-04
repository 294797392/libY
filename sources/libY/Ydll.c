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

struct Ydll_s
{
	// dll的完整路径
	YCHAR *path;
};

// dll全局管理器
static Ydll_mgr *dllmgr = NULL;



static int query_dll(Ylist *yl, void *item, void *data, void *userdata)
{

}




int Y_load_dll(const YCHAR *dll_path)
{
	if(dllmgr == NULL)
	{
		dllmgr = (Ydll_mgr *)Ycalloc(1, sizeof(Ydll_mgr));
		dllmgr->dlllist = Y_create_list();
	}

	Ydll *dll = Y_list_query(dllmgr->dlllist, query_dll, dll_path, dllmgr);
	if(dll == NULL)
	{
		HMODULE lib = LoadLibrary(dll_path);
		if(lib == NULL)
		{
			return YERR_FAILED;
		}

		dll = (Ydll *)Ycalloc(1, sizeof(Ydll));
		dll->path = Ycalloc()
	}

	return YERR_SUCCESS;
}

void Y_unload_dll(Ydll *dll)
{

}
