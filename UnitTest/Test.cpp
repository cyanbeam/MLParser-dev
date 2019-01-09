#include "MLParser.h"
#include <iostream>
//#include <gtest\gtest.h>
using namespace std;
using namespace Cyan;
int main(int argc, char **argv)
{
	MLParser *ml = new MLParser();
	ml->Parse("<html><head><title>aaaaaaa</title></head><body style=\"aaaa\" auto>mmmm</body></html>");

	getchar();
	return 0;
}