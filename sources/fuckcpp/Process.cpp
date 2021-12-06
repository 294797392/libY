#include <string>

#include <Windows.h>

#include "Process.h"


Process::Process(ProcessStartInfo& startInfo)
{
	this->startInfo = startInfo;

	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));

	// Set the bInheritHandle flag so pipe handles are inherited.
	SECURITY_ATTRIBUTES security;
	security.nLength = sizeof(SECURITY_ATTRIBUTES);
	security.bInheritHandle = TRUE;
	security.lpSecurityDescriptor = NULL;

	if (this->startInfo.RedirectStandardInput)
	{
		// Create a pipe for the child process's STDIN
		if (!CreatePipe(&this->inRead, &this->inWrite, &security, 0))
		{
		}

		// Ensure the write handle to the pipe for STDIN is not inherited
		if (!SetHandleInformation(this->inWrite, HANDLE_FLAG_INHERIT, 0))
		{
		}
	}

	if (this->startInfo.RedirectStandardOutput)
	{
		// Create a pipe for the child process's STDOUT
		if (!CreatePipe(&this->outRead, &this->outWrite, &security, 0))
		{
		}

		// Ensure the read handle to the pipe for STDOUT is not inherited
		if (!SetHandleInformation(this->outRead, HANDLE_FLAG_INHERIT, 0))
		{
		}
	}

	if (!CreateProcess(
		NULL,
		(const LPWSTR)this->startInfo.Arguments.c_str(),
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&this->procInfo))
	{
		// error
	}

	this->StandardInput = new StreamReader();
}

Process::~Process()
{}
