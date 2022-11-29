#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
#include <Windows.h>
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
#include <pthread.h>
#endif

#include "libY.h"

struct Ythread_s
{
    void *userdata;
    Ythread_entry entry;

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
    HANDLE handle;
    DWORD threadid;
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
    pthread_t threadid;
#endif
};

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
static DWORD WINAPI win32_thread_proc(LPVOID lpThreadParameter)
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
static void *unix_thread_proc(void *lpThreadParameter)
#endif
{
    Ythread *thread = (Ythread*)lpThreadParameter;

    thread->entry(thread->userdata);

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
    return 0;
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
    return NULL;
#endif
}

Ythread *Y_create_thread(Ythread_entry entry, void *userdata)
{
    Ythread *thread = (Ythread *)calloc(1, sizeof(Ythread));
    thread->entry = entry;
    thread->userdata = userdata;

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
    if ((thread->handle = CreateThread(NULL, 0, win32_thread_proc, thread, 0, &thread->threadid)) == NULL)
    {
        free(thread);
        return NULL;
    }
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
    pthread_create(&thread->threadid, NULL, unix_thread_proc, thread);
    pthread_detach(thread->threadid);
#endif

    return thread;
}

void Y_delete_thread(Ythread *thread)
{
#if (defined(Y_WIN32)) || (defined(Y_MINGW))
    // Windows下使用WaitForSingleObject等待线程运行结束
    WaitForSingleObject(thread->handle, INFINITE);
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
    pthread_join(thread->threadid, NULL);
#endif
    free(thread);
}