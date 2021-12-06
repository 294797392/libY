#ifndef __FUCK_POCESS_H__
#define __FUCK_POCESS_H__

#include <string>

#include "String.h"
#include "StreamReader.h"
#include "fuckcpp.h"

using std::string;

class Process
{
public:
	enum ProcessWindowStyle
	{
		Normal = 0,
		Hidden,
		Minimized,
		Maximized
	};

	struct ProcessStartInfo
	{
	public:
		String FileName;
		bool UseShellExecute;
		bool RedirectStandardError;
		bool RedirectStandardOutput;
		bool RedirectStandardInput;
		bool CreateNoWindow;
		String Arguments;
		ProcessWindowStyle WindowStyle;
	};


private:
	ProcessStartInfo startInfo;
	HANDLE inRead = NULL;
	HANDLE inWrite = NULL;
	HANDLE outRead = NULL;
	HANDLE outWrite = NULL;
	PROCESS_INFORMATION procInfo;

public:
	Process(ProcessStartInfo& startInfo);
	~Process();

public:
	StreamReader* StandardInput;
	StreamReader* StandardOutput;

private:

};


#endif