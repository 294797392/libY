#ifndef __TOOLBASE_H__
#define __TOOLBASE_H__

#ifdef WIN32

#ifndef TOOLS_EXPORT
#define TOOLSAPI __declspec(dllexport)
#else
#define TOOLSAPI __declspec(dllimport)
#endif

#else

#define TOOLSAPI

#endif

#endif