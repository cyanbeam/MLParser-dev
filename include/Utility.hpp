#pragma once
#include <iostream>
namespace Cyan
{
	char *strcpy(const char *src)
	{
		size_t len = sizeof(src);
		char *t = new char[len];
		memcpy(t, src, len);
		return t;
	}
}