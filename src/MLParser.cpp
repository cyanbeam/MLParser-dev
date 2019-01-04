#include "MLParser.h"
namespace Cyan
{
	void Scanner::Scan()
	{
		size_t offset = 0;
		Token *lastToken = root;
		while (true)
		{
			if (raw[offset] == '\0') break;
			if (raw[offset] == '<')
			{
				if (raw[offset + 1] == '/')
				{
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = EndTag;
					lastToken->value = GetEndTagName(offset);
				}
				//////////
			}
		}
		now = root;
	}
}