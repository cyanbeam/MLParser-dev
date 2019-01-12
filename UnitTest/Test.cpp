#include "MLParser.h"
#include <iostream>
//#include <gtest\gtest.h>
using namespace std;
using namespace Cyan;
int main(int argc, char **argv)
{
	char buffer[256];
	FILE *fp;
	fopen_s(&fp, "G:\\test3.html", "rb+");
	string s;
	size_t n;
	memset(buffer, '\0', 256);
	while ((n = fread_s(buffer, 256, 255, 1, fp)) >= 0)
	{
		if (strlen(buffer) == 0) break;
		s += buffer;
		memset(buffer, '\0', 256);
	}
	fclose(fp);
	MLParser ml;
	ml.Parse(s);
	ml.PrintTree(true);
	ml.Dispose();
	getchar();
	return 0;
}