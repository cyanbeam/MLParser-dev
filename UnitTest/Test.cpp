#include "MLParser.h"
#include <iostream>
//#include <gtest\gtest.h>
using namespace std;
using namespace Cyan;
int main(int argc, char **argv)
{
	//testing::InitGoogleTest(&argc, argv);
	//cout << RUN_ALL_TESTS() << endl;
	Scanner *sc = new Scanner("<html><head><title>aaaaaaa</title></head><body style=\"aaaa\" auto>mmmm</body></html>");
	sc->Scan();
	delete sc;

	getchar();
	return 0;
}