#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <Windows.h>

static void test(wchar_t *str)
{}

int main(int argc, char **argv)
{
	wchar_t *text = L"123";
	fwprintf(stdout, text);

return 0;
}
