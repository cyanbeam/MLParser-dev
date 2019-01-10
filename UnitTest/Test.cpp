#include "MLParser.h"
#include <iostream>
//#include <gtest\gtest.h>
using namespace std;
using namespace Cyan;
int main(int argc, char **argv)
{
	MLParser *ml = new MLParser();
	ml->Parse("<html><head><title>aaaaaaa</title><link src='http://baidu.com'></head><body style=\"aaaa\" autofocus>mmmm</body></html>");

	getchar();
	return 0;
}