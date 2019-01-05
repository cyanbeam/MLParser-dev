#pragma once
#include <iostream>
#include <string>
using std::string;
//这里保存一些有效但是不安全的代码
//使用这些函数需要小心
namespace Cyan
{
	inline char *strcpy(const char *src)
	{
		size_t len = sizeof(src);
		char *t = new char[len];
		memcpy(t, src, len);
		return t;
	}
	inline string substr(const char *src, size_t count)
	{
		char *t = new char[count + 1]();
		memcpy(t, src, count);
		string s = string(t);
		delete[] t;
		return s;
	}
}