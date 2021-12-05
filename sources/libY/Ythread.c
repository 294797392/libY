#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef Y_API_WIN32
#include <Windows.h>
#elif Y_API_UNIX
#endif

#include "Ybase.h"
#include "Ythread.h"

struct Ythread_s
{
    void *userdata;
    Ythread_entry entry;

#ifdef Y_API_WIN32
    HANDLE handle;
    DWORD threadid;
#else
    pthread_t pthread;
#endif
};

#ifdef Y_API_WIN32
static DWORD WINAPI win32_thread_proc(LPVOID lpThreadParameter)
#else
static void *unix_thread_proc(void *lpThreadParameter)
#endif
{
    Ythread *thread = (Ythread*)lpThreadParameter;

    thread->entry(thread->userdata);

#ifdef Y_API_WIN32
    return 0;
#else
    return NULL;
#endif
}

Ythread *Y_create_thread(Ythread_entry entry, void *userdata)
{
    Ythread *thread = (Ythread*)calloc(1, sizeof(Ythread));
    thread->entry = entry;
    thread->userdata = userdata;

#ifdef Y_API_WIN32
    if((thread->handle = CreateThread(NULL, 0, win32_thread_proc, thread, 0, &thread->threadid)) == NULL)
    {
        free(thread);
        return NULL;
    }
#else
#endif

    return thread;
}
