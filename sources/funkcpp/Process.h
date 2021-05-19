#ifndef __FUCK_POCESS_H__
#define __FUCK_POCESS_H__

#include <string>

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
		string FileName;
		bool UseShellExecute;
		bool RedirectStandardError;
		bool RedirectStandardOutput;
		bool RedirectStandardInput;
		bool CreateNoWindow;
		string Arguments;
		ProcessWindowStyle WindowStyle;
	};

public:
	Process();
	~Process();


public:
	static Process* Start(ProcessStartInfo& startInfo);


private:

};


#endif