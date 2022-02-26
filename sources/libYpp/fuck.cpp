#include <string>
#include <iostream>

#include <locale>

using std::string;
using std::wstring;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	wstring s = L"ÄãºÃ123";
	cout << s.length() << endl;

	std::cout << sizeof(wchar_t) << std::endl;

	string c = "ÄãºÃ";
	std::cout << c << std::endl;

	string str1 = "123";
	string* str2 = new string(str1);

	std::cout << str1 << std::endl;
	std::cout << str2->c_str() << std::endl;

	str1.append("AAA");

	std::cout << str1 << std::endl;
	std::cout << str2->c_str() << std::endl;

	return 0;
}