#include "MLParser.h"
#include <iostream>

//#define CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>  
//#include <gtest\gtest.h>
using namespace std;
using namespace Cyan;
int main(int argc, char **argv)
{
	//_CrtSetBreakAlloc(188);
	char buffer[256];
	FILE *fp;
	fopen_s(&fp, "E:\\test.html", "rb+");
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
	ml.PrintTree(false);
	Results rs = ml.SearchByTagName("span");
	for (auto result : rs)
	{
		cout << result.GetMLParser()["em"].GetInner() << endl;
	}
	ml.Dispose();
	getchar();
	//_CrtDumpMemoryLeaks();
	return 0;
}