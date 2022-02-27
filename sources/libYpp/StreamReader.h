#ifndef __STREAMREADER_H__	
#define __STREAMREADER_H__	

#include <Windows.h>

#include "String.h"

class StreamReader 
{
public:
	StreamReader(HANDLE handle);
	~StreamReader();

private:

public:
	String ReadLine();
	void Close();
};

#endif