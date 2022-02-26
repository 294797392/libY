#ifndef __STRING_H__
#define __STRING_H__


#include <string>

using std::wstring;

class String :public wstring
{
public:
	String();
	~String();

private:
};

String::String()
{
}

String::~String()
{
}

#endif