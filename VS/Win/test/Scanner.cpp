#include "Scanner.hpp"
namespace Cyan
{
		void Scanner::Scan()
	{
		size_t offset = 0;
		Token *lastToken = root;
		size_t len = strlen(raw);
		while (true)
		{
			if (offset >= len) break;
			if (raw[offset] == '<')
			{
				if (raw[offset + 1] == '/')
				{
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = EndTag;
					lastToken->offset = offset;
					offset += 2;//ʹ��raw[offset]=tagName�ĵ�һλ�ַ�
					lastToken->value = gstrExcept('>', offset);
					str2lwr(lastToken->value);
					continue;
				}
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = LeftAngleBracket;
				lastToken->offset = offset;
				offset += 1;//ʹ��raw[offset]=tagName�ĵ�һλ�ַ�
				lastToken->value = gstrExcept(' ', '/', '>', offset);
				str2lwr(lastToken->value);
				while (raw[offset] != '>')
				{
					skipAll(' ', offset);
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeName;
					lastToken->value = gstrExcept('=', ' ', '>', offset);
					skipAll(' ','=', offset);
					//skipAll('=', offset);

					//�õ���˫�������������ַ�������ܳ��ֿո�Ϊ����ȷ�ָ�
					//Ӧ�����ٴ���������˫���ŵ�ʱ��ֹͣ��������һ���е��� �ո񡢵�˫���� ��ֱ��ֹͣ
					//Ҳ����ʵ�� ���ַ�����Ϊһ���������ָ��ʹ�ַ����ڰ����ո񣨻�˫���ţ�
					char flag = raw[offset];//��ǰ�ַ��ǵȺ�(=)�ұߵĵ�һ���ǿո��ַ�  name = ["]value"
					if (flag == '"' || flag == '\'') ++offset;//������������ţ���ô������ָ��������
					//html code:
					///<text style="aaa" autofocus></text>
					//The attribute "autofocus" will cause a error
					//
					//the next judge is to fix this bug.
					if (flag == '>') break;
					
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeValue;
					if (flag == '"' || flag == '\'')
						lastToken->value = gstrExcept(flag, offset);
					else
						lastToken->value = gstrExcept(' ', '>', offset);
					skipAll('"', '\'', ' ', offset);
					skipAll('/', offset);
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
}