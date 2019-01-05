#include "MLParser.h"
#include <gtest\gtest.h>
namespace Cyan
{
	TEST(Scanner_Test, GetAttributeValue_Test)
	{
		Scanner *sc = new Scanner("=aaaabbb >");
		size_t offset = 0;
		ASSERT_EQ("aaaabbb", sc->GetAttributeValue(offset));
		delete sc;
	}
}