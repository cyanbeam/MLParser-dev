#pragma once
#include <iostream>
#include <string>
#include <exception>
using std::string;
//���ﱣ��һЩ��Ч���ǲ���ȫ�Ĵ���
//ʹ����Щ������ҪС��
namespace Cyan
{
	inline char *strcpy(const char *src)
	{
		size_t len = strlen(src) + 1;
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