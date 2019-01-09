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
					offset += 2;//使得raw[offset]=tagName的第一位字符
					lastToken->value = gstrExcept('>', offset);
					continue;
				}
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = LeftAngleBracket;
				lastToken->offset = offset;
				offset += 1;//使得raw[offset]=tagName的第一位字符
				lastToken->value = gstrExcept(' ', '>', offset);
				while (raw[offset] != '>')
				{
					skipAll(' ', offset);
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeName;
					lastToken->value = gstrExcept('=', ' ', '>', offset);
					skipAll(' ', offset);
					skipAll('=', '"', '\'', offset);

					//html code:
					///<text style="aaa" auto></text>
					//The attribute "auto" will cause a error
					//
					//the next judge is to fix this bug.
					if (raw[offset] == '>') break;

					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeValue;
					lastToken->value = gstrExcept('"', '\'', ' ', '>', offset);
					skipAll('"', '\'', ' ', offset);
				}
			}
			if (raw[offset] == '>')
			{
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = RightAngleBracket;
				lastToken->offset = offset;
			}
			++offset;
		}
		now = root;
	}
	bool MLParser::Parse(string html)
	{
		raw = Cyan::strcpy(html.data());
		Scanner SC = Scanner(raw);
		SC.Scan();
		Token *token = nullptr;
		while ((token = SC.next()) != nullptr)
		{
			std::cout << token->value << std::endl;
		}
		return true;
	}
}