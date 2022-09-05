#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"
#include "Yerrno.h"
#include "Ylist.h"
#include "Ystring.h"
#include "Ydll.h"

typedef struct Ydll_mgr_s
{
	Ylist *dlllist;
}Ydll_mgr;

struct Ydll_s
{
	// dll的完整路径
	YCHAR *path;

#if (defined(Y_WIN32))
	HMODULE *hmodule;
#elif (defined(Y_UNIX))
#endif
};

// dll全局管理器
static Ydll_mgr *dllmgr = NULL;



static int query_dll(Ylist *yl, void *item, void *data, void *userdata)
{
	YCHAR *dll_path = (YCHAR *)data;
	Ydll *dll = (Ydll *)item;

	if(Ystrcmp(dll->path, dll_path) == 0)
	{
		return 1;
	}

	return 0;
}




int Y_load_dll(const YCHAR *dll_path, Ydll **outdll)
{
	if(dllmgr == NULL)
	{
		dllmgr = (Ydll_mgr *)Ycalloc(1, sizeof(Ydll_mgr));
		dllmgr->dlllist = Y_create_list();
	}

	Ydll *dll = (Ydll *)Y_list_query(dllmgr->dlllist, query_dll, dll_path, dllmgr);
	if(dll != NULL)
	{
		return YERR_SUCCESS;
	}

#if (defined(Y_WIN32))
	HMODULE lib = LoadLibrary(dll_path);
#elif (defined(Y_UNIX))
#endif

	if(lib == NULL)
	{
		return YERR_FAILED;
	}

	dll = (Ydll *)Ycalloc(1, sizeof(Ydll));
	dll->path = (YCHAR *)Ystr_copynew(dll_path);
	dll->hmodule = lib;
	
	Y_list_add(dllmgr->dlllist, dll);

	*outdll = dll;

	return YERR_SUCCESS;
}

void Y_unload_dll(Ydll *dll)
{
	if(dllmgr == NULL)
	{
		return;
	}

	Y_list_remove(dllmgr->dlllist, dll, 0);
}

void *Y_load_symbol(Ydll *dll, const char *symbol)
{
#if (defined(Y_WIN32))
	return GetProcAddress(dll->hmodule, (LPCSTR)symbol);
#elif (defined(Y_UNIX))
#endif
}

