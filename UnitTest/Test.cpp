#include <iostream>
#include <gtest\gtest.h>
using namespace std;

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	cout << RUN_ALL_TESTS() << endl;
	getchar();
	return 0;
}